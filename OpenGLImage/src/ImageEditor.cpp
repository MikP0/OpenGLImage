#include "ImageEditor.hpp"

#include <vector>

void ImageEditor::change_brightness(std::shared_ptr<Image> my_image, int brightness)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	std::vector<unsigned char> output_image_data;

	for (auto i = 0; i < my_image->get_width() * my_image->get_height(); i++) {
		ImageMisc::GetPixel(my_image->get_data(), my_image->get_width(), i % my_image->get_width(), i / my_image->get_width(), &r, &g, &b, &a);
		if (r + brightness < 0)
			output_image_data.push_back(0);
		//output_image_data.emplace_back(0);
		else if (r + brightness > 255)
			output_image_data.push_back(255);
		else
			output_image_data.push_back(r + brightness);

		if (g + brightness < 0)
			output_image_data.push_back(0);
		else if (g + brightness > 255)
			output_image_data.push_back(255);
		else
			output_image_data.push_back(g + brightness);

		if (b + brightness < 0)
			output_image_data.push_back(0);
		else if (b + brightness > 255)
			output_image_data.push_back(255);
		else
			output_image_data.push_back(b + brightness);

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
