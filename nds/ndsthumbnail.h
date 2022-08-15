#ifndef _NDSCREATOR_H
#define _NDSCREATOR_H

#include <kio/thumbcreator.h>

class NDSThumbnail : public ThumbCreator
{
public:
	NDSThumbnail() = default;
	~NDSThumbnail() override = default;
	bool create(const QString &path, int width, int height, QImage &img) override;
};

#endif // _NDSCREATOR_H