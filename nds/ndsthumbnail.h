#ifndef _NDSCREATOR_H
#define _NDSCREATOR_H

#include <KIO/ThumbnailCreator>

class NDSThumbnail : public KIO::ThumbnailCreator
{
public:
	NDSThumbnail(QObject *parent, const QVariantList &args);
	~NDSThumbnail() override = default;
	KIO::ThumbnailResult create(const KIO::ThumbnailRequest &request) override;
};

#endif // _NDSCREATOR_H