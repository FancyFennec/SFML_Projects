#pragma once

#include <iostream>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "GlobalVariables.h"
#include "Scene.h"
#include "CommandManager.h"

void layerGUI(Scene& scene);

void CreateNewLayer(Scene & scene);

void LayerNamePopup(Scene & scene);

void DrawLayerList(Scene & scene);

void MaterialSettings(Scene & scene);

int layerNumber;
bool layerNamePopupIsOpen = false;
static char layerName[128] = "";
const int WIDTH = 250;

void layerGUI(Scene& scene)
{
	if (SHOW_GUI) {
		ImGui::SetNextWindowSize(ImVec2(WIDTH, WINDOW_HEIGHT / 2));
		ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH - WIDTH, 18));

		ImGui::Begin("Layers", &SHOW_GUI, ImGuiWindowFlags_NoResize);
		{
			if (ImGui::CollapsingHeader("Material Settings")) MaterialSettings(scene);

			if (ImGui::Button("New Layer")) CreateNewLayer(scene);

			layerNumber = scene.layers.size();
			DrawLayerList(scene);
		}
		ImGui::End();

		if (layerNamePopupIsOpen) LayerNamePopup(scene);
	}
}

void CreateNewLayer(Scene & scene)
{
	if (scene.lastActiveLayer < scene.layers.end()) {
		CommandManager::createLayer();
	}
	else {
		std::cout << "ERROR! Maxing number of Layers reached!!!" << std::endl;
	}
	scene.lastActiveLayer->layerName = "Layer";
	scene.lastActiveLayer->layerName.append(std::to_string(scene.getSize()));
}

void LayerNamePopup(Scene & scene)
{
	ImGui::SetNextWindowSize(ImVec2(300, 80));
	ImGui::OpenPopup("Chose Layer Name");
	if (ImGui::BeginPopupModal("Chose Layer Name", &layerNamePopupIsOpen)) {

		if (ImGui::InputText("Press Enter", layerName, sizeof(layerName), ImGuiInputTextFlags_EnterReturnsTrue)) {
			scene.currentLayer->layerName = std::string(layerName);
			layerNamePopupIsOpen = false;
		}
		ImGui::Dummy(ImVec2(0, 30));
		ImGui::SameLine(ImGui::GetWindowSize().x - 60);
		if (ImGui::Button("Close", ImVec2(50, 20))) layerNamePopupIsOpen = false;
		ImGui::EndPopup();
	}
}

void DrawLayerList(Scene & scene)
{
	// Draw list of all the layers
	sf::Color borderColor;
	for (auto iter = scene.lastActiveLayer; iter > scene.layers.begin(); std::advance(iter, -1)) { // Don't draw the first layer

																								   // Image of the layer with white border if it is selected
		if (iter == scene.currentLayer) {
			borderColor = sf::Color::White;
		}
		else {
			borderColor = sf::Color(150, 150, 150);
		}
		ImGui::Image(iter->sprite, sf::Vector2f(30, 30), sf::Color::White, borderColor);

		// Button that sets the current Layer to the current iterator
		// Double clicking lets one chose a layer name
		ImGui::SameLine();
		std::string buttonName = iter->layerName;
		buttonName.append("##");
		buttonName.append(std::to_string(layerNumber));

		if (ImGui::Button(buttonName.data())) {
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
		delButton.append(std::to_string(scene.getLayerDistance(iter))); // If we don't append the layer name imgui is confused when we press the button
		if (ImGui::Button(delButton.data())) {
			CommandManager::deleteLayer(iter);
		}
		layerNumber--;
	}
}

void MaterialSettings(Scene & scene)
{
	ImGui::SliderFloat("Shininess", &scene.currentLayer->material.shininess, 1.0f, 256.0f);
	ImGui::SliderFloat("AmbInt", &scene.currentLayer->material.ambInt, 0.0f, 1.0f);
	ImGui::SliderFloat("DifInt", &scene.currentLayer->material.difInt, 0.0f, 1.0f);
	ImGui::SliderFloat("SpecInt", &scene.currentLayer->material.specInt, 0.0f, 1.0f);
}
