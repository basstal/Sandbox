#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <cmath>

Image::Image()
{
	_pixels = nullptr;
	_width = 0;
	_height = 0;
	_channels = 0;
}

Image::~Image()
{
	if (_pixels != nullptr)
	{
		stbi_image_free(_pixels);
	}
}

std::shared_ptr<Image> Image::loadImage(const char *path)
{
	std::shared_ptr<Image> image = std::make_shared<Image>();
	stbi_uc *pixels = stbi_load(path, &image->_width, &image->_height, &image->_channels, STBI_rgb_alpha);
	//std::cout<< "load image finished" << std::endl;
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	image->_pixels = pixels;
	image->_mipLevels = static_cast<unsigned short>(std::floor(std::log2(std::max(image->_width, image->_height)))) + 1;
	return image;
}
