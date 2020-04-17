#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <vector>


#include "ImageLoader.hpp"
#include <complex>

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
	void set_grey_data(unsigned char* data);
	unsigned char* get_grey_data() const;
	void set_size(float size);
	std::string get_filename() const;
	std::vector<unsigned char> get_main_data() const;
	void set_main_data(std::vector<unsigned char> main_data);
	int get_channels() const;
	void set_channels(int channels);
	std::string get_save_path() const;
	void set_save_path(const std::string& save_path);
	std::vector<float> getReds(Colors Red);
	std::vector<float> get_red_histogram() const;
	void set_red_histogram(const std::vector<float>& red_histogram);
	std::vector<float> get_green_histogram() const;
	void set_green_histogram(const std::vector<float>& green_histogram);
	std::vector<float> get_blue_histogram() const;
	void set_blue_histogram(const std::vector<float>& blue_histogram);
	float sum_histogram_elements(Colors color, int n) const;
	GLuint& get_fftPhase_texture();
	void set_fftPhase_texture(GLuint texture);
	GLuint& get_fftMagnitude_texture();
	void set_fftMagnitude_texture(GLuint texture);
	GLuint& get_invfft_texture();
	void set_invfft_texture(GLuint texture);
	std::vector <unsigned char> get_fftPhase_data();
	void set_fftPhase_data(std::vector<unsigned char> data);
	std::vector <unsigned char> get_fftMagnitude_data();
	void set_fftMagnitude_data(std::vector<unsigned char> data);
	std::vector<std::vector<std::complex<double>>> get_fftData();
	void set_fftData(std::vector<std::vector<std::complex<double>>> data);


private:
	int height_{};
	int width_{};
	int channels_{};
	GLuint texture_{};
	std::string filename_{};
	unsigned char* data_{};
	unsigned char* greyData_{};
	std::vector<unsigned char> main_data_{};
	float size_{};
	std::string save_path_{};
	std::vector<float> red_histogram_;
	std::vector<float> green_histogram_;
	std::vector<float> blue_histogram_;
	GLuint fftPhaseTexture_{};
	GLuint fftMagnitudeTexture_{};
	std::vector <unsigned char> fftPhaseData_;
	std::vector <unsigned char> fftMagnitudeData_;
	GLuint invfftTexture_{};
	std::vector<std::vector<std::complex<double>>> fftData_;
};

