#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED


#include <memory>

class Image
{
protected:
	int _width;
	int _height;
	int _channels;
	unsigned char *_pixels;
	void load_image(const char *path);
public:
	int width() const { return _width; }
	int height() const { return _height; }
	int channels() const { return _channels; }
	unsigned char *pixels() const { return _pixels; }
	Image(const char *path);
	~Image();
};
#endif // IMAGE_H_INCLUDED