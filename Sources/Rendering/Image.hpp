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
	static std::shared_ptr<Image> LoadImage(const char* path);
	int Width() const { return m_width; }
	int Height() const { return m_height; }
	int Channels() const { return m_channels; }
	unsigned char *Pixels() const { return m_pixels; }
	unsigned short MipLevels() const { return m_mipLevels; }
	Image();
	~Image();
};
