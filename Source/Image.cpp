#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

Image::Image(const char *path)
{
	_pixels = nullptr;
	_width = 0;
	_height = 0;
	_channels = 0;
	load_image(path);
}

Image::~Image()
{
	if (_pixels != nullptr)
	{
		stbi_image_free(_pixels);
	}
}

void Image::load_image(const char *path)
{
	stbi_uc *pixels = stbi_load(path, &_width, &_height, &_channels, STBI_rgb_alpha);
	//std::cout<< "load image finished" << std::endl;
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	_pixels = pixels;
}
