#pragma once

#include <iostream>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "Settings.h"
#include "Scene.h"
#include "CommandManager.h"

void mainMenuGUI(sf::RenderWindow& mainWindow);
void brushGUI(sf::RenderWindow& mainWindow, sf::RenderWindow& brushWindow, Scene& scene);
void layerGUI(Scene& scene);
void createBrushWindow(sf::RenderWindow& mainWindow, sf::RenderWindow& brushWindow, Scene& scene);


void mainMenuGUI(sf::RenderWindow& mainWindow)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save as JPEG"))
			{
				std::cout << "Save as JPEG" << std::endl;
			}
			if (ImGui::MenuItem("Save as PNG"))
			{
				std::cout << "Save as PNG" << std::endl;
			}
			if (ImGui::MenuItem("Save Layer as JPEG"))
			{
				std::cout << "Save Layer as JPEG" << std::endl;
			}
			if (ImGui::MenuItem("Save Layer as PNG"))
			{
				std::cout << "Save Layer as PNG" << std::endl;
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
	}
}

void brushGUI(sf::RenderWindow& mainWindow, sf::RenderWindow& brushWindow, Scene& scene)
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
			createBrushWindow(mainWindow, brushWindow, scene);
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
				CommandManager::createLayer(std::distance(scene.layers.begin(), scene.lastActiveLayer));
			}
			else {
				std::cout << "ERROR! Maxing number of Layers reached!!!" << std::endl;
			}
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
			std::string layerName = "Layer";
			layerName.append(std::to_string(std::distance(scene.layers.begin(), iter)));
			if (ImGui::Button(layerName.data())) {
				if (iter != scene.layers.begin()) scene.currentLayer = iter;
			}

			//Delete button
			ImGui::SameLine();
			std::string delButton = "Del##";
			delButton.append(layerName); // If we don't append the layer name imgui is confused when we press the button
			if (ImGui::Button(delButton.data())) {
				CommandManager::deleteLayer(std::distance(scene.layers.begin(), iter), iter->tex);
			}
			layerNumber--;
		}
	}
	ImGui::End();
}

void createBrushWindow(sf::RenderWindow& mainWindow, sf::RenderWindow& brushWindow, Scene& scene)
{
	brushWindow.create(sf::VideoMode(scene.brushWidth, scene.brushWidth), "Define Brush", sf::Style::Titlebar);
	brushWindow.setFramerateLimit(120);
	brushWindow.clear(sf::Color::Black);
	brushWindow.setPosition(mainWindow.getPosition() +
		sf::Vector2i(WINDOW_WIDTH / 2 - scene.brushWidth / 2, WINDOW_HEIGHT / 2 - scene.brushWidth / 2));
}
