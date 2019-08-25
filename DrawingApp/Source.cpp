#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "json.hpp"
#include "Layer.h"
#include "Settings.h"
#include "Scene.h"

using json = nlohmann::json;

void mainMenuGUI();
void brushGUI();
void layerGUI();
void mainWindowEventHandling();
void brushWindowEventHandling();
void lmbPressed();
void mainWindowDrawing();
void brushWindowDrawing();
void createMainWindow();
void createBrushWindow();
void brushWindowRendering();
void getDesktopResolution(int& horizontal, int& vertical);

sf::Clock deltaClock;

//Window initialisation
sf::RenderWindow mainWindow;
sf::RenderWindow brushWindow;
sf::Event event;

MSG msg;
UINT32 pointerId;
POINTER_PEN_INFO penInfo;

// Layers on which we are temporarily drawing before we assign the drawing layers content to a layer
const int LAYER_WIDTH = 800;
const int LAYER_HEIGHT = 1200;

Scene scene(LAYER_WIDTH, LAYER_HEIGHT);

int main() {
	createMainWindow();
	ImGui::SFML::Init(mainWindow);

	while (mainWindow.isOpen())
	{
		sf::RenderStates state;
		state.transform.translate(sf::Vector2f((*scene.currentLayer)->offset));
		mainWindow.clear(sf::Color(0, 0, 0, 0));
		for (auto iter = scene.layers.begin(); iter < scene.layers.end(); std::advance(iter, 1)) {
			if(iter <= scene.currentLayer) mainWindow.draw((*iter)->sprite, state);
		}

		if (PeekMessageW(&msg, mainWindow.getSystemHandle(), 0, 0, PM_NOREMOVE)) {
			pointerId = GET_POINTERID_WPARAM(msg.wParam);
			if (GetPointerPenInfo(pointerId, &penInfo)) {
				(*scene.currentBrush)->pressure = penInfo.pressure / 1024.0f;
			}
		}
		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			mainWindowEventHandling();
		}

		ImGui::SFML::Update(mainWindow, deltaClock.restart());
		mainMenuGUI();
		brushGUI();
		layerGUI();

		mainWindowDrawing();
		brushWindowDrawing();

		for (auto iter = scene.layers.begin(); iter < scene.layers.end(); std::advance(iter, 1)) {
			if (iter > scene.currentLayer) mainWindow.draw((*iter)->sprite, state);
		}

		ImGui::SFML::Render(mainWindow);
		mainWindow.display();

		brushWindowRendering();
	}
	return 0;
}

void mainMenuGUI()
{
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, 50));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	bool windowFlag = true;
	ImGui::Begin("##Main Menu", &windowFlag, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::Dummy(ImVec2(0, 5));
	if (ImGui::Button("File")) {
	}
	ImGui::SameLine();
	if (ImGui::Button("Edit")) {
	}
	ImGui::SameLine(SCREEN_WIDTH - 50);
	if (ImGui::Button("Close")) {
		mainWindow.close();
	}
	ImGui::End();
}

