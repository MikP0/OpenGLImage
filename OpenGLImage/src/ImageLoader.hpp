#pragma once

#include <vector>
#include <memory>

#include "vendor/stb_image/stb_image.h"
#include "vendor/stb_image/stb_image_write.h"

namespace ImageMisc {

	struct Pixel
	{
		float r;
		float g;
		float b;
		float a;

		Pixel(float ap, float bp, float cp, float dp) : r(ap), g(bp), b(cp), a(dp) {}
		Pixel() {}

		void Multiply(int s)
		{
			r *= s;
			g *= s;
			b *= s;
		}

		void Add(Pixel pix)
		{
			r += pix.r;
			g += pix.g;
			b += pix.b;
		}
	};

	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	bool LoadTextureFromData(stbi_uc const* buffer, GLuint* out_texture, int out_width, int out_height);
	unsigned char* LoadPictureData(const char* filename, int* x, int* y, int* n);
	unsigned char* LoadPictureGreyData(const char* filename, int* x, int* y, int* n);
	void GetPixel(stbi_uc* image, size_t imageWidth, size_t x, size_t y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
	void GetPixelNeighbourhood(stbi_uc* image, size_t imageWidth, int poi, int matrix_size, std::shared_ptr<std::vector<Pixel>> pixel_neighbourhood, int &centre_point);
	void SavePixel(stbi_uc* data, size_t imageWidth, size_t x, size_t y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
	void WriteImage(char const* filename, int x, int y, int comp, const void* data);
}
