#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <cmath>

#include "Base/Device.hpp"
#include "Components/Buffer.hpp"

Image::Image()
{
}

Image::~Image()
{
	if (m_pixels != nullptr)
	{
		stbi_image_free(m_pixels);
	}
	if (m_pixelsHdr != nullptr)
	{
		stbi_image_free(m_pixelsHdr);
	}
}

std::shared_ptr<Image> Image::LoadImage(const char* path)
{
	std::shared_ptr<Image> image = std::make_shared<Image>();
	stbi_uc* pixels = stbi_load(path, &image->m_width, &image->m_height, &image->m_channels, STBI_rgb_alpha);
	//std::cout<< "load image finished" << std::endl;
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	image->m_pixels = pixels;
	image->m_mipLevels = static_cast<unsigned short>(std::floor(std::log2(std::max(image->m_width, image->m_height)))) + 1;
	return image;
}

std::shared_ptr<Image> Image::LoadHdrImage(const char* path)
{
	std::shared_ptr<Image> image = std::make_shared<Image>();
	float* pixels = stbi_loadf(path, &image->m_width, &image->m_height, &image->m_channels, STBI_rgb_alpha);
	//std::cout<< "load image finished" << std::endl;
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	image->m_pixelsHdr = pixels;
	image->m_mipLevels = static_cast<unsigned short>(std::floor(std::log2(std::max(image->m_width, image->m_height)))) + 1;
	return image;
}
