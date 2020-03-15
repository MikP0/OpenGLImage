#pragma once

#include <vector>

#include "vendor/stb_image/stb_image.h"
#include "vendor/stb_image/stb_image_write.h"

namespace ImageMisc {
	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	bool LoadTextureFromData(stbi_uc const* buffer, GLuint* out_texture, int out_width, int out_height);
	unsigned char* LoadPictureData(const char* filename, int* x, int* y, int* n);
	void GetPixel(stbi_uc* image, size_t imageWidth, size_t x, size_t y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
	void SavePixel(stbi_uc* data, size_t imageWidth, size_t x, size_t y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
	void WriteImage(char const* filename, int x, int y, int comp, const void* data);
}
