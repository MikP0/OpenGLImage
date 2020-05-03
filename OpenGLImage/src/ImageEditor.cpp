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

			r_temp = powf(powf((temp_pixel.r - pixel_vector->at(pixel_vector->size() - 1).r), 2.0f) + powf(pixel_vector->at(center + matrix_size).r - pixel_vector->at(center + 1).r, 2.0f), 1.0f / 2.0f);
			g_temp = powf(powf((temp_pixel.g - pixel_vector->at(pixel_vector->size() - 1).g), 2.0f) + powf(pixel_vector->at(center + matrix_size).g - pixel_vector->at(center + 1).g, 2.0f), 1.0f / 2.0f);
			b_temp = powf(powf((temp_pixel.b - pixel_vector->at(pixel_vector->size() - 1).b), 2.0f) + powf(pixel_vector->at(center + matrix_size).b - pixel_vector->at(center + 1).b, 2.0f), 1.0f / 2.0f);


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
		if (!(i % image->get_height() == 0 || i % image->get_height() == image->get_height() - 1 || i < image->get_width() || i > image->get_width() * image->get_height() - image->get_height()))
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
		if (!(i % image->get_height() == 0 || i % image->get_height() == image->get_height() - 1 || i < image->get_width() || i > image->get_width() * image->get_height() - image->get_height() || i % image->get_height() == 1 || i % image->get_height() == image->get_height() - 2 || i < image->get_width() + image->get_width() || i > image->get_width() * image->get_height() - image->get_height() - image->get_height()))
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

void ImageEditor::_2dfft(const std::shared_ptr<Image>& my_image)
{
	std::vector<std::vector<std::complex<double>>> data(my_image->get_width(), std::vector<std::complex<double>>(my_image->get_height()));

	for (int y = 0; y < my_image->get_height(); y++)
	{
		for (int x = 0; x < my_image->get_width(); x++)
		{
			data[y][x] = my_image->get_grey_data()[x + y * my_image->get_width()];
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
				complexData[i][j] = std::complex(0, 0);
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
			complexData[i][j] *= std::exp(std::complex<double>(0, (-i * k * 2 * std::acos(-1) / imageHeight) + (-j * l * 2 * std::acos(-1) / imageWidth) + (std::acos(-1) * (k + l))));
		}
	}

	updatePhaseMagnitude(my_image, complexData);
	swapQuadrants(complexData);
	ifft2d(complexData, my_image);
}

void Split(std::vector<std::vector<ImageMisc::Pixel>> pixels, int splitValue, std::vector<std::vector<std::vector<ImageMisc::Pixel>>>& groups_vector, int minSizeOfGroup);
void Merge(std::vector<std::vector<std::vector<ImageMisc::Pixel>>>& groups_vector, int splitValue);
void SecondMerge(std::vector<std::vector<std::vector<ImageMisc::Pixel>>>& groups_vector, int splitValue);
bool ShouldSplitMore(std::vector<std::vector<ImageMisc::Pixel>> pixels, int splitValue, int minSizeOfGroup);
bool ShouldMerge(std::vector<std::vector<ImageMisc::Pixel>> group1, std::vector<std::vector<ImageMisc::Pixel>> group2, int splitValue);
bool ShouldMergeSecond(std::vector<std::vector<ImageMisc::Pixel>> group1, std::vector<std::vector<ImageMisc::Pixel>> group2, int splitValue);
void WriteMasksFiles(std::vector<ImageMisc::Pixel> pixels, int numberOfGroups, std::string masksSavePath);

