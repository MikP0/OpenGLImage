#pragma once
#include <memory>

#include "Image.hpp"

class ImageEditor
{
public:
	static void change_brightness(std::shared_ptr<Image> my_image, int brightness);
	static void change_size(std::shared_ptr<Image> my_image, float size);
	static void change_negative(const std::shared_ptr<Image>& my_image);
	static void change_contrast(const std::shared_ptr<Image>& my_image, float contrast);
};

