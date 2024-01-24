#pragma once

#include <memory>

class Image
{
protected:
	int _width;
	int _height;
	int _channels;
	unsigned char *_pixels;
	unsigned short _mipLevels;
public:
	static std::shared_ptr<Image> loadImage(const char *path);
	int width() const { return _width; }
	int height() const { return _height; }
	int channels() const { return _channels; }
	unsigned char *pixels() const { return _pixels; }
	unsigned short mipLevels() const { return _mipLevels; }
	Image();
	~Image();
};