void ImageEditor::region_split_merge(const std::shared_ptr<Image>& my_image, int splitValueSpliting, int splitValueMerging, int minSizeOfPixelsGroup, int& numOfMask)
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

	std::vector<std::vector<ImageMisc::Pixel>> pixels_vector;

	int indexIterator = 0;
	for (auto x = 0; x < temp_image->get_width(); x++)
	{
		pixels_vector.push_back(std::vector<ImageMisc::Pixel>());

		for (auto y = 0; y < temp_image->get_height(); y++)
		{
			ImageMisc::GetPixelXY(temp_image->get_data(), my_image->get_width(), x, y, &r, &g, &b, &a);

			pixels_vector[x].push_back(ImageMisc::Pixel(r, g, b, a, indexIterator, 0, x, y));
			indexIterator++;
		}
	}

	std::vector<std::vector<std::vector<ImageMisc::Pixel>>> groups;
	Split(pixels_vector, splitValueSpliting, groups, minSizeOfPixelsGroup);

	Merge(groups, splitValueMerging);
	SecondMerge(groups, splitValueMerging);

	std::vector<ImageMisc::Pixel> pixel_vector_new(pixels_vector.size() * pixels_vector.size());
	int countAll = 0;

	for (auto i = 0; i < groups.size(); i++)
	{
		for (int j = 0; j < groups[i].size(); j++)
		{
			for (int k = 0; k < groups[i][j].size(); k++)
			{
				pixel_vector_new[groups[i][j][k].Index] = groups[i][j][k];

				countAll++;
			}
		}
	}

	std::vector<ImageMisc::Pixel> pixels_vector_masks = pixel_vector_new;

	bool paint;
	for (int i = 0; i < pixel_vector_new.size(); i++)
	{
		paint = false;

		if (i < (sqrt(pixel_vector_new.size()) + 15) || i >(pixel_vector_new.size() - sqrt(pixel_vector_new.size() - 15)))
		{
			continue;
		}

		if (pixel_vector_new[i].Group != pixel_vector_new[i + 1].Group)
		{
			paint = true;
		}
		else if (pixel_vector_new[i].Group != pixel_vector_new[i - 1].Group)
		{
			paint = true;
		}
		else if (pixel_vector_new[i].Group != pixel_vector_new[i + sqrt(pixel_vector_new.size())].Group)
		{
			paint = true;
		}
		else if (pixel_vector_new[i].Group != pixel_vector_new[i - sqrt(pixel_vector_new.size())].Group)
		{
			paint = true;
		}

		if (paint)
		{
			pixel_vector_new[i].r = 255;
			pixel_vector_new[i].g = 255;
			pixel_vector_new[i].b = 255;
		}
	}

	for (auto k = 0; k < pixel_vector_new.size(); k++)
	{
		auto tempPixel = pixel_vector_new[k];

		r_val = tempPixel.r;
		g_val = tempPixel.g;
		b_val = tempPixel.b;
		a_val = tempPixel.a;;

		output_image_data.push_back(r_val);
		output_image_data.push_back(g_val);
		output_image_data.push_back(b_val);
		output_image_data.push_back(a_val);
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);

	numOfMask = groups.size();
	WriteMasksFiles(pixels_vector_masks, numOfMask, my_image->get_masks_path());
}

