#include "ndsthumbnail.h"
#include <KPluginFactory>
#include <QFile>
#include <QImage>
#include <QtEndian>
#include <QVector>
#include <stdint.h>


K_PLUGIN_CLASS_WITH_JSON(NDSThumbnail, "ndsthumbnail.json")

NDSThumbnail::NDSThumbnail(QObject *parent, const QVariantList &args)
    : KIO::ThumbnailCreator(parent, args)
{
}

struct NDSBanner
{
    uint16_t version;
    uint16_t crc16[4];
    uint8_t reserved1[22];
    uint8_t bitmap[512];
    uint16_t palette[16];

    char16_t jp_title[128];
    char16_t en_title[128];
    char16_t fr_title[128];
    char16_t de_title[128];
    char16_t it_title[128];
    char16_t es_title[128];
    char16_t cn_title[128];
    char16_t kr_title[128];

    uint8_t reserved2[2048];
};

static_assert(sizeof(NDSBanner) == 4672, "NDSBanner is not 4672 bytes!");

static const size_t BANNER_OFFSET = 0x68;

static uint32_t RGB555_to_ARGB32(uint16_t value)
{

    return 0xFF000000 |
        (value & 0x001F) << 19 |
        (value & 0x03E0) << 6 |
        (value & 0x7C00) >> 7;
}

static KIO::ThumbnailResult createImageFromBanner(const NDSBanner &banner, QSize size)
{
    auto smallest = size.width() < size.height() ? size.width() : size.height();
    if (banner.version != 0x0001 &&
        banner.version != 0x0002 &&
        banner.version != 0x0003 &&
        banner.version != 0x0103) {
        qWarning("Unknown NDS banner version");
        return KIO::ThumbnailResult::fail();
    }
    QVector<uint32_t> palette(16);
    palette[0] = 0x00000000;
    for (size_t i = 1; i < 16; i++) {
        palette[i] = RGB555_to_ARGB32(banner.palette[i]);
    }
    QByteArray pixels(sizeof(uint32_t) * 32 * 32, 0xFF);
    for (size_t i = 0; i < 32 * 32; i++) {
        pixels.data()[i * 4 + 0] = 0xFF;
        pixels.data()[i * 4 + 1] = 0x00;
        pixels.data()[i * 4 + 2] = 0x00;
        pixels.data()[i * 4 + 3] = 0xFF;
    }
    size_t pos = 0;
    for (size_t j = 0; j < 4; j++) {
        for (size_t i = 0; i < 4; i++) {
            for (size_t y = 0; y < 8; y++) {
                for (size_t x = 0; x < 4; x++) {
                    uint8_t tile_data = banner.bitmap[pos];
                    size_t bx = x * 2 + i * 8;
                    size_t by = y + j * 8;
                    size_t base = bx + by * 32;
                    memcpy(pixels.data() + (0 + base) * sizeof(uint32_t), palette.data() + ((tile_data & 0x0F) >> 0), sizeof(uint32_t));
                    memcpy(pixels.data() + (1 + base) * sizeof(uint32_t), palette.data() + ((tile_data & 0xF0) >> 4), sizeof(uint32_t));
                    pos++;
                }
            }
        }
    }
    QImage img = QImage((const uchar*)pixels.constData(), 32, 32, 32 * sizeof(uint32_t), QImage::Format_ARGB32);
    int base = 32;
    while (base < smallest) {
        base <<= 1;
    }
    img = img.scaledToWidth(base);
    return KIO::ThumbnailResult::pass(img);
}

static bool getNDSBanner(const QUrl &url, NDSBanner &banner)
{
    QFile file (url.toLocalFile());

    quint32 offset;
    QByteArray offset_array(sizeof(offset), '\0');
    QByteArray banner_array(sizeof(NDSBanner), '\0');
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open file");
        return false;
    }
    QDataStream ndsStream;
    ndsStream.setDevice(&file);
    if (ndsStream.skipRawData(BANNER_OFFSET) != BANNER_OFFSET ||
        ndsStream.readRawData(offset_array.data(), sizeof(offset)) != sizeof(offset)
    ) {
        qWarning("Cannot read NDS header");
        ndsStream.device()->close();
        return false;
    }
    offset = qFromLittleEndian<quint32>(offset_array);
    if (offset < BANNER_OFFSET + sizeof(offset) ||
        ndsStream.skipRawData(offset - (BANNER_OFFSET + sizeof(offset))) != offset - (BANNER_OFFSET + sizeof(offset)) ||
        ndsStream.readRawData(banner_array.data(), sizeof(banner)) != sizeof(banner)
    ) {
        qWarning("Cannot read NDS banner");
        ndsStream.device()->close();
        return false;
    }
    ndsStream.device()->close();
    memcpy(&banner, banner_array.data(), sizeof(banner));
    return true;
}


KIO::ThumbnailResult NDSThumbnail::create(const KIO::ThumbnailRequest &request)
{
    NDSBanner banner;

    if (!getNDSBanner(request.url(), banner)) {
        return KIO::ThumbnailResult::fail();
    }
    return createImageFromBanner(banner, request.targetSize());
}

#include "ndsthumbnail.moc"