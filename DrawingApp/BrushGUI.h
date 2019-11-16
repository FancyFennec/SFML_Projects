#pragma once

#include <iostream>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "GlobalVariables.h"
#include "Scene.h"
#include "CommandManager.h"

void brushGUI(Scene& scene);
void CreateNewBrush(Scene & scene);
void NewBrushPopup(Scene & scene);
void BrushNamePopup(Scene & scene);
void ColorPickers(Scene & scene);
void BrushSettings(Scene & scene);
void DrawBrushList(Scene & scene);
void ScatterSettings(Scene & scene);

static const ImGuiColorEditFlags colorEditFlags =
ImGuiColorEditFlags_NoSidePreview |
ImGuiColorEditFlags_NoSmallPreview |
ImGuiColorEditFlags_NoAlpha |
ImGuiColorEditFlags_NoLabel |
ImGuiColorEditFlags_NoInputs;


bool brushNamePopupIsOpen = false;
bool newBrushPopupIsOpen = false;
static char brushName[128] = "";

void brushGUI(Scene& scene)
{
	if (SHOW_GUI) {
		ImGui::SetNextWindowSize(ImVec2(250, WINDOW_HEIGHT - 250));
		ImGui::SetNextWindowPos(ImVec2(0, 18));

		ImGui::Begin("Brush Settings", &SHOW_GUI, ImGuiWindowFlags_NoResize);
		ColorPickers(scene);
		
		ImGui::PushItemWidth(170);
		if (ImGui::CollapsingHeader("Brush Settings")) BrushSettings(scene);

		ImGui::PushItemWidth(177);
		if (ImGui::CollapsingHeader("Scatter Settings")) ScatterSettings(scene);

		if (ImGui::SmallButton("New Brush")) CreateNewBrush(scene);

		DrawBrushList(scene);
		ImGui::End();

		if (brushNamePopupIsOpen) BrushNamePopup(scene);
		if (newBrushPopupIsOpen) NewBrushPopup(scene);
	}
}

void CreateNewBrush(Scene & scene)
{
	if (scene.brushes.size() == 20) {
		std::cout << "ERROR! Maximum number of brushes reached." << std::endl;
	}
	else {
		newBrushPopupIsOpen = true;
	}
}

inline void NewBrushPopup(Scene & scene)
{
	ImGui::SetNextWindowSize(ImVec2(300, 80));
	ImGui::OpenPopup("New Brush");
	if (ImGui::BeginPopupModal("New Brush", &newBrushPopupIsOpen)) {
		std::string folderPath = SAVED_FILES_DIRECTORY;

		for (auto p : fs::directory_iterator(folderPath)) {
			std::string fileName = p.path().string().substr(folderPath.size() + 1); //Remove folder name

			if (!fs::is_directory(p) && fileName.substr(fileName.size() - 4) == ".png") {
				if (ImGui::Button(fileName.data(), ImVec2(200, 20))) {
					
					std::string brushPath = folderPath;
					brushPath.append("/");
					brushPath.append(fileName);
					scene.brushes.push_back(std::make_unique<Brush>(brushPath.data()));
					scene.brushes.back()->name = "NewBrush";
					newBrushPopupIsOpen = false;
				}
			}
		}

		ImGui::Dummy(ImVec2(0, 30));
		ImGui::SameLine(ImGui::GetWindowSize().x - 60);
		if (ImGui::Button("Close", ImVec2(50, 20))) newBrushPopupIsOpen = false;
		ImGui::EndPopup();
	}
}

void BrushNamePopup(Scene & scene)
{
	ImGui::SetNextWindowSize(ImVec2(300, 80));
	ImGui::OpenPopup("Chose Brush Name");
	if (ImGui::BeginPopupModal("Chose Brush Name", &brushNamePopupIsOpen)) {

		if (ImGui::InputText("Press Enter", brushName, sizeof(brushName), ImGuiInputTextFlags_EnterReturnsTrue)) {
			(*scene.currentBrush)->name = std::string(brushName);
			brushNamePopupIsOpen = false;
		}
		ImGui::Dummy(ImVec2(0, 30));
		ImGui::SameLine(ImGui::GetWindowSize().x - 60);
		if (ImGui::Button("Close", ImVec2(50, 20))) brushNamePopupIsOpen = false;
		ImGui::EndPopup();
	}
}

void ColorPickers(Scene & scene)
{
	switch (DRAWING_STATE) {
	case(DrawingState::ALPHA): {
		if (ImGui::CollapsingHeader("Colour Picker")) {
			ImGui::PushItemWidth(235);
			if (ImGui::ColorPicker3("##ColorPicker", (*scene.currentBrush)->guiBrushColor, colorEditFlags)) {
				(*scene.currentBrush)->synchronizeBrushColor(); // Need to synchronize the color we get from the colorpicker with the brush color
			}
			ImGui::ColorButton("Current##CurrentColor", (**scene.currentBrush).getCurrentImColorRGB(), 0, ImVec2(30.0f, 20.0f));
			ImGui::SameLine();
			ImGui::ColorButton("Previous##PreviousColor", (**scene.currentBrush).getPreviousImColorRGB(), 0, ImVec2(30.0f, 20.0f));
		}
		break;
	}
	case(DrawingState::NORMAL): {
		if (ImGui::CollapsingHeader("Normal Picker")) {
			if (ImGui::ImageButton(normalTex, sf::Vector2f(210, 210), 1, sf::Color::Black, sf::Color::White)) {
				pickNormalValue = true;
			}
			ImGui::ColorButton("Current##CurrentNormal", (**scene.currentBrush).getCurrentImNormalRGB(), 0, ImVec2(30.0f, 20.0f));
		}
		break;
	}
	}
}

