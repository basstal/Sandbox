#pragma once

#include <memory>

class Image
{
protected:
	int m_width;
	int m_height;
	int m_channels;
	unsigned char* m_pixels;
	unsigned short m_mipLevels;

public:
	static std::shared_ptr<Image> loadImage(const char* path);
	int width() const { return m_width; }
	int height() const { return m_height; }
	int channels() const { return m_channels; }
	unsigned char* pixels() const { return m_pixels; }
	unsigned short mipLevels() const { return m_mipLevels; }
	Image();
	~Image();
};
