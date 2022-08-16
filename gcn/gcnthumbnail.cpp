#include "gcnthumbnail.h"
#include <QFile>
#include <QImage>
#include <QtEndian>
#include <QVector>
#include <string.h>
#include <iostream>

extern "C"
{
    Q_DECL_EXPORT ThumbCreator *new_creator()
    {
        return new GCNThumbnail();
    }
};

struct Buffer {
    uchar *start;
    uchar *end;
    bool contains(uchar *ptr) const { return start <= ptr && ptr <= end; }
    bool contains(Buffer &b) const { return start <= b.start && b.end <= end; }
    size_t size() const { return end - start; }
};

static bool mapFile(QFile &file, Buffer &fileBuffer)
{
    qint64 size = file.size();
    if (size < 0x440) {
        qWarning("Invalid file: to small for header");
        return false;
    }
    uchar *base = file.map(0, size);
    if (base == nullptr) {
        qWarning("Failed to map file");
        return false;
    }
    fileBuffer.start = base;
    fileBuffer.end = fileBuffer.start + size;
    return true;
}

static bool getFST(const Buffer &fileBuffer, Buffer &fst)
{
    quint32 fstOffset = qFromBigEndian<quint32>(fileBuffer.start + 0x0424);
    quint32 fstSize = qFromBigEndian<quint32>(fileBuffer.start + 0x0428);
    fst.start = fileBuffer.start + fstOffset;
    fst.end = fst.start + fstSize;
    if (!fileBuffer.contains(fst)) {
        qWarning("Invalid file: to small for FST");
        return false;
    }
    return true;
}

static bool getFile(const char *to_find, const Buffer &fileBuffer, const Buffer &fst, Buffer &file)
{

    quint32 fileCount = qFromBigEndian<quint32>(fst.start + 8);
    qInfo("File count: %x", fileCount);
    if (!fst.contains(fst.start + fileCount * 0xc)) {
        qWarning("File description bigger thant FST size");
        return false;
    }
    char *stringTableBase = (char *) (fst.start + fileCount * 0xc);
    if (!fst.contains((uchar*) stringTableBase)) {
        qWarning("String table start outside FST");
        return false;
    }
    for (size_t i = 1; i < fileCount; i++) {
        uchar *offset = fst.start + i * 0xc;
        quint32 type_name = qFromBigEndian<quint32>(offset + 0);
        quint32 type = (type_name & 0xFF000000) >> 24;
        quint32 nameOffset = type_name & 0x00FFFFFF;
        quint32 fileOffset = qFromBigEndian<quint32>(offset + 4);
        quint32 length = qFromBigEndian<quint32>(offset + 8);
        char *name = stringTableBase + nameOffset;
        if (!fst.contains((uchar *) name)) {
            qWarning("Filename outside of FST");
            return false;
        }
        if (strcasecmp(to_find, name) == 0) {
            file.start = fileBuffer.start + fileOffset;
            file.end = file.start + length;
            if (!fileBuffer.contains(file)) {
                qWarning("File is outside disk boundaries");
                return false;
            }
            qInfo("Type:%x Name:%s File offset:%u Length:%u", type, stringTableBase + nameOffset, fileOffset, length);
            return true;
        }
    }
    qDebug("File not found %s", to_find);
    return false;
}

struct Banner {
    char magic[4];
    char padding[0x1c];
    uint16_t graphic_data[96*32];
    char name[0x20];
    char company[0x20];
    char full_name[0x40];
    char full_company[0x40];
    char desc[0x80];
};
static_assert(sizeof(Banner) == 0x1960);


static quint32 RGB555A1_to_ARGB32(quint16 value)
{
    quint32 res = 0;
    res |= quint32(value & 0x8000) ? 0xFF000000 : 0;
    res |= quint32(value & 0x7B00) << 9; // R
    res |= quint32(value & 0x03E0) << 6; // G
    res |= quint32(value & 0x001F) << 3; // B
    return res;
}

bool fillImageFromBanner(const Buffer &bannerFile, int width, int height, QImage &img)
{
    if (bannerFile.size() < sizeof(Banner)) {
        qWarning("Banner file is too small");
        return false;
    }
    Banner *banner = reinterpret_cast<Banner *>(bannerFile.start);
    if (strcmp(banner->magic, "BNR1") != 0 && strcmp(banner->magic, "BNR2") != 0) {
        qWarning("Invalid magic word for banner file");
        return false;
    }
    QByteArray pixels(sizeof(quint32) * 96 * 32, 0x00);

    size_t pos = 0;
	for (size_t y = 0; y < 8; y++) {
		for (size_t x = 0; x < 24; x++) {
	        for (size_t j = 0; j < 4; j++) {
		        for (size_t i = 0; i < 4; i++) {
                    quint32 color = RGB555A1_to_ARGB32(qFromBigEndian<quint16>(banner->graphic_data[pos]));
                    size_t bx = x * 4 + i;
                    size_t by = y * 4 + j;
                    size_t base = bx + by * 96;
                    memcpy(pixels.data() + base * sizeof(uint32_t), &color, sizeof(uint32_t));
					pos++;
				}
			}
		}
	}
    img = QImage((const uchar*)pixels.constData(), 96, 32, 96 * sizeof(uint32_t), QImage::Format_ARGB32);
    int smallest = width < height * 3 ? width : height * 3;
    int base = 96;
    while (base < smallest) {
        base <<= 1;
    }
    img = img.scaledToWidth(base);
    return true;
}

bool GCNThumbnail::create(const QString &path, int width, int height, QImage &img)
{
    QFile file (path);

    if(!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open file");
        return false;
    }
    Buffer fileBuffer;
    Buffer fst;
    Buffer banner;
    if (!mapFile(file, fileBuffer)) {
        return false;
    }
    if (!getFST(fileBuffer, fst)) {
        return false;
    }
    if (!getFile("opening.bnr", fileBuffer, fst, banner)) {
        return false;
    }
    if (!fillImageFromBanner(banner, width, height, img)) {
        return false;
    }
    qInfo("Success");
    return true;
}