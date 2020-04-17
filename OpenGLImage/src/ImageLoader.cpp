#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <GL/gl3w.h>

#include "ImageLoader.hpp"

#include <iostream>


// Simple helper function to load an image into a OpenGL texture with common settings
    bool ImageMisc::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
    {
        // Load from file
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
        
        if (image_data == NULL)
            return false;

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        *out_texture = image_texture;
        *out_width = image_width;
        *out_height = image_height;

        return true;
    }

    bool ImageMisc::LoadTextureFromData(stbi_uc const* buffer, GLuint* out_texture, int out_width, int out_height)
    {
        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_width, out_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

        *out_texture = image_texture;
        return true;
    }

    unsigned char* ImageMisc::LoadPictureData(const char* filename, int* x, int* y, int* n)
    {
    	return stbi_load(filename, x, y, n, 4);
    }

	unsigned char* ImageMisc::LoadPictureGreyData(const char* filename, int* x, int* y, int* n)
	{
		return stbi_load(filename, x, y, n, 1);
	}

    void ImageMisc::GetPixel(stbi_uc* image, size_t imageWidth, size_t x, size_t y, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
    {
        unsigned bytePerPixel = 4;
        unsigned char* pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
        *r = pixelOffset[0];
        *g = pixelOffset[1];
        *b = pixelOffset[2];
        *a = pixelOffset[3]; //8 >= 4 ? pixelOffset[3] : 0xff;
    }

	void ImageMisc::GetPixelNeighbourhood(stbi_uc* image, size_t imageWidth, int poi, int matrix_size, std::shared_ptr<std::vector<Pixel>> pixel_neighbourhood, int &centre_point)
	{
		unsigned bytePerPixel = 4;
		unsigned char* pixelOffset = 0;

		size_t x;
		size_t y;

		int temp_poi;

		centre_point = matrix_size * matrix_size / 2;

		if (matrix_size == 3)
		{
			for (auto i = 0; i < matrix_size * matrix_size; i++)
			{
				if (i < matrix_size)
				{
					temp_poi = poi - imageWidth - matrix_size / 2 + i;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}
				else if (i < matrix_size * 2)
				{
					temp_poi = poi - matrix_size / 2 + i - matrix_size;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}
				else if (i < matrix_size * matrix_size)
				{
					temp_poi = poi + imageWidth - matrix_size / 2 + i - matrix_size * 2;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}

				pixel_neighbourhood->push_back(Pixel(((float)pixelOffset[0] / 255.0f), ((float)pixelOffset[1] / 255.0f), ((float)pixelOffset[2] / 255.0f), ((float)pixelOffset[3] / 255.0f)));
			}
		}
		else if (matrix_size == 5)
		{
			for (auto i = 0; i < matrix_size * matrix_size; i++)
			{
				if (i < matrix_size)
				{
					temp_poi = poi - imageWidth - imageWidth - matrix_size / 2 + i;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}
				else if (i < matrix_size * 2)
				{
					temp_poi = poi - imageWidth - matrix_size / 2 + i - matrix_size;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}
				else if (i < matrix_size * 3)
				{
					temp_poi = poi - matrix_size / 2 + i - matrix_size * 2;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}
				else if (i < matrix_size * 4)
				{
					temp_poi = poi + imageWidth - matrix_size / 2 + i - matrix_size * 3;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}
				else if (i < matrix_size * matrix_size)
				{
					temp_poi = poi + imageWidth + imageWidth - matrix_size / 2 + i - matrix_size * 4;
					x = temp_poi % imageWidth;
					y = temp_poi / imageWidth;
					pixelOffset = image + (x + imageWidth * y) * bytePerPixel;
				}

				pixel_neighbourhood->push_back(Pixel(((float)pixelOffset[0] / 255.0f), ((float)pixelOffset[1] / 255.0f), ((float)pixelOffset[2] / 255.0f), ((float)pixelOffset[3] / 255.0f)));
			}
		}
	}

	void ImageMisc::SavePixel(stbi_uc* data, size_t imageWidth, size_t x, size_t y, unsigned char* r = NULL, unsigned char* g = NULL, unsigned char* b = NULL, unsigned char* a = NULL)
    {
        unsigned bytePerPixel = 4;
    	unsigned char* pixelOffset = data + (x + imageWidth * y) * bytePerPixel;
    	if(*r != NULL)
			pixelOffset[0] = *r;
    	if(*g != NULL)
			pixelOffset[1] = *g;
        if(*b != NULL)
    		pixelOffset[2] = *b;
        if(*a != NULL)
    		pixelOffset[3] = *a;
    }

	//SAVE PIXEL?

    void ImageMisc::WriteImage(char const* filename, int x, int y, int comp, const void* data)
    {
        stbi_write_bmp(filename, x, y, comp, data);
    }


