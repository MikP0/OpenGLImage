#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <vector>


#include "ImageLoader.hpp"

enum Colors
{
	RED = 0,
	GREEN = 1,
	BLUE = 2
};


class Image
{
public:
	Image();
	explicit Image(std::string filename);
	explicit Image(unsigned char* data, const int width, const int height);
	int &get_height();
	void set_height(int height);
	int &get_width();
	void set_width(int width);
	GLuint &get_texture();
	void set_texture(GLuint texture);
	void set_filename(const char* filename);
	unsigned char* get_data() const;
	void set_data(unsigned char* data);
	float get_size() const;
	void set_size(float size);
	std::string get_filename() const;
	std::vector<unsigned char> get_main_data() const;
	void set_main_data(std::vector<unsigned char> main_data);
	int get_channels() const;
	void set_channels(int channels);
	std::string get_save_path() const;
	void set_save_path(const std::string& save_path);
	std::vector<float> getReds(Colors Red);

private:
	int height_{};
	int width_{};
	int channels_{};
public:


private:
	GLuint texture_{};
	std::string filename_{};
	unsigned char* data_{};
	std::vector<unsigned char> main_data_{};
	float size_{};
	std::string save_path_{};
public:

};

