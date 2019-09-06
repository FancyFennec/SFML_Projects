#pragma once

#include <iostream>

#include "GlobalVariables.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "Settings.h"
#include "Scene.h"
#include "CommandManager.h"

void mainMenuGUI(Scene& scene);
void brushGUI(Scene& scene);
void layerGUI(Scene& scene);
void createBrushWindow(Scene& scene);

bool popupIsOpen = false;
bool layerNamePopupIsOpen = false;
std::string input;
static char layerName[128] = "";

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
				popupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Scene as PNG"))
			{
				file_type = PNG;
				popupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Layer as JPEG"))
			{
				file_type = SINGLE_JPEG;
				popupIsOpen = true;
				input = "Input FileName and";
			}
			if (ImGui::MenuItem("Save Layer as PNG"))
			{
				file_type = SINGLE_PNG;
				popupIsOpen = true;
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

		ImGui::SameLine(WINDOW_WIDTH - 60);
		if (ImGui::Button("Close"))
		{
			mainWindow.close();
		}

		ImGui::EndMainMenuBar();

		if (popupIsOpen) {
			ImGui::OpenPopup("Save Window");
			if (ImGui::BeginPopupModal("Save Window", &popupIsOpen)) {
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

					popupIsOpen = false;
				}
				ImGui::Dummy(ImVec2(0, 30));
				ImGui::SameLine(ImGui::GetWindowSize().x - 60);
				if (ImGui::Button("Close", ImVec2(50, 20))) popupIsOpen = false;
				ImGui::EndPopup();
			}
		}
	}
}

void brushGUI(Scene& scene)
{
	ImGui::SetNextWindowSize(ImVec2(250, WINDOW_HEIGHT - 250));
	ImGui::SetNextWindowPos(ImVec2(0, 50));
	bool windowFlag = true;

	ImGui::Begin("Brush Settings", &windowFlag, ImGuiWindowFlags_NoResize);
	if (ImGui::CollapsingHeader("Colour Picker")) {
		if (ImGui::ColorPicker3("Colour", scene.currentColor)) {
			scene.updateColor();
		}
	}
	if (ImGui::CollapsingHeader("Settings")) {
		ImGui::SliderFloat("Spacing", &(*scene.currentBrush)->stepsize, 0, 500);
		if (ImGui::SliderFloat("Size", &scene.brushSize, 0, 1)) (*scene.currentBrush)->setBrushSize(scene.brushSize);
		ImGui::SliderInt("Opacity", &(*scene.currentBrush)->opacity, 0, 255);
		ImGui::SliderInt("Flow", &(*scene.currentBrush)->flow, 0, 255);
	}
	if (ImGui::CollapsingHeader("Scatter")) {
		ImGui::SliderFloat("S-Scatter", &(*scene.currentBrush)->scaterScale, 0, 1);
		ImGui::SliderFloat("P-Scatter", &(*scene.currentBrush)->scaterPos, 0, 1000);
		ImGui::SliderFloat("A-Scatter", &(*scene.currentBrush)->scaterAngle, 0, 180);
	}

	//Button to create a new brush
	if (ImGui::SmallButton("New Brush")) {
		if (scene.brushes.size() == 20) {
			std::cout << "ERROR! Maximum number of brushes reached." << std::endl;
		}
		else {
			scene.brushLayer.clearLayer();
			createBrushWindow(scene);
		}
	}

	// Draw list of all brushes
	int brushNumber = 1;
	if (scene.brushes.begin() == scene.currentBrush) {
		ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
	}
	else {
		ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150, 150, 150));
	}
	ImGui::Image((*scene.brushes.begin())->sprite, sf::Vector2f(30, 30));

	ImGui::SameLine();
	std::string brushName = "Brush";
	brushName.append(std::to_string(brushNumber));

	//Button that sets the current brush to the current iterator
	if (ImGui::Button(brushName.data())) {
		scene.currentBrush = scene.brushes.begin();
	}
	brushNumber++;
	for (auto iter = std::prev(scene.brushes.end()); iter > scene.brushes.begin(); std::advance(iter, -1)) {

		//Image of the brush with white border if it is selected
		if (iter == scene.currentBrush) {
			ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
		}
		else {
			ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150, 150, 150));
		}
		ImGui::Image((*iter)->sprite, sf::Vector2f(30, 30));

		ImGui::SameLine();
		std::string brushName = "Brush";
		brushName.append(std::to_string(brushNumber));

		//Button that sets the current brush to the current iterator
		if (ImGui::Button(brushName.data())) {
			scene.currentBrush = iter;
		}

		//Delete button
		if (iter != scene.brushes.begin()) {
			ImGui::SameLine();
			std::string delButton = "Del##";
			delButton.append(brushName); // If we don't append the brush name imgui is confused when we press the button

			if (ImGui::Button(delButton.data())) {
				auto iterDist = std::distance(scene.brushes.begin(), scene.currentBrush);
				scene.currentBrush = scene.brushes.begin();


				// This makes sure that we continue working on the brush we were before deleting
				bool isCurrentBrushBelowIter = scene.brushes.begin() + iterDist < iter;
				iter = scene.brushes.erase(iter);
				if (isCurrentBrushBelowIter) {
					std::advance(scene.currentBrush, iterDist);
				}
				else {
					std::advance(scene.currentBrush, iterDist - 1);
				}
			}
		}
		brushNumber++;
	}
	ImGui::End();
}

