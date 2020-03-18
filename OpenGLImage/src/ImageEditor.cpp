#include "ImageEditor.hpp"

#include <vector>

void ImageEditor::change_brightness(std::shared_ptr<Image> my_image, int brightness)
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    std::vector<unsigned char> output_image_data;
	
    for (auto i = 0; i < my_image->get_width() * my_image->get_height(); i ++) {
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

        output_image_data.emplace_back(255-r);
        output_image_data.emplace_back(255-g);
        output_image_data.emplace_back(255-b);
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

unsigned char count_density(int sum, float g_min, float g_max, int size)
{
    return powf(powf(g_min, 0.33f) + ((powf(g_max, 0.33f) - powf(g_min, 0.33f)) * 1/size * sum), 3);
}
	
