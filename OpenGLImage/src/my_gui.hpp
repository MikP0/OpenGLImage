#pragma once

#include "ImageEditor.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imfilebrowser.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

namespace my_gui
{
	bool p_open = true;

	ImGui::FileBrowser fileDialog;

	void setup_file_dialog();
	void create_image_view(std::vector<std::shared_ptr<Image>> my_image);
	void draw_histogram(const std::shared_ptr<Image>& image);

	inline void init(const char* glsl_version, GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);


		setup_file_dialog();
	}
	void setup_file_dialog()
	{
		fileDialog.SetTitle("Open an image");
		fileDialog.SetTypeFilters({ ".bmp" });
	}
	inline void create_frame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	inline void create_mainmenu_window()

	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				fileDialog.Open();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		static std::vector<std::shared_ptr<Image>> image_vector;
		static bool show_output = false;


		fileDialog.Display();
		if (fileDialog.HasSelected())
		{
			std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
			image_vector.emplace_back(std::make_shared<Image>(fileDialog.GetSelected().string().c_str()));
			show_output = true;
			fileDialog.ClearSelected();
		}

		if (show_output) create_image_view(image_vector);
	};

	void create_basic_modifiers(std::shared_ptr<Image>);

	inline void create_image_view(std::vector<std::shared_ptr<Image>> image_vector)
	{
		for (auto my_image : image_vector) {
			static float size = 1;
			ImGui::Begin(my_image->get_filename().c_str(), &p_open);
			ImGui::Columns(2);
			ImGui::Text("size = %d x %d", my_image->get_width(), my_image->get_height());
			if (ImGui::SliderFloat("Size", &size, 0.1, 1.5))
				my_image->set_size(size);
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(my_image->get_texture())),
				ImVec2(my_image->get_width() * my_image->get_size(), my_image->get_height() * my_image->get_size()));
			create_basic_modifiers(my_image);
			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
	}

	inline void create_basic_modifiers(std::shared_ptr<Image> my_image)
	{
		static int brightness = 0;
		static float contrast = 1;
		static bool negative = false;
		static bool show_histogram = false;
		static bool show_density = false;
		static float g_max = 1.0f;
		static float g_min = 0.1f;

		static bool filt_3x3 = false;
		static int filt_matrix3x3[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };

		static bool filt_5x5 = false;
		static int filt_matrix5x5[25] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

		static bool filt_med_3x3 = false;
		static bool filt_med_5x5 = false;

		static bool filt_mean_3x3 = false;
		static bool filt_mean_5x5 = false;

		static bool filt_roberts = false;

		static bool show_fft_button = false;
		static bool show_fft = false;

		ImGui::NextColumn();
		if (show_histogram) {
			if (ImGui::CollapsingHeader("Histograms")) {
				draw_histogram(my_image);

				ImGui::Checkbox("Density function - power of 2/3", &show_density);
				if (show_density)
				{
					if (ImGui::CollapsingHeader("Density fucntion options"))
					{
						ImGui::SliderFloat("g_min", &g_min, 0.0f, 255.0f);
						ImGui::SliderFloat("g_max", &g_max, 0.0f, 255.0f);
					}
				}
			}
		}
		if (ImGui::CollapsingHeader("Basic options", 1)) {
			ImGui::SliderInt("Brightness", &brightness, -255, 255);
			ImGui::SliderFloat("Contrast", &contrast, 0.1f, 8.0f);
			ImGui::Checkbox("Negative", &negative);
			ImGui::Checkbox("FilterMedian3x3", &filt_med_3x3);
			ImGui::Checkbox("FilterMedian5x5", &filt_med_5x5);
			ImGui::Checkbox("FilterMean3x3", &filt_mean_3x3);
			ImGui::Checkbox("FilterMean5x5", &filt_mean_5x5);

			ImGui::Checkbox("Filter3x3", &filt_3x3);
	 
			if (filt_3x3)
			{
				ImGui::InputScalarN(" ", ImGuiDataType_S32, filt_matrix3x3, 3);
				ImGui::InputScalarN("  ", ImGuiDataType_S32, filt_matrix3x3 + 3, 3);
				ImGui::InputScalarN("   ", ImGuiDataType_S32, filt_matrix3x3 + 6, 3);
			}

			ImGui::Checkbox("Filter5x5", &filt_5x5);

			if (filt_5x5)
			{
				ImGui::InputScalarN(" ", ImGuiDataType_S32, filt_matrix5x5, 5);
				ImGui::InputScalarN("  ", ImGuiDataType_S32, filt_matrix5x5 + 5, 5);
				ImGui::InputScalarN("   ", ImGuiDataType_S32, filt_matrix5x5 + 10, 5);
				ImGui::InputScalarN("    ", ImGuiDataType_S32, filt_matrix5x5 + 15, 5);
				ImGui::InputScalarN("     ", ImGuiDataType_S32, filt_matrix5x5 + 20, 5);
			}		

			ImGui::Checkbox("FilterRoberts", &filt_roberts);		
		}

		if (show_fft) {
			ImGui::Begin("FFT");
			static int page = 1;
			ImGui::Columns(3);
			if (ImGui::Button("Phase")) {
				page = 1;
			}
			ImGui::NextColumn();
			if (ImGui::Button("Magnitude")) {
				page = 2;
			}
			ImGui::NextColumn();
			if (ImGui::Button("Filters")) {
				page = 3;
			}
			ImGui::Columns();
			switch (page) {
				case 1:
					ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(my_image->get_fftPhase_texture())),
						ImVec2(my_image->get_width() * my_image->get_size(), my_image->get_height() * my_image->get_size()));
					break;
				case 2:
					ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(my_image->get_fftMagnitude_texture())),
						ImVec2(my_image->get_width() * my_image->get_size(), my_image->get_height() * my_image->get_size()));
					break;
				case 3:
					static int group = 0;
					static bool isLowpass = false;
					static int radius = 0;
					static int size = 0;

					static float angle = 0.0;
					static float angleOffset = 0.0;

					static int k = 0;
					static int l = 0;

					ImGui::RadioButton("Lowpass Filter", &group, 0);
					ImGui::RadioButton("Highpass Filter", &group, 1);
					ImGui::RadioButton("Bandpass Filter", &group, 2);
					ImGui::RadioButton("Bandstop Filter", &group, 3);
					ImGui::RadioButton("Edge Detection Filter", &group, 4);
					ImGui::RadioButton("Spectrum Filter", &group, 5);

					switch (group)
					{
					case 0:
						ImGui::SliderInt("Radius", &radius, 0, 250);
						if (ImGui::Button("Proceed")) {
							ImageEditor::fft_filter_lowpass(my_image, radius);
						}
						break;
					case 1:
						ImGui::SliderInt("Radius", &radius, 0, 250);
						if (ImGui::Button("Proceed")) {
							ImageEditor::fft_filter_highpass(my_image, radius);
						}
						break;
					case 2:
						ImGui::SliderInt("Radius", &radius, 0, 250);
						ImGui::SliderInt("Size", &size, 0, 250);
						if (ImGui::Button("Proceed")) {
							ImageEditor::fft_filter_bandpass(my_image, radius, size);
						}
						break;
					case 3:
						ImGui::SliderInt("Radius", &radius, 0, 250);
						ImGui::SliderInt("Size", &size, 0, 250);
						if (ImGui::Button("Proceed")) {
							ImageEditor::fft_filter_bandstop(my_image, radius, size);
						}
						break;
					case 4:
						ImGui::SliderAngle("Angle", &angle, 0, 90);
						ImGui::SliderAngle("Angle offset", &angleOffset, 0, 180);
						ImGui::SliderInt("Radius", &radius, 0, 250);
						if (ImGui::Button("Proceed")) {
							ImageEditor::fft_filter_edgedetect(my_image, angle, angleOffset, radius);
						}
						break;
					case 5:
						ImGui::SliderInt("Horizontal", &k, 0, 512);
						ImGui::SliderInt("Vertical", &l, 0, 512);
						if (ImGui::Button("Proceed")) {
							ImageEditor::fft_filter_spectrum(my_image, k, l);
						}
						break;
					}
					
			}


			ImGui::End();
		}
		if (ImGui::Button("Proceed"))
		{
			ImageEditor::change_brightness(my_image, brightness);
			ImageEditor::change_contrast(my_image, contrast);

			if (negative)
				ImageEditor::change_negative(my_image);

			if (show_density)
				ImageEditor::density_function(my_image, g_min, g_max);

			if (filt_med_3x3)
				ImageEditor::filter_median(my_image, 3);

			if (filt_med_5x5)
				ImageEditor::filter_median(my_image, 5);

			if (filt_mean_3x3)
				ImageEditor::filter_mean(my_image, 3);

			if (filt_mean_5x5)
				ImageEditor::filter_mean(my_image, 5);

			if (filt_3x3)
				ImageEditor::filter_convolution(my_image, 3, filt_matrix3x3);

			if (filt_5x5)
				ImageEditor::filter_convolution(my_image, 5, filt_matrix5x5);

			if (filt_roberts)
				ImageEditor::filter_roberts(my_image, 3);

			show_histogram = true;
			show_fft_button = true;
		}
		if (show_fft_button) {
			if (ImGui::Button("FFT")) {
				ImageEditor::_2dfft(my_image);
				show_fft = true;
			}
		}
	}

	inline void draw_histogram(const std::shared_ptr<Image>& image)
	{
		auto re = std::make_shared<std::vector<float>>(image->getReds(Colors::RED));
		auto gr = std::make_shared<std::vector<float>>(image->getReds(Colors::GREEN));
		auto bl = std::make_shared<std::vector<float>>(image->getReds(Colors::BLUE));

		image->set_red_histogram(*re);
		image->set_green_histogram(*gr);
		image->set_blue_histogram(*bl);
		
		auto max_red = std::max_element(std::begin(*re), std::end(*re)); //MP: We'll leave it here for now
		auto max_green = std::max_element(std::begin(*gr), std::end(*gr));
		auto max_blue = std::max_element(std::begin(*bl), std::end(*bl));

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(255, 0, 0));
		ImGui::PlotHistogram("", re->data(), re->size(), 0, "RED HISTOGRAM", 0.0f, 4500, ImVec2(500, 100));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(0, 255, 0));
		ImGui::PlotHistogram("", gr->data(), gr->size(), 0, "GREEN HISTOGRAM", 0.0f, 4500, ImVec2(500, 100));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(0, 0, 255));
		ImGui::PlotHistogram("", bl->data(), bl->size(), 0, "BLUE HISTOGRAM", 0.0f, 4500, ImVec2(500, 100));
		ImGui::PopStyleColor(3);
	}
}