void Split(std::vector<std::vector<ImageMisc::Pixel>> pixels, int splitValue, std::vector<std::vector<std::vector<ImageMisc::Pixel>>>& groups_vector, int minSizeOfGroup)
{
	int lenghtOfXLabel = pixels.size() / 2;

	std::vector<std::vector<ImageMisc::Pixel>> pixels_1(lenghtOfXLabel);
	std::vector<std::vector<ImageMisc::Pixel>> pixels_2(lenghtOfXLabel);
	std::vector<std::vector<ImageMisc::Pixel>> pixels_3(lenghtOfXLabel);
	std::vector<std::vector<ImageMisc::Pixel>> pixels_4(lenghtOfXLabel);

	int pix_1_iter = 0;
	int pix_2_iter = 0;
	int pix_3_iter = 0;
	int pix_4_iter = 0;

	for (int x = 0; x < pixels.size(); x++)
	{
		for (int y = 0; y < pixels[x].size(); y++)
		{
			if (x < lenghtOfXLabel && y < lenghtOfXLabel)
			{
				pixels_1[pix_1_iter / (lenghtOfXLabel)].push_back(pixels[x][y]);
				pix_1_iter++;
			}
			else if (x >= lenghtOfXLabel && y < lenghtOfXLabel)
			{
				pixels_2[pix_2_iter / (lenghtOfXLabel)].push_back(pixels[x][y]);
				pix_2_iter++;
			}
			else if (x < lenghtOfXLabel && y >= lenghtOfXLabel)
			{
				pixels_3[pix_3_iter / (lenghtOfXLabel)].push_back(pixels[x][y]);
				pix_3_iter++;
			}
			else if (x >= lenghtOfXLabel && y >= lenghtOfXLabel)
			{
				pixels_4[pix_4_iter / (lenghtOfXLabel)].push_back(pixels[x][y]);
				pix_4_iter++;
			}
		}
	}


	if (ShouldSplitMore(pixels_1, splitValue, minSizeOfGroup))
	{
		Split(pixels_1, splitValue, groups_vector, minSizeOfGroup);
	}
	else
	{
		for (int i = 0; i < pixels_1.size(); i++)
		{
			for (int j = 0; j < pixels_1[i].size(); j++)
			{
				pixels_1[i][j].Group = groups_vector.size();
			}
		}

		groups_vector.push_back(pixels_1);
	}

	if (ShouldSplitMore(pixels_2, splitValue, minSizeOfGroup))
	{
		Split(pixels_2, splitValue, groups_vector, minSizeOfGroup);
	}
	else
	{
		for (int i = 0; i < pixels_2.size(); i++)
		{
			for (int j = 0; j < pixels_2[i].size(); j++)
			{
				pixels_2[i][j].Group = groups_vector.size();
			}
		}

		groups_vector.push_back(pixels_2);
	}

	if (ShouldSplitMore(pixels_3, splitValue, minSizeOfGroup))
	{
		Split(pixels_3, splitValue, groups_vector, minSizeOfGroup);
	}
	else
	{
		for (int i = 0; i < pixels_3.size(); i++)
		{
			for (int j = 0; j < pixels_3[i].size(); j++)
			{
				pixels_3[i][j].Group = groups_vector.size();
			}
		}

		groups_vector.push_back(pixels_3);
	}

	if (ShouldSplitMore(pixels_4, splitValue, minSizeOfGroup))
	{
		Split(pixels_4, splitValue, groups_vector, minSizeOfGroup);
	}
	else
	{
		for (int i = 0; i < pixels_4.size(); i++)
		{
			for (int j = 0; j < pixels_4[i].size(); j++)
			{
				pixels_4[i][j].Group = groups_vector.size();
			}
		}

		groups_vector.push_back(pixels_4);
	}
}

void Merge(std::vector<std::vector<std::vector<ImageMisc::Pixel>>>& groups_vector, int splitValue)
{
	int iterator = groups_vector.size();
	int i = 0;
	while (i < iterator)
	{
		if (ShouldMerge(groups_vector[i], groups_vector[i + 1], splitValue))
		{
			for (int j = 0; j < groups_vector[i + 1].size(); j++)
			{
				groups_vector[i].push_back(groups_vector[i + 1][j]);
			}

			groups_vector.erase(groups_vector.begin() + (i + 1));
		}
		else
		{
			i++;
		}

		iterator--;
	}

	for (auto i = 0; i < groups_vector.size(); i++)
	{
		for (int j = 0; j < groups_vector[i].size(); j++)
		{
			for (int k = 0; k < groups_vector[i][j].size(); k++)
			{
				groups_vector[i][j][k].Group = i;
			}
		}
	}
}

void SecondMerge(std::vector<std::vector<std::vector<ImageMisc::Pixel>>>& groups_vector, int splitValue)
{
	for (int i = 0; i < groups_vector.size(); i++)
	{
		for (int j = 0; j < groups_vector.size(); j++)
		{
			if (i == j)
			{
				continue;
			}

			if (ShouldMergeSecond(groups_vector[i], groups_vector[j], splitValue))
			{
				for (int k = 0; k < groups_vector[j].size(); k++)
				{
					groups_vector[i].push_back(groups_vector[j][k]);
				}
				groups_vector.erase(groups_vector.begin() + (j));
			}
		}
	}

	for (auto i = 0; i < groups_vector.size(); i++)
	{
		for (int j = 0; j < groups_vector[i].size(); j++)
		{
			for (int k = 0; k < groups_vector[i][j].size(); k++)
			{
				groups_vector[i][j][k].Group = i;
			}
		}
	}
}

