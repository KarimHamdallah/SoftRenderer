#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../GpuLayer/stb_image.h"

#define RGBA_TO_UNIT32(r, g, b, a) ((r) | (g << 8) | (b << 16) | (a << 24))

Texture2D::Texture2D(const std::filesystem::path& path, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);

	int w, h, bpp;
	uint8_t* img = stbi_load(path.string().c_str(), &w, &h, &bpp, 3);
	int buffer_size = w * h * bpp;

	if (!img)
		throw std::runtime_error("failed to load image!");


	pixels = new uint32_t[w * h];
	Width = w;
	Height = h;

	for (size_t i = 0; i < w * h * bpp / 3 ; i++)
	{
		int r = img[i * 3 + 0];
		int g = img[i * 3 + 1];
		int b = img[i * 3 + 2];

		pixels[i] = RGBA_TO_UNIT32(r, g, b, 1);
	}

	stbi_image_free(img);
	Loaded = true;
	stbi_set_flip_vertically_on_load(false);
}

Texture2D::~Texture2D()
{
	delete[] pixels;
	pixels = nullptr;
	Width = 0;
	Height = 0;
}
