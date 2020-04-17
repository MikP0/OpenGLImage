#include "ImageEditor.hpp"

#include <vector>
#include <algorithm>
#include <complex>
#include <valarray>
#include <cmath>
#include <math.h>

void ImageEditor::change_brightness(std::shared_ptr<Image> my_image, int brightness)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	std::vector<unsigned char> output_image_data;
	unsigned char LUT[256];

	for (int i = 0; i < 256; i++) {
		if ((brightness + i) > 255)
		{
			LUT[i] = 255;
		}
		else if ((brightness + i) < 0)
		{
			LUT[i] = 0;
		}
		else
		{
			LUT[i] = (brightness + i);
		}
	}

	for (auto i = 0; i < my_image->get_width() * my_image->get_height(); i++) {
		ImageMisc::GetPixel(my_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
		output_image_data.push_back(LUT[r]);
		output_image_data.push_back(LUT[g]);
		output_image_data.push_back(LUT[b]);
		output_image_data.push_back(a);
	}
	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

void ImageEditor::change_size(std::shared_ptr<Image> my_image, float size)
{
	my_image->set_size(size);
}

void ImageEditor::change_negative(const std::shared_ptr<Image>& my_image)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	std::vector<unsigned char> output_image_data;


	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++) {
		ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);

		output_image_data.emplace_back(255 - r);
		output_image_data.emplace_back(255 - g);
		output_image_data.emplace_back(255 - b);
		output_image_data.emplace_back(a);
	}
	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

void ImageEditor::change_contrast(const std::shared_ptr<Image>& my_image, float contrast)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	std::vector<unsigned char> output_image_data;

	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++) {
		ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
		auto r_val = ((r - 128) * contrast) + 128;
		auto g_val = ((g - 128) * contrast) + 128;
		auto b_val = ((b - 128) * contrast) + 128;
		if (r_val < 0)
			output_image_data.push_back(0);
		else if (r_val > 255)
			output_image_data.push_back(255);
		else
			output_image_data.push_back(r_val);

		if (g_val < 0)
			output_image_data.push_back(0);
		else if (g_val > 255)
			output_image_data.push_back(255);
		else
			output_image_data.push_back(g_val);

		if (b_val < 0)
			output_image_data.push_back(0);
		else if (b_val > 255)
			output_image_data.push_back(255);
		else
			output_image_data.push_back(b_val);

		output_image_data.push_back(a);
	}
	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

unsigned char count_density(int sum, float g_min, float g_max, int size);

void ImageEditor::density_function(const std::shared_ptr<Image>& my_image, float g_min, float g_max)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	std::vector<unsigned char> output_image_data;

	auto gs = powf(g_min, 1.0f / 3.0f) + (powf(g_max, 1.0f / 3.0) - powf(g_min, 1.0f / 3.0));

	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++) {
		ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);

		output_image_data.emplace_back(count_density(my_image->sum_histogram_elements(Colors::RED, r), g_min, g_max, temp_image->get_width() * temp_image->get_height()));
		output_image_data.emplace_back(count_density(my_image->sum_histogram_elements(Colors::GREEN, g), g_min, g_max, temp_image->get_width() * temp_image->get_height()));
		output_image_data.emplace_back(count_density(my_image->sum_histogram_elements(Colors::BLUE, b), g_min, g_max, temp_image->get_width() * temp_image->get_height()));
		output_image_data.emplace_back(a);
	}
	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

bool check_border_for_filter(int i, int matrix_size, std::shared_ptr<Image> image);