bool ShouldSplitMore(std::vector<std::vector<ImageMisc::Pixel>> pixels, int splitValue, int minSizeOfGroup)
{
	int max = 0;
	int min = 1000;

	for (auto x = 0; x < pixels.size(); x++)
	{
		for (auto y = 0; y < pixels[x].size(); y++)
		{
			if ((pixels[x][y].r + pixels[x][y].g + pixels[x][y].b) > max)
			{
				max = pixels[x][y].r + pixels[x][y].g + pixels[x][y].b;
			}

			if ((pixels[x][y].r + pixels[x][y].g + pixels[x][y].b) < min)
			{
				min = pixels[x][y].r + pixels[x][y].g + pixels[x][y].b;
			}
		}
	}


	if (max - min <= splitValue)
	{
		return false;
	}
	else
	{
		if (pixels.size() * pixels.size() > minSizeOfGroup)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool ShouldMerge(std::vector<std::vector<ImageMisc::Pixel>> group1, std::vector<std::vector<ImageMisc::Pixel>> group2, int splitValue)
{
	int max_1 = 0;
	int min_1 = 1000;

	for (auto i = 0; i < group1.size(); i++)
	{
		for (auto j = 0; j < group1[i].size(); j++)
		{
			if ((group1[i][j].r + group1[i][j].g + group1[i][j].b) > max_1)
			{
				max_1 = group1[i][j].r + group1[i][j].g + group1[i][j].b;
			}

			if ((group1[i][j].r + group1[i][j].g + group1[i][j].b) < min_1)
			{
				min_1 = group1[i][j].r + group1[i][j].g + group1[i][j].b;
			}
		}
	}


	int max_2 = 0;
	int min_2 = 1000;

	for (auto i = 0; i < group2.size(); i++)
	{
		for (auto j = 0; j < group2[i].size(); j++)
		{
			if ((group2[i][j].r + group2[i][j].g + group2[i][j].b) > max_2)
			{
				max_2 = group2[i][j].r + group2[i][j].g + group2[i][j].b;
			}

			if ((group2[i][j].r + group2[i][j].g + group2[i][j].b) < min_2)
			{
				min_2 = group2[i][j].r + group2[i][j].g + group2[i][j].b;
			}
		}
	}


	if ((max_1 - min_2) <= splitValue && (max_2 - min_1) <= splitValue)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ShouldMergeSecond(std::vector<std::vector<ImageMisc::Pixel>> group1, std::vector<std::vector<ImageMisc::Pixel>> group2, int splitValue)
{
	int max_1 = 0;
	int min_1 = 1000;

	for (auto i = 0; i < group1.size(); i++)
	{
		for (auto j = 0; j < group1[i].size(); j++)
		{
			if ((group1[i][j].r + group1[i][j].g + group1[i][j].b) > max_1)
			{
				max_1 = group1[i][j].r + group1[i][j].g + group1[i][j].b;
			}

			if ((group1[i][j].r + group1[i][j].g + group1[i][j].b) < min_1)
			{
				min_1 = group1[i][j].r + group1[i][j].g + group1[i][j].b;
			}
		}
	}


	int max_2 = 0;
	int min_2 = 1000;

	for (auto i = 0; i < group2.size(); i++)
	{
		for (auto j = 0; j < group2[i].size(); j++)
		{
			if ((group2[i][j].r + group2[i][j].g + group2[i][j].b) > max_2)
			{
				max_2 = group2[i][j].r + group2[i][j].g + group2[i][j].b;
			}

			if ((group2[i][j].r + group2[i][j].g + group2[i][j].b) < min_2)
			{
				min_2 = group2[i][j].r + group2[i][j].g + group2[i][j].b;
			}
		}
	}

	bool Neighbour = false;


	if ((max_1 - min_2) <= splitValue && (max_2 - min_1) <= splitValue)
	{

		for (auto i = 0; i < group1.size(); i++)
		{
			for (auto j = 0; j < group1[i].size(); j++)
			{

				for (auto a = 0; a < group2.size(); a++)
				{
					for (auto b = 0; b < group2[a].size(); b++)
					{
						if (group1[i][j].X == group2[a][b].X)
						{
							if ((group1[i][j].Y == (group2[a][b].Y - 1)) || (group1[i][j].Y == (group2[a][b].Y + 1)))
							{
								Neighbour = true;
							}
						}
						else if (group1[i][j].Y == group2[a][b].Y)
						{
							if ((group1[i][j].X == (group2[a][b].X - 1)) || (group1[i][j].X == (group2[a][b].X + 1)))
							{
								Neighbour = true;
							}
						}

					}
				}
			}
		}

		if (Neighbour)
		{
			return true;
		}

		return false;
	}
	else
	{
		return false;
	}
}

void WriteMasksFiles(std::vector<ImageMisc::Pixel> pixels, int numberOfGroups, std::string masksSavePath)
{
	std::vector<ImageMisc::Pixel> tempPixels = pixels;

	std::vector<unsigned char> output_image_data;

	std::filesystem::path path_to_clear("masks");
	std::filesystem::remove_all(path_to_clear);
	std::filesystem::create_directory("masks");

	int r_val = 0;
	int g_val = 0;
	int b_val = 0;
	int a_val = 0;

	for (int g = 0; g < numberOfGroups; g++)
	{
		for (int i = 0; i < pixels.size(); i++)
		{
			if (tempPixels[i].Group == g)
			{
				tempPixels[i].r = 255;
				tempPixels[i].g = 255;
				tempPixels[i].b = 255;
			}
			else
			{
				tempPixels[i].r = 0;
				tempPixels[i].g = 0;
				tempPixels[i].b = 0;
			}
		}

		for (auto k = 0; k < tempPixels.size(); k++)
		{
			auto tempPixel = tempPixels[k];

			r_val = tempPixel.r;
			g_val = tempPixel.g;
			b_val = tempPixel.b;
			a_val = tempPixel.a;

			output_image_data.push_back(r_val);
			output_image_data.push_back(g_val);
			output_image_data.push_back(b_val);
			output_image_data.push_back(a_val);
		}

		std::string path = "masks/" + std::to_string(g) + ".bmp";

		ImageMisc::WriteImage(path.c_str(), 512, 512, 4, output_image_data.data());

		output_image_data.clear();
		tempPixels = pixels;
	}
}

void ImageEditor::add_mask(const std::shared_ptr<Image>& my_image, const std::shared_ptr<Image>& mask_image, bool& was_masked)
{
	std::shared_ptr<Image> temp_image = std::make_shared<Image>(my_image->get_save_path().c_str());
	std::shared_ptr<Image> temp_mask_image = std::make_shared<Image>(mask_image->get_filename());

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

	std::vector<ImageMisc::Pixel> pixel_vector_main;
	std::vector<ImageMisc::Pixel> pixel_vector_original;
	std::vector<ImageMisc::Pixel> pixel_vector_mask;

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		ImageMisc::GetPixel(temp_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
		pixel_vector_main.push_back(ImageMisc::Pixel(r, g, b, a));
	}

	for (auto i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		ImageMisc::GetPixel(my_image->get_original_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
		pixel_vector_original.push_back(ImageMisc::Pixel(r, g, b, a));
	}

	for (auto i = 0; i < temp_mask_image->get_width() * temp_mask_image->get_height(); i++)
	{
		ImageMisc::GetPixel(temp_mask_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
		pixel_vector_mask.push_back(ImageMisc::Pixel(r, g, b, a));
	}

	if (!was_masked)
	{
		for (int i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
		{
			pixel_vector_main[i].r = 0;
			pixel_vector_main[i].g = 0;
			pixel_vector_main[i].b = 0;
		}

		was_masked = true;
	}

	for (int i = 0; i < temp_image->get_width() * temp_image->get_height(); i++)
	{
		if (pixel_vector_mask[i].r == 255 && pixel_vector_mask[i].g == 255 & pixel_vector_mask[i].b == 255)
		{
			pixel_vector_main[i].r = pixel_vector_original[i].r;
			pixel_vector_main[i].g = pixel_vector_original[i].g;
			pixel_vector_main[i].b = pixel_vector_original[i].b;
		}
	}

	for (auto k = 0; k < pixel_vector_main.size(); k++)
	{
		auto tempPixel = pixel_vector_main[k];

		r_val = tempPixel.r;
		g_val = tempPixel.g;
		b_val = tempPixel.b;
		a_val = tempPixel.a;;

		output_image_data.push_back(r_val);
		output_image_data.push_back(g_val);
		output_image_data.push_back(b_val);
		output_image_data.push_back(a_val);
	}

	ImageMisc::LoadTextureFromData(output_image_data.data(), &my_image->get_texture(), my_image->get_width(), my_image->get_height());
	ImageMisc::WriteImage(my_image->get_save_path().c_str(), 512, 512, 4, output_image_data.data());
	my_image->set_main_data(output_image_data);
}