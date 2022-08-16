#ifndef _NDSCREATOR_H
#define _NDSCREATOR_H

#include <kio/thumbcreator.h>

class GCNThumbnail : public ThumbCreator
{
public:
	GCNThumbnail() = default;
	~GCNThumbnail() override = default;
	bool create(const QString &path, int width, int height, QImage &img) override;
};

#endif // _NDSCREATOR_H