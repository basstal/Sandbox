#pragma once

#include <memory>

class Image
{
protected:
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	unsigned char* m_pixels = nullptr;
	float* m_pixelsHdr = nullptr;
	unsigned short m_mipLevels = 1;

public:
	static std::shared_ptr<Image> LoadImage(const char* path);
	static std::shared_ptr<Image> LoadHdrImage(const char* path);
	int Width() const { return m_width; }
	int Height() const { return m_height; }
	int Channels() const { return m_channels; }
	unsigned char *Pixels() const { return m_pixels; }
	float *PixelsHdr() const { return m_pixelsHdr; }
	unsigned short MipLevels() const { return m_mipLevels; }
	bool IsHdr() const { return m_pixelsHdr != nullptr; }
	Image();
	~Image();
};
