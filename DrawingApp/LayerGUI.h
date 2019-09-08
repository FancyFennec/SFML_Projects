#pragma once

#include <iostream>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "GlobalVariables.h"
#include "Scene.h"
#include "CommandManager.h"

void layerGUI(Scene& scene);

bool layerNamePopupIsOpen = false;
static char layerName[128] = "";


void layerGUI(Scene& scene)
{
	if (SHOW_GUI) {
		ImGui::SetNextWindowSize(ImVec2(150, WINDOW_HEIGHT / 2));
		ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH - 150, 18));

		ImGui::Begin("Layers", &SHOW_GUI, ImGuiWindowFlags_NoResize);
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

				// Image of the layer with white border if it is selected
				if (iter == scene.currentLayer) {
					ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
				}
				else {
					ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150, 150, 150));
				}
				ImGui::Image(iter->sprite, sf::Vector2f(30, 30));

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
				delButton.append(std::to_string(scene.getDistance(iter))); // If we don't append the layer name imgui is confused when we press the button
				if (ImGui::Button(delButton.data())) {
					CommandManager::deleteLayer(iter);
				}
				layerNumber--;
			}
		}
		ImGui::End();

		if (layerNamePopupIsOpen) {
			ImGui::SetNextWindowSize(ImVec2(300, 80));
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
}