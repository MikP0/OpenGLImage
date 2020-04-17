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
	static void density_function(const std::shared_ptr<Image>& my_image, float g_min, float g_max);
	static void filter_convolution(const std::shared_ptr < Image>& my_image, int matrix_size, int* num);
	static void filter_median(const std::shared_ptr < Image>& my_image, int matrix_size);
	static void filter_mean(const std::shared_ptr < Image>& my_image, int matrix_size);
	static void filter_roberts(const std::shared_ptr < Image>& my_image, int matrix_size);
	static void _2dfft(const std::shared_ptr<Image> & my_image);
	static void fft_filter_lowpass(const std::shared_ptr<Image>& my_image, int radius);
	static void fft_filter_highpass(const std::shared_ptr<Image>& my_image, int radius);
	static void fft_filter_bandpass(const std::shared_ptr<Image>& my_image, int radius, int size);
	static void fft_filter_bandstop(const std::shared_ptr<Image>& my_image, int radius, int size);
	static void fft_filter_edgedetect(const std::shared_ptr<Image>& my_image, double filterAngle, double filterAngleOffset, int filterRadius);
	static void fft_filter_spectrum(const std::shared_ptr<Image>& my_image, int k, int l);
};

 