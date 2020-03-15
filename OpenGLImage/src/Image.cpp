#include "Image.hpp"

#include <filesystem>

Image::Image()
= default;

Image::Image(std::string filename)
{
	this->filename_ = filename;
	this->size_ = 1;
	ImageMisc::LoadTextureFromFile(this->filename_.c_str(), &this->texture_, &this->width_, &this->height_);
	this->data_ = ImageMisc::LoadPictureData(this->filename_.c_str(), &this->width_, &this->height_, &this->channels_);
	std::vector<unsigned char> temp;
	this->main_data_ = std::vector<unsigned char>(width_ * height_ * 4);
	this->save_path_.append("result_images/");
	this->save_path_.append(std::filesystem::u8path(filename_).filename().u8string());
}

Image::Image(unsigned char* data, const int width, const int height)
{
	this->width_ = width;
	this->height_ = height;
	this->size_ = 1;
	ImageMisc::LoadTextureFromData(data, &this->texture_, width_, height_);
}

int& Image::get_height()
{
	return height_;
}

void Image::set_height(const int height)
{
	height_ = height;
}

int& Image::get_width()
{
	return width_;
}

void Image::set_width(const int width)
{
	width_ = width;
}

GLuint& Image::get_texture()
{
	return texture_;
}

void Image::set_texture(const GLuint texture)
{
	texture_ = texture;
}

std::string Image::get_filename() const
{
	return filename_;
}

void Image::set_filename(const char* filename)
{
	filename_ = filename;
}

unsigned char* Image::get_data() const
{
	return data_;
}

void Image::set_data(unsigned char* data)
{
	data_ = data;
}

float Image::get_size() const
{
	return size_;
}

void Image::set_size(float size)
{
	size_ = size;
}

std::vector<unsigned char> Image::get_main_data() const
{
	return main_data_;
}

void Image::set_main_data(std::vector<unsigned char> main_data)
{
	main_data_ = main_data;
}

int Image::get_channels() const
{
	return channels_;
}

void Image::set_channels(int channels)
{
	channels_ = channels;
}

std::string Image::get_save_path() const
{
	if(save_path_.length() > 0)
		return save_path_;
	return filename_;
}

void Image::set_save_path(const std::string& save_path)
{
	save_path_ = save_path;
}

std::shared_ptr<std::vector<float>> Image::getReds(std::shared_ptr<std::vector<float>> reds)
{
	for(int i = 0; i < width_ * height_* 4; i+=4)
	{
		reds->push_back(static_cast<float>(main_data_[i]));
	}
	return reds;
}