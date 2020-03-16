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
    
        ImGui::NextColumn();
        if (show_histogram) {
            if (ImGui::CollapsingHeader("Histograms")) {
                draw_histogram(my_image);
            }
        }
        ImGui::SliderInt("Brightness", &brightness, -255, 255);
        ImGui::SliderFloat("Contrast", &contrast, 0.1f, 8.0f);
        ImGui::Checkbox("Negative", &negative);
    	
    	if(ImGui::Button("Proceed"))
    	{
            ImageEditor::change_brightness(my_image, brightness);
            ImageEditor::change_contrast(my_image, contrast);
    		
            if (negative)
                ImageEditor::change_negative(my_image);

            show_histogram = true;
    	}	
    }
	
    void draw_histogram(const std::shared_ptr<Image>& image)
    {
        auto re = std::make_shared<std::vector<float>>(image->getReds(Colors::RED));
    	auto gr = std::make_shared<std::vector<float>>(image->getReds(Colors::GREEN));
        auto bl = std::make_shared<std::vector<float>>(image->getReds(Colors::BLUE));

    	auto max_red = std::max_element(std::begin(*re), std::end(*re)); //MP: We'll leave it here for now
        auto max_green = std::max_element(std::begin(*gr), std::end(*gr));
    	auto max_blue = std::max_element(std::begin(*bl), std::end(*bl));
    	
		ImGui::PlotHistogram("", re->data(),re->size(), 0, "RED HISTOGRAM", 0.0f, 4500, ImVec2(400, 100));
        ImGui::PlotHistogram("", gr->data(), gr->size(), 0, "GREEN HISTOGRAM", 0.0f, 4500, ImVec2(400, 100));
        ImGui::PlotHistogram("", bl->data(), bl->size(), 0, "BLUE HISTOGRAM", 0.0f, 4500, ImVec2(400, 100));
    }
}
