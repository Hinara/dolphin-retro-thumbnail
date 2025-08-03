#ifndef _NDSCREATOR_H
#define _NDSCREATOR_H

#include <KIO/ThumbnailCreator>

class GCNThumbnail : public KIO::ThumbnailCreator
{
public:
	GCNThumbnail(QObject *parent, const QVariantList &args);
	~GCNThumbnail() override = default;
	KIO::ThumbnailResult create(const KIO::ThumbnailRequest &request) override;
};

#endif // _NDSCREATOR_H