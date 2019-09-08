#pragma once

#include <iostream>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "GlobalVariables.h"
#include "Scene.h"
#include "CommandManager.h"

void mainMenuGUI(Scene& scene);

bool saveFilePopupIsOpen = false;
std::string input;

static enum FILE_TYPE {
	PNG,
	JPEG,
	SINGLE_PNG,
	SINGLE_JPEG,
	SCN
};

FILE_TYPE file_type;

void mainMenuGUI(Scene& scene)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load Scene"))
			{
				std::cout << "Loading Scene" << std::endl;
			}
			if (ImGui::MenuItem("Load Layer"))
			{
				std::cout << "Loading Layer" << std::endl;
			}
			if (ImGui::MenuItem("Saving Scene"))
			{
				std::cout << "Save Scene" << std::endl;
			}
			if (ImGui::MenuItem("Save Scene as JPEG"))
			{
				file_type = JPEG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Scene as PNG"))
			{
				file_type = PNG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Layer as JPEG"))
			{
				file_type = SINGLE_JPEG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Layer as PNG"))
			{
				file_type = SINGLE_PNG;
				saveFilePopupIsOpen = true;
				input = "Input FileName and";
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("menu item"))
			{
				std::cout << "Menu item pressed" << std::endl;
			}
			ImGui::EndMenu();
		}

		if (ImGui::ArrowButton("##Back", ImGuiDir_Left))
		{
			CommandManager::moveBackward();
		}
		if (ImGui::ArrowButton("##Forward", ImGuiDir_Right))
		{
			CommandManager::moveForward();
		}

		ImGui::SameLine(WINDOW_WIDTH - 60);
		if (ImGui::Button("Close"))
		{
			mainWindow.close();
		}

		ImGui::EndMainMenuBar();

		if (saveFilePopupIsOpen) {
			ImGui::OpenPopup("Save Window");
			if (ImGui::BeginPopupModal("Save Window", &saveFilePopupIsOpen)) {
				std::string folderPath("./SavedFiles");
				for (auto p : fs::directory_iterator(folderPath)) {
					std::string fileName = p.path().string().substr(folderPath.size() + 1); //Remove folder name
					if (ImGui::Button(fileName.data(), ImVec2(200, 20))) {
						input = fileName.substr(0, fileName.length() - 4).c_str(); //remove ending
					}
				}

				ImGui::Dummy(ImVec2(0, 10));
				ImGui::DrawLine(sf::Vector2f(0, 0), sf::Vector2f(WINDOW_WIDTH, 0), sf::Color::White, 1.0f);
				ImGui::Dummy(ImVec2(0, 20));

				char* inputChar = (char*)input.data();
				if (ImGui::InputText("Press Enter", inputChar, 30, ImGuiInputTextFlags_EnterReturnsTrue)) {
					std::string file = "./SavedFiles/";
					file.append(inputChar);
					switch (file_type) {
					case(JPEG):
					case(SINGLE_JPEG):
						file.append(".jpg");
						break;
					case(PNG):
					case(SINGLE_PNG):
						file.append(".png");
						break;
					case(SCN):
						file.append(".scn");
						break;
					}

					sf::RenderTexture rTex;
					rTex.create(scene.width, scene.height);
					switch (file_type) {
					case(JPEG): {
						rTex.clear(sf::Color::White);
						for (auto layer = scene.layers.begin(); layer != scene.layers.end(); layer++) {
							rTex.draw(layer->sprite);
						}
						break;
					}
					case(PNG): {
						rTex.clear(sf::Color(255, 255, 255, 0));
						for (auto layer = std::next(scene.layers.begin()); layer != scene.layers.end(); layer++) {
							rTex.draw(layer->sprite);
						}
						break;
					}
					case(SINGLE_JPEG): {
						rTex.clear(sf::Color::White);
						rTex.draw(scene.layers.begin()->sprite);
						rTex.draw(scene.currentLayer->sprite);
						break;
					}
					case(SINGLE_PNG): {
						rTex.clear(sf::Color(255, 255, 255, 0));
						rTex.draw(scene.currentLayer->sprite);
						break;
					}
					case(SCN): {
						//TODO: Create Folder and save all the textures;
					}
					}

					rTex.display();
					rTex.getTexture().copyToImage().saveToFile(file.data());

					saveFilePopupIsOpen = false;
				}
				ImGui::Dummy(ImVec2(0, 30));
				ImGui::SameLine(ImGui::GetWindowSize().x - 60);
				if (ImGui::Button("Close", ImVec2(50, 20))) saveFilePopupIsOpen = false;
				ImGui::EndPopup();
			}
		}
	}
}