void brushGUI()
{
	ImGui::SetNextWindowSize(ImVec2(250, SCREEN_HEIGHT - 250));
	ImGui::SetNextWindowPos(ImVec2(0, 50));
	bool windowFlag = true;

	ImGui::Begin("Brush Settings", &windowFlag, ImGuiWindowFlags_NoResize);
	if (ImGui::CollapsingHeader("Colour Picker")) {
		if (ImGui::ColorPicker3("Colour", scene.col)) {
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
		ImGui::SliderFloat("P-Scatter", &(*scene.currentBrush)->scaterPos, 0, 2000);
		ImGui::SliderFloat("A-Scatter", &(*scene.currentBrush)->scaterAngle, 0, 180);
	}

	//Button to create a new brush
	if (ImGui::SmallButton("New Brush")) {
		if (scene.brushes.size() == 20) {
			std::cout << "ERROR! Maximum number of brushes reached." << std::endl;
		}
		else {
			scene.brushLayer.clearLayer();
			createBrushWindow();
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
	for (auto iter = std::prev(scene.brushes.end()); iter > scene.brushes.begin() ; std::advance(iter, -1)) {

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

void layerGUI()
{
	ImGui::SetNextWindowSize(ImVec2(150, SCREEN_HEIGHT / 2));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 150, 50));
	bool windowFlag = true;
	ImGui::Begin("Layers", &windowFlag, ImGuiWindowFlags_NoResize);
	{
		//Button to create a new Layer
		if (ImGui::Button("New Layer")) {
			if (scene.layers.size() < 21) {
				scene.layers.push_back(std::make_unique<Layer>(LAYER_WIDTH, LAYER_HEIGHT));
			}
			else {
				std::cout << "ERROR! Maxing number of Layers reached!!!" << std::endl;
			}
		}
		int layerNumber = scene.layers.size();

		// Draw list of all the layers
		for (auto iter = std::prev(scene.layers.end()); iter > scene.layers.begin(); std::advance(iter, -1)) {
			
			//Image of the layer with white border if it is selected
			if (iter == scene.currentLayer) {
				ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
			}
			else {
				ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150,150,150));
			}
			ImGui::Image((*iter)->sprite, sf::Vector2f(30, 30));
			
			//Button that sets the current Layer to the current iterator
			ImGui::SameLine();
			std::string layerName;
			if (iter != scene.layers.begin()) {
				layerName = "Layer";
				layerName.append(std::to_string(layerNumber - 1));
			}
			else {
				layerName = "Background";
			}
			if (ImGui::Button(layerName.data())) {
				if (iter != scene.layers.begin()) scene.currentLayer = iter;
			}

			//Delete button
			ImGui::SameLine();
			std::string delButton = "Del##";
			delButton.append(layerName); // If we don't append the layer name imgui is confused when we press the button
			if (ImGui::Button(delButton.data())) {
				auto iterDist = std::distance(scene.layers.begin(), scene.currentLayer);
				scene.currentLayer = scene.layers.begin();

				if (scene.layers.size() == 2) { // Create a new Layer if There are none left
					scene.layers.push_back(std::make_unique<Layer>(LAYER_WIDTH, LAYER_HEIGHT));
					iter = scene.layers.erase(iter);
				}
				else { // This makes sure that we continue working on the layer we were before deleting
					bool isCurrentLayerBelowIter = scene.layers.begin() + iterDist < iter;
					iter = scene.layers.erase(iter);
					if (isCurrentLayerBelowIter) {
						std::advance(scene.currentLayer, iterDist);
					}
					else {
						std::advance(scene.currentLayer, iterDist - 1);
					}
				}
				if (scene.currentLayer == scene.layers.begin()) std::advance(scene.currentLayer, 1);
			}
			layerNumber--;
		}
	}
	ImGui::End();
}

void mainWindowEventHandling()
{
	if (event.type == sf::Event::Closed)
		mainWindow.close();
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			lmbPressed();
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				(*scene.currentLayer)->updateLayer(scene.drawingLayer, scene.currentBrush);
			}
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q): {
			mainWindow.clear(sf::Color(255, 255, 255, 255));
			(*scene.currentLayer)->updateLayer(mainWindow);
			break;
		}
		case(sf::Keyboard::LAlt) : {
			setAltIsHeld();
			break;
		}
		case(sf::Keyboard::LControl): {
			setCtrlIsHeld();
			break;
		}
		//TODO: Fix this... Since we now have layers this does not work anymore
		case(sf::Keyboard::Z): {
			if (isCtrlHeld()) {
			}
			break;
		}
		case(sf::Keyboard::Y): {
			if (isCtrlHeld()) {
			}
			break;
		}
		case(sf::Keyboard::Space): {
			setSpaceIsHeld();
			break;
		}
		case(sf::Keyboard::Up): {
			(*scene.currentLayer)->offset += sf::Vector2i(0, -10);
			break;
		}
		case(sf::Keyboard::Down): {
			(*scene.currentLayer)->offset += sf::Vector2i(0, 10);
			break;
		}
		case(sf::Keyboard::Right): {
			(*scene.currentLayer)->offset += sf::Vector2i(10, 0);
			break;
		}
		case(sf::Keyboard::Left): {
			(*scene.currentLayer)->offset += sf::Vector2i(-10, 0);
			break;
		}
		case(sf::Keyboard::Escape): {
			mainWindow.close();
			break;
		}
		}
	}
	if (event.type == sf::Event::KeyReleased) {
		switch (event.key.code) {
		case(sf::Keyboard::LAlt): {
			setAltNotHeld();
			break;
		}
		case(sf::Keyboard::LControl): {
			setCtrlNotHeld();
			break;
		}
		case(sf::Keyboard::Space): {
			setSpaceNotHeld();
			break;
		}
		}
	}
}

void lmbPressed()
{
	scene.drawingLayer.clearLayer();

	if (isAltHeld()) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			scene.pickColor(mainWindow);
		}
	} else {
		setMouseIsHeld();
		scene.resetCursorPositions(mainWindow);
		//No need to draw the window here, it gets drawn because the lmb is being held later
	}
}

void brushWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.key.code == sf::Mouse::Left) {
			setMouseIsHeld();
			scene.resetCursorPositions(brushWindow);
			//No need to draw the window here, it gets drawn because the lmb is held later
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.key.code == sf::Mouse::Left) {
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			scene.saveBrush();
			brushWindow.close();
		}
	}
}

void mainWindowDrawing()
{
	if (mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (isMouseHeld()) {
			scene.drawOnDrawingLayer(mainWindow);
		}
	}
}

void brushWindowDrawing()
{
	if (brushWindow.hasFocus()) {
		if (sf::Mouse::getPosition(brushWindow).x >= 0 && sf::Mouse::getPosition(brushWindow).y >= 0) {
			if (isMouseHeld()) {
				scene.drawOnBrushLayer(brushWindow);
			}
		}
		/*else {
			mainWindow.requestFocus();
		}*/
	}
	else {
		if (sf::Mouse::getPosition(brushWindow).x >= 0 && sf::Mouse::getPosition(brushWindow).y >= 0) {
			brushWindow.requestFocus();
		}
	}
}

void createMainWindow()
{
	/*getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);

	drawingLayer.updateSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	currentLayer->updateSize(SCREEN_WIDTH, SCREEN_HEIGHT);*/

	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Titlebar);
	mainWindow.setMouseCursorVisible(false);
	mainWindow.setFramerateLimit(120);
}

void createBrushWindow()
{
	brushWindow.create(sf::VideoMode(scene.brushWidth, scene.brushWidth), "Define Brush", sf::Style::Titlebar);
	brushWindow.setFramerateLimit(120);
	brushWindow.clear(sf::Color::Black);
	brushWindow.setPosition(mainWindow.getPosition() +
		sf::Vector2i(SCREEN_WIDTH / 2 - scene.brushWidth / 2, SCREEN_HEIGHT / 2 - scene.brushWidth / 2));
}

void brushWindowRendering()
{
	if (brushWindow.isOpen() && brushWindow.hasFocus()) {
		while (brushWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				brushWindow.close();
			brushWindowEventHandling();
		}
		brushWindow.clear(sf::Color::Black);
		scene.brushLayer.drawLayer(brushWindow);
		brushWindow.display();
	}
}

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}