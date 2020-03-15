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
    	//auto data = stbi_load(filename, x, y, n, 4);
        //return std::vector<unsigned char>(data, data + (*x * *y));
    	return stbi_load(filename, x, y, n, 4);
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