void layerGUI(Scene& scene)
{
	ImGui::SetNextWindowSize(ImVec2(150, WINDOW_HEIGHT / 2));
	ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH - 150, 50));
	bool windowFlag = true;

	ImGui::Begin("Layers", &windowFlag, ImGuiWindowFlags_NoResize);
	{
		//Button to create a new Layer
		if (ImGui::Button("New Layer")) {
			if (scene.lastActiveLayer < scene.layers.end()) {
				CommandManager::createLayer();
			}
			else {
				std::cout << "ERROR! Maxing number of Layers reached!!!" << std::endl;
			}
			scene.lastActiveLayer->layerName = "Layer";
			scene.lastActiveLayer->layerName.append(std::to_string(scene.getSize()));
		}
		int layerNumber = scene.layers.size();

		// Draw list of all the layers
		for (auto iter = scene.lastActiveLayer; iter > scene.layers.begin(); std::advance(iter, -1)) {

			//Image of the layer with white border if it is selected
			if (iter == scene.currentLayer) {
				ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
			}
			else {
				ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150, 150, 150));
			}
			ImGui::Image(iter->sprite, sf::Vector2f(30, 30));

			//Button that sets the current Layer to the current iterator
			ImGui::SameLine();
			if (ImGui::Button(iter->layerName.data())) {
				if (iter != scene.layers.begin()) scene.currentLayer = iter;
				if (clickClock.getElapsedTime().asMilliseconds() < DOUBLE_CLICK_SPEED) {
					layerNamePopupIsOpen = true;
					strcpy_s(layerName, "");
				}
				clickClock.restart();
			}

			//Delete button
			ImGui::SameLine();
			std::string delButton = "Del##";
			delButton.append(std::to_string(scene.getDistance(iter))); // If we don't append the layer name imgui is confused when we press the button
			if (ImGui::Button(delButton.data())) {
				CommandManager::deleteLayer(iter);
			}
			layerNumber--;
		}
	}
	ImGui::End();

	if (layerNamePopupIsOpen) {
		ImGui::OpenPopup("Chose Layer Name");
		if (ImGui::BeginPopupModal("Chose Layer Name", &layerNamePopupIsOpen)) {

			if (ImGui::InputText("Press Enter", layerName, sizeof(layerName), ImGuiInputTextFlags_EnterReturnsTrue)) {
				//TODO: not working yet... layer doesn't inherit the name
				scene.currentLayer->layerName = std::string(layerName);
				layerNamePopupIsOpen = false;
			}
			ImGui::Dummy(ImVec2(0, 30));
			ImGui::SameLine(ImGui::GetWindowSize().x - 60);
			if (ImGui::Button("Close", ImVec2(50, 20))) layerNamePopupIsOpen = false;
			ImGui::EndPopup();
		}
	}
}

void createBrushWindow(Scene& scene)
{
	brushWindow.create(sf::VideoMode(scene.brushWidth, scene.brushWidth), "Define Brush", sf::Style::Titlebar);
	brushWindow.setFramerateLimit(120);
	brushWindow.clear(sf::Color::Black);
	brushWindow.setPosition(mainWindow.getPosition() +
		sf::Vector2i(WINDOW_WIDTH / 2 - scene.brushWidth / 2, WINDOW_HEIGHT / 2 - scene.brushWidth / 2));
}