void BrushSettings(Scene & scene)
{
	ImGui::Dummy(ImVec2(19, 0));
	ImGui::SameLine();
	ImGui::SliderFloat("Spac", &(*scene.currentBrush)->stepSize, 0, 500);

	ImGui::Dummy(ImVec2(19, 0));
	ImGui::SameLine();
	ImGui::SliderInt("Opac", &(*scene.currentBrush)->opacity, 0, 255);

	ImGui::Checkbox("##FlowPress", &(*scene.currentBrush)->useFlowPress);
	ImGui::SameLine();
	if (ImGui::SliderInt("Flow", &(*scene.currentBrush)->maxFlow, 0, 255)) {
		(*scene.currentBrush)->minFlow = (*scene.currentBrush)->maxFlow * (*scene.currentBrush)->flowRatio;
	}
	if ((*scene.currentBrush)->useFlowPress) {
		ImGui::Dummy(ImVec2(19, 0));
		ImGui::SameLine();
		if (ImGui::SliderInt("Min##Flow", &(*scene.currentBrush)->minFlow, 0, (*scene.currentBrush)->maxFlow)) {
			(*scene.currentBrush)->flowRatio = float((*scene.currentBrush)->minFlow) / float((*scene.currentBrush)->maxFlow);
		}
	}

	ImGui::Checkbox("##SizePress", &(*scene.currentBrush)->useSizePress);
	ImGui::SameLine();
	if (ImGui::SliderFloat("Size", &(*scene.currentBrush)->maxSize, 0, 1)) {
		(*scene.currentBrush)->setSpriteSize();
		(*scene.currentBrush)->minSize = (*scene.currentBrush)->maxSize * (*scene.currentBrush)->sizeRatio;
	}
	if ((*scene.currentBrush)->useSizePress) {
		ImGui::Dummy(ImVec2(19, 0));
		ImGui::SameLine();
		if (ImGui::SliderFloat("Min##Size", &(*scene.currentBrush)->minSize, 0.0f, (*scene.currentBrush)->maxSize)) {
			(*scene.currentBrush)->sizeRatio = (*scene.currentBrush)->minSize / (*scene.currentBrush)->maxSize;
		}

	}
}

void DrawBrushList(Scene & scene)
{
	int brushNumber = 1;
	if (scene.brushes.begin() == scene.currentBrush) {
		ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
	}
	else {
		ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150, 150, 150));
	}
	ImGui::Image((*scene.brushes.begin())->sprite, sf::Vector2f(30, 30));

	ImGui::SameLine();
	//Button that sets the current brush to the current iterator
	if (ImGui::Button("Default Brush")) {
		scene.currentBrush = scene.brushes.begin();
	}
	brushNumber++;
	sf::Color borderColor;
	for (auto iter = std::prev(scene.brushes.end()); iter > scene.brushes.begin(); std::advance(iter, -1)) {

		//Image of the brush with white border if it is selected
		if (iter == scene.currentBrush) {
			borderColor = sf::Color::White;
		}
		else {
			borderColor = sf::Color(150, 150, 150);
		}
		ImGui::Image((*iter)->sprite, sf::Vector2f(30, 30), sf::Color::White, borderColor);

		ImGui::SameLine();
		std::string buttonName = (*iter)->name;
		buttonName.append("##");
		buttonName.append(std::to_string(brushNumber));

		//Button that sets the current brush to the current iterator
		if (ImGui::Button(buttonName.data())) {
			scene.currentBrush = iter;
			if (clickClock.getElapsedTime().asMilliseconds() < DOUBLE_CLICK_SPEED) {
				brushNamePopupIsOpen = true;
				strcpy_s(brushName, "");
			}
			clickClock.restart();
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
}

void ScatterSettings(Scene & scene)
{
	ImGui::Checkbox("Size##Checkbox", &(*scene.currentBrush)->useSScatter);
	if ((*scene.currentBrush)->useSScatter) {
		ImGui::SameLine(65);
		ImGui::SliderFloat("##S-Scatter", &(*scene.currentBrush)->scatterScale, 0, 1);
	}
	ImGui::Checkbox("Pos##Checkbox", &(*scene.currentBrush)->usePScatter);
	if ((*scene.currentBrush)->usePScatter) {
		ImGui::SameLine(65);
		ImGui::SliderFloat("##P-Scatter", &(*scene.currentBrush)->scatterPos, 0, 1000);
	}
	ImGui::Checkbox("Angle##Checkbox", &(*scene.currentBrush)->useAScatter);
	if ((*scene.currentBrush)->useAScatter) {
		ImGui::SameLine(65);
		ImGui::SliderFloat("##A-Scatter", &(*scene.currentBrush)->scatterAngle, 0, 180);
	}
}