void ImageEditor::filter_convolution(const std::shared_ptr<Image>& my_image, int matrix_size, int* num)
{
	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	std::vector<unsigned char> output_image_data;

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	int r_val = 0;
	int g_val = 0;
	int b_val = 0;
	int a_val = 0;

	std::shared_ptr<std::vector<ImageMisc::Pixel>> pixel_vector = std::make_shared<std::vector<ImageMisc::Pixel>>();
	ImageMisc::Pixel temp_pixel;

	int weight_sum = 0;

	for (auto i = 0; i < matrix_size * matrix_size; i++)
	{
		weight_sum += num[i];
	}

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		if (check_border_for_filter(i, matrix_size, temp_image))
		{
			int center = 0;

			ImageMisc::GetPixelNeighbourhood(temp_image->get_data(), my_image->get_width(), i, matrix_size, pixel_vector, center);

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				pixel_vector->at(j).Multiply(num[j]);
			}

			temp_pixel = pixel_vector->at(center);
			pixel_vector->erase(pixel_vector->begin() + center);

			while (pixel_vector->size())
			{
				auto pix = pixel_vector->back();
				pixel_vector->pop_back();
				temp_pixel.Add(pix);
			}

			if (weight_sum != 0)
			{
				r_val = (temp_pixel.r * 255) / weight_sum;
				g_val = (temp_pixel.g * 255) / weight_sum;
				b_val = (temp_pixel.b * 255) / weight_sum;
			}
			else
			{
				r_val = (temp_pixel.r * 255);
				g_val = (temp_pixel.g * 255);
				b_val = (temp_pixel.b * 255);
			}
			a_val = temp_pixel.a * 255;

			if (r_val < 0)
				output_image_data.push_back(0);
			else if (r_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(r_val);

			if (g_val < 0)
				output_image_data.push_back(0);
			else if (g_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(g_val);

			if (b_val < 0)
				output_image_data.push_back(0);
			else if (b_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(b_val);

			output_image_data.push_back(a_val);
		}
		else
		{
			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			r_val = r;
			g_val = g;
			b_val = b;
			a_val = 0;

			output_image_data.push_back(r_val);
			output_image_data.push_back(g_val);
			output_image_data.push_back(b_val);
			output_image_data.push_back(a_val);
		}

		pixel_vector->clear();
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

void ImageEditor::filter_median(const std::shared_ptr<Image>& my_image, int matrix_size)
{
	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	std::vector<unsigned char> output_image_data;

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	int r_val = 0;
	int g_val = 0;
	int b_val = 0;
	int a_val = 0;

	float r_temp = 0;
	float g_temp = 0;
	float b_temp = 0;
	float a_temp = 0;

	std::shared_ptr<std::vector<ImageMisc::Pixel>> pixel_vector = std::make_shared<std::vector<ImageMisc::Pixel>>();
	ImageMisc::Pixel temp_pixel;

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		if (check_border_for_filter(i, matrix_size, temp_image))
		{
			int center = 0;

			ImageMisc::GetPixelNeighbourhood(temp_image->get_data(), my_image->get_width(), i, matrix_size, pixel_vector, center);

			std::vector<float> vec;

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				vec.push_back(pixel_vector->at(j).r);
			}

			std::sort(vec.begin(), vec.end());
			r_temp = vec.at(vec.size() / 2);
			vec.clear();

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				vec.push_back(pixel_vector->at(j).g);
			}

			std::sort(vec.begin(), vec.end()); 
			g_temp = vec.at(vec.size() / 2);
			vec.clear();

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				vec.push_back(pixel_vector->at(j).b);
			}

			std::sort(vec.begin(), vec.end());
			b_temp = vec.at(vec.size() / 2);
			vec.clear();

			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			a_val = a;

			r_val = r_temp * 255;
			g_val = g_temp * 255;
			b_val = b_temp * 255;


			if (r_val < 0)
				output_image_data.push_back(0);
			else if (r_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(r_val);

			if (g_val < 0)
				output_image_data.push_back(0);
			else if (g_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(g_val);

			if (b_val < 0)
				output_image_data.push_back(0);
			else if (b_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(b_val);

			output_image_data.push_back(a_val);
		}
		else
		{
			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			r_val = r;
			g_val = g;
			b_val = b;
			a_val = 0;

			output_image_data.push_back(r_val);
			output_image_data.push_back(g_val);
			output_image_data.push_back(b_val);
			output_image_data.push_back(a_val);
		}

		pixel_vector->clear();
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

void ImageEditor::filter_mean(const std::shared_ptr<Image>& my_image, int matrix_size)
{
	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	std::vector<unsigned char> output_image_data;

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	int r_val = 0;
	int g_val = 0;
	int b_val = 0;
	int a_val = 0;

	float r_temp = 0;
	float g_temp = 0;
	float b_temp = 0;
	float a_temp = 0;

	std::shared_ptr<std::vector<ImageMisc::Pixel>> pixel_vector = std::make_shared<std::vector<ImageMisc::Pixel>>();
	ImageMisc::Pixel temp_pixel;

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		if (check_border_for_filter(i, matrix_size, temp_image))
		{
			int center = 0;

			ImageMisc::GetPixelNeighbourhood(temp_image->get_data(), my_image->get_width(), i, matrix_size, pixel_vector, center);

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				r_temp += pixel_vector->at(j).r;
			}
			r_temp /= pixel_vector->size();

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				g_temp += pixel_vector->at(j).g;
			}
			g_temp /= pixel_vector->size();

			for (auto j = 0; j < pixel_vector->size(); j++)
			{
				b_temp += pixel_vector->at(j).b;
			}
			b_temp /= pixel_vector->size();

			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			a_val = a;

			r_val = r_temp * 255;
			g_val = g_temp * 255;
			b_val = b_temp * 255;


			if (r_val < 0)
				output_image_data.push_back(0);
			else if (r_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(r_val);

			if (g_val < 0)
				output_image_data.push_back(0);
			else if (g_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(g_val);

			if (b_val < 0)
				output_image_data.push_back(0);
			else if (b_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(b_val);

			output_image_data.push_back(a_val);
		}
		else
		{
			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			r_val = r;
			g_val = g;
			b_val = b;
			a_val = 0;

			output_image_data.push_back(r_val);
			output_image_data.push_back(g_val);
			output_image_data.push_back(b_val);
			output_image_data.push_back(a_val);
		}

		pixel_vector->clear();
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

void ImageEditor::filter_roberts(const std::shared_ptr<Image>& my_image, int matrix_size)
{
	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());

	std::vector<unsigned char> output_image_data;

	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	int r_val = 0;
	int g_val = 0;
	int b_val = 0;
	int a_val = 0;

	float r_temp = 0;
	float g_temp = 0;
	float b_temp = 0;
	float a_temp = 0;

	std::shared_ptr<std::vector<ImageMisc::Pixel>> pixel_vector = std::make_shared<std::vector<ImageMisc::Pixel>>();
	ImageMisc::Pixel temp_pixel;

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		if (check_border_for_filter(i, matrix_size, temp_image))
		{
			int center = 0;

			ImageMisc::GetPixelNeighbourhood(temp_image->get_data(), my_image->get_width(), i, matrix_size, pixel_vector, center);

			temp_pixel = pixel_vector->at(center);

			r_temp = powf(powf((temp_pixel.r - pixel_vector->at(pixel_vector->size() - 1).r), 2.0f) + powf(pixel_vector->at(center + matrix_size).r - pixel_vector->at(center + 1).r,2.0f), 1.0f/2.0f);
			g_temp = powf(powf((temp_pixel.g - pixel_vector->at(pixel_vector->size() - 1).g), 2.0f) + powf(pixel_vector->at(center + matrix_size).g - pixel_vector->at(center + 1).g,2.0f), 1.0f/2.0f);
			b_temp = powf(powf((temp_pixel.b - pixel_vector->at(pixel_vector->size() - 1).b), 2.0f) + powf(pixel_vector->at(center + matrix_size).b - pixel_vector->at(center + 1).b,2.0f), 1.0f/2.0f);
		

			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			a_val = a;

			r_val = r_temp * 255;
			g_val = g_temp * 255;
			b_val = b_temp * 255;


			if (r_val < 0)
				output_image_data.push_back(0);
			else if (r_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(r_val);

			if (g_val < 0)
				output_image_data.push_back(0);
			else if (g_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(g_val);

			if (b_val < 0)
				output_image_data.push_back(0);
			else if (b_val > 255)
				output_image_data.push_back(255);
			else
				output_image_data.push_back(b_val);

			output_image_data.push_back(a_val);
		}
		else
		{
			ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
			r_val = r;
			g_val = g;
			b_val = b;
			a_val = 0;

			output_image_data.push_back(r_val);
			output_image_data.push_back(g_val);
			output_image_data.push_back(b_val);
			output_image_data.push_back(a_val);
		}

		pixel_vector->clear();
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}

unsigned char count_density(int sum, float g_min, float g_max, int size)
{
	return powf(powf(g_min, 0.33f) + ((powf(g_max, 0.33f) - powf(g_min, 0.33f)) * 1 / size * sum), 3);
}

bool check_border_for_filter(int i, int matrix_size, std::shared_ptr<Image> image)
{
	if (matrix_size == 3)
	{
		if (!(i % image->get_height() == 0 || i % image->get_height() == image->get_height() - 1 || i < image->get_width() || i > image->get_width()* image->get_height() - image->get_height()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (matrix_size == 5)
	{
		if (!(i % image->get_height() == 0 || i % image->get_height() == image->get_height() - 1 || i < image->get_width() || i > image->get_width()* image->get_height() - image->get_height() || i % image->get_height() == 1 || i % image->get_height() == image->get_height() - 2 || i < image->get_width() + image->get_width() || i > image->get_width() * image->get_height() - image->get_height() - image->get_height()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


void fft(std::vector<std::complex<double>>& x)
{
	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	std::complex<double> phiT = std::complex<double>(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				std::complex<double> t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			std::complex<double> t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}

void swapQuadrants(std::vector<std::vector<std::complex<double>>>& complexData) 
{
	int size = complexData.size();

	for (int x = 0; x < size / 2; x++)
	{
		for (int y = 0; y < size / 2; y++)
		{
			std::complex<double> temp = complexData[x][y];
			complexData[x][y] = complexData[x + (size / 2)][y + (size / 2)];
			complexData[x + (size / 2)][y + (size / 2)] = temp;
		}
	}

	for (int x = size / 2; x < size; x++)
	{
		for (int y = 0; y < size / 2; y++)
		{
			std::complex<double> temp = complexData[x][y];
			complexData[x][y] = complexData[x - (size / 2)][y + (size / 2)];
			complexData[x - (size / 2)][y + (size / 2)] = temp;
		}
	}
}

unsigned char fixOverflow(double value) {
	if (value > 255) {
		value = 255;
	}
	else if (value < 0) {
		value = 0;
	}

	return value;
}

void ifft2d(std::vector<std::vector<std::complex<double>>> vals, const std::shared_ptr<Image>& my_image)
{
	// conjugate the complex numbers
	for (int i = 0; i < vals.size(); i++)
	{
		for (int j = 0; j < vals.size(); j++) {
			vals[i][j] = std::conj(vals[i][j]);
		}
	}

	// forward fft
	for (int i = 0; i < vals.size(); i++)
	{
		fft(vals[i]);
	}

	// conjugate the complex numbers again
	for (int i = 0; i < vals.size(); i++)
	{
		for (int j = 0; j < vals.size(); j++) {
			vals[i][j] = std::conj(vals[i][j]);
		}
	}

	for (int i = 0; i < vals.size(); i++)
	{
		for (int j = 0; j < vals.size(); j++) {
			vals[i][j] /= vals.size();
		}
	}

	int swappedCols = vals.size();
	int swappedRows = vals[0].size();

	std::vector<std::vector<std::complex<double>>> swappedData(swappedCols, std::vector<std::complex<double>>(swappedRows));

	for (int x = 0; x < swappedCols; x++) {
		for (int y = 0; y < swappedRows; y++) {
			swappedData[y][x] = vals[x][y];
		}
	}

	for (int i = 0; i < swappedData.size(); i++)
	{
		for (int j = 0; j < swappedData.size(); j++) {
			swappedData[i][j] = std::conj(swappedData[i][j]);
		}
	}

	for (int i = 0; i < vals.size(); i++)
	{
		fft(swappedData[i]);
	}


	for (int i = 0; i < swappedData.size(); i++)
	{
		for (int j = 0; j < swappedData.size(); j++) {
			swappedData[i][j] = std::conj(swappedData[i][j]);
		}
	}

	for (int i = 0; i < swappedData.size(); i++)
	{
		for (int j = 0; j < swappedData.size(); j++) {
			swappedData[i][j] /= swappedData.size();
		}
	}

	std::vector<unsigned char> output_image_data;

	for (int y = 0; y < swappedCols; y++)
	{
		for (int x = 0; x < swappedRows; x++)
		{
			output_image_data.push_back(fixOverflow(swappedData[y][x].real()));
			output_image_data.push_back(fixOverflow(swappedData[y][x].real()));
			output_image_data.push_back(fixOverflow(swappedData[y][x].real()));
			output_image_data.push_back(255);

		}
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);

}

struct MinMax {
	MinMax() { Min = 1.79769e+308; Max = -2.22507e-308; };
	double Min;
	double Max;
};

enum ComplexProps {
	PHASE,
	MAGNITUDE
};

MinMax calculateMinMax(std::vector<std::vector<std::complex<double>>> data, ComplexProps prop) {
	MinMax result;
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			auto value = 0.0;
			switch (prop)
			{
			case PHASE:
				value = std::arg(data[i][j]);
				break;
			case MAGNITUDE:
				value = std::abs(data[i][j]);
				break;
			}
			result.Min = value < result.Min ? value : result.Min;
			result.Max = value > result.Max ? value : result.Max;
		}
	}
	return result;
}

double logNormalize(double value, double max, double expectedMax)
{
	return log(1 + value) / log(1 + max) * expectedMax;
}

void updatePhaseMagnitude(const std::shared_ptr<Image>& my_image, std::vector<std::vector<std::complex<double>>> complexData) {
	auto phaseInterval = calculateMinMax(complexData, PHASE);
	auto magnitudeInterval = calculateMinMax(complexData, MAGNITUDE);

	std::vector<unsigned char> phaseValueVector;
	std::vector<unsigned char> magnitudeValueVector;
	for (int y = 0; y < my_image->get_height(); y++)
	{
		for (int x = 0; x < my_image->get_width(); x++)
		{
			//auto index = x + y * my_image->get_width();
			phaseValueVector.emplace_back((unsigned char)logNormalize(std::arg(complexData[y][x]), phaseInterval.Max, 255));
			phaseValueVector.emplace_back((unsigned char)logNormalize(std::arg(complexData[y][x]), phaseInterval.Max, 255));
			phaseValueVector.emplace_back((unsigned char)logNormalize(std::arg(complexData[y][x]), phaseInterval.Max, 255));
			phaseValueVector.emplace_back(255);
			magnitudeValueVector.emplace_back((unsigned char)logNormalize(std::abs(complexData[y][x]), magnitudeInterval.Max, 255));
			magnitudeValueVector.emplace_back((unsigned char)logNormalize(std::abs(complexData[y][x]), magnitudeInterval.Max, 255));
			magnitudeValueVector.emplace_back((unsigned char)logNormalize(std::abs(complexData[y][x]), magnitudeInterval.Max, 255));
			magnitudeValueVector.emplace_back(255);

		}
	}

	ImageMisc::LoadTextureFromData(phaseValueVector.data(), &my_image->get_fftPhase_texture(), 512, 512);
	ImageMisc::LoadTextureFromData(magnitudeValueVector.data(), &my_image->get_fftMagnitude_texture(), 512, 512);
}

void ImageEditor::_2dfft(const std::shared_ptr<Image> & my_image) 
{
	std::vector<std::vector<std::complex<double>>> data(my_image->get_width(), std::vector<std::complex<double>>(my_image->get_height()));

	for (int y = 0; y < my_image->get_height(); y++)
	{
		for (int x = 0; x < my_image->get_width(); x++)
		{
			data[y][x] = my_image->get_grey_data()[x + y * my_image->get_width() ];
		}
	}

	for (int i = 0; i < data.size(); i++)
	{
		fft(data[i]);
	}

	std::vector<std::vector<std::complex<double>>> swappedData(my_image->get_width(), std::vector<std::complex<double>>(my_image->get_height()));

	int swappedCols = data.size();
	int swappedRows = data[0].size();

	for (int x = 0; x < swappedCols; x++) {
		for (int y = 0; y < swappedRows; y++) {
			swappedData[y][x] = data[x][y];
		}
	}

	for (int i = 0; i < data.size(); i++)
	{
		fft(swappedData[i]);
	}

	my_image->set_fftData(swappedData);

	auto phaseInterval = calculateMinMax(swappedData, PHASE);
	auto magnitudeInterval = calculateMinMax(swappedData, MAGNITUDE);

	swapQuadrants(swappedData);

	std::vector<unsigned char> phaseValueVector;
	std::vector<unsigned char> magnitudeValueVector;
	for (int y = 0; y < my_image->get_height(); y++)
	{
		for (int x = 0; x < my_image->get_width(); x++)
		{
			//auto index = x + y * my_image->get_width();
			phaseValueVector.emplace_back((unsigned char)logNormalize(std::arg(swappedData[y][x]), phaseInterval.Max, 255));
			phaseValueVector.emplace_back((unsigned char)logNormalize(std::arg(swappedData[y][x]), phaseInterval.Max, 255));
			phaseValueVector.emplace_back((unsigned char)logNormalize(std::arg(swappedData[y][x]), phaseInterval.Max, 255));
			phaseValueVector.emplace_back(255);
			magnitudeValueVector.emplace_back((unsigned char)logNormalize(std::abs(swappedData[y][x]), magnitudeInterval.Max, 255));
			magnitudeValueVector.emplace_back((unsigned char)logNormalize(std::abs(swappedData[y][x]), magnitudeInterval.Max, 255));
			magnitudeValueVector.emplace_back((unsigned char)logNormalize(std::abs(swappedData[y][x]), magnitudeInterval.Max, 255));
			magnitudeValueVector.emplace_back(255);

		}
	}
	
	
	my_image->set_fftPhase_data(phaseValueVector);
	my_image->set_fftMagnitude_data(magnitudeValueVector);
	ImageMisc::LoadTextureFromData(my_image->get_fftPhase_data().data(), &my_image->get_fftPhase_texture(), 512, 512);
	ImageMisc::LoadTextureFromData(my_image->get_fftMagnitude_data().data(), &my_image->get_fftMagnitude_texture(), 512, 512);
	//ImageMisc::WriteImage("help.bmp", 512, 512, 4, my_image->get_fft_data().data());
	swapQuadrants(swappedData);
}

void ImageEditor::fft_filter_lowpass(const std::shared_ptr<Image>& my_image, int radius)
{
	auto complexData = my_image->get_fftData();
	swapQuadrants(complexData);

	double imageHeight = complexData.size();
	double imageWidth = complexData[0].size();

	double halfImageHeight = imageHeight / 2;
	double halfImageWidth = imageWidth / 2;

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			if (i == halfImageHeight && j == halfImageWidth)
			{
				continue;
			}

			auto widthFactor = std::pow((halfImageWidth - j) / (halfImageWidth - radius), 2);
			auto heightFactor = std::pow((halfImageHeight - i) / (halfImageHeight - radius), 2);

			if (std::sqrt(widthFactor + heightFactor) > 1)
			{
				complexData[i][j] = std::complex(0,0);
			}
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	//ImageMisc::WriteImage("help.bmp", 512, 512, 4, my_image->get_fft_data().data());
	swapQuadrants(complexData);


	ifft2d(complexData, my_image);
}

void ImageEditor::fft_filter_highpass(const std::shared_ptr<Image>& my_image, int radius)
{
	auto complexData = my_image->get_fftData();
	swapQuadrants(complexData);

	double imageHeight = complexData.size();
	double imageWidth = complexData[0].size();

	double halfImageHeight = imageHeight / 2;
	double halfImageWidth = imageWidth / 2;

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			if (i == halfImageHeight && j == halfImageWidth)
			{
				continue;
			}

			auto widthFactor = std::pow((halfImageWidth - j) / radius, 2);
			auto heightFactor = std::pow((halfImageHeight - i) / radius, 2);

			if (std::sqrt(widthFactor + heightFactor) < 1)
			{
				complexData[i][j] = 0;
			}
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	swapQuadrants(complexData);
	ifft2d(complexData, my_image);

}

void ImageEditor::fft_filter_bandpass(const std::shared_ptr<Image>& my_image, int radius, int size)
{
	auto complexData = my_image->get_fftData();
	swapQuadrants(complexData);

	double imageHeight = complexData.size();
	double imageWidth = complexData[0].size();

	double halfImageHeight = imageHeight / 2;
	double halfImageWidth = imageWidth / 2;

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			if (i == halfImageHeight && j == halfImageWidth)
			{
				continue;
			}

			auto widthFactor = std::pow(halfImageWidth - j, 2);
			auto heightFactor = std::pow(halfImageHeight - i, 2);
			auto result = std::sqrt(widthFactor + heightFactor);

			if (result > radius + size || result < radius)
			{
				complexData[i][j] = 0;
			}
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	swapQuadrants(complexData);
	ifft2d(complexData, my_image);
}

void ImageEditor::fft_filter_bandstop(const std::shared_ptr<Image>& my_image, int radius, int size)
{
	auto complexData = my_image->get_fftData();
	swapQuadrants(complexData);

	double imageHeight = complexData.size();
	double imageWidth = complexData[0].size();

	double halfImageHeight = imageHeight / 2;
	double halfImageWidth = imageWidth / 2;

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			if (i == halfImageHeight && j == halfImageWidth)
			{
				continue;
			}

			auto widthFactor = std::pow(halfImageWidth - j, 2);
			auto heightFactor = std::pow(halfImageHeight - i, 2);
			auto result = std::sqrt(widthFactor + heightFactor);

			if (result < radius + size && result > radius)
			{
				complexData[i][j] = 0;
			}
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	swapQuadrants(complexData);
	ifft2d(complexData, my_image);
}

void ImageEditor::fft_filter_edgedetect(const std::shared_ptr<Image>& my_image, double filterAngle, double filterAngleOffset, int filterRadius)
{
	auto complexData = my_image->get_fftData();
	swapQuadrants(complexData);

	const double EPSILON = 0.0000001;

	double _firstLineAngle;
	double _secondLineAngle;

	_firstLineAngle = filterAngle + filterAngleOffset;
	_secondLineAngle = -filterAngle + filterAngleOffset;

	double imageHeight = complexData.size();
	double imageWidth = complexData[0].size();

	double halfImageHeight = imageHeight / 2;
	double halfImageWidth = imageWidth / 2;

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			if (i == halfImageHeight && j == halfImageWidth)
			{
				continue;
			}

			auto widthFactor = std::pow((halfImageWidth - j) / filterRadius, 2);
			auto heightFactor = std::pow((halfImageHeight - i) / filterRadius, 2);

			if (std::sqrt(widthFactor + heightFactor) < 1)
			{
				complexData[i][j] = 0;
				continue;
			}

			auto firstLineValue = std::tan(_firstLineAngle) * (j - halfImageWidth);
			auto secondLineValue = std::tan(_secondLineAngle) * (j - halfImageWidth);

			bool firstHalfStartAssert;
			bool firstHalfEndAssert;
			bool secondHalfStartAssert;
			bool secondHalfEndAssert;

			auto currentValue = i - halfImageHeight;
			//std::acos(-1) = PI
			if ((_firstLineAngle - std::acos(-1) / 2 > EPSILON && _secondLineAngle - std::acos(-1) / 2 < EPSILON) ||
				(_firstLineAngle - 1.5 * std::acos(-1) > EPSILON && _secondLineAngle - 1.5 * std::acos(-1) < EPSILON))
			{
				firstHalfStartAssert = firstLineValue > currentValue;
				firstHalfEndAssert = secondLineValue > currentValue;
				secondHalfStartAssert = secondLineValue < currentValue;
				secondHalfEndAssert = firstLineValue < currentValue;
			}
			else if (_firstLineAngle - std::acos(-1) > EPSILON && _firstLineAngle - 1.5 * std::acos(-1) < EPSILON)
			{
				firstHalfStartAssert = firstLineValue > currentValue;
				firstHalfEndAssert = secondLineValue < currentValue;
				secondHalfStartAssert = firstLineValue < currentValue;
				secondHalfEndAssert = secondLineValue > currentValue;
			}
			else
			{
				firstHalfStartAssert = firstLineValue < currentValue;
				firstHalfEndAssert = secondLineValue > currentValue;
				secondHalfStartAssert = firstLineValue > currentValue;
				secondHalfEndAssert = secondLineValue < currentValue;
			}

			if (!((firstHalfStartAssert && firstHalfEndAssert) || (secondHalfStartAssert && secondHalfEndAssert)))
			{
				complexData[i][j] = 0;
			}
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	swapQuadrants(complexData);
	ifft2d(complexData, my_image);
}

void ImageEditor::fft_filter_spectrum(const std::shared_ptr<Image>& my_image, int k, int l)
{
	auto complexData = my_image->get_fftData();
	swapQuadrants(complexData);

	double imageHeight = complexData.size();
	double imageWidth = complexData[0].size();

	for (int i = 0; i < imageHeight; i++)
	{
		for (int j = 0; j < imageWidth; j++)
		{
			complexData[i][j] *= std::exp( std::complex<double>(0, (-i * k * 2 * std::acos(-1) / imageHeight) + (-j * l * 2 * std::acos(-1) / imageWidth) + (std::acos(-1) * (k + l))));
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	swapQuadrants(complexData);
	ifft2d(complexData, my_image);
}




