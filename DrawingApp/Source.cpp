#include <SFML/Graphics.hpp>
#include "wtypes.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib> 

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "json.hpp"
#include "Layer.h"
#include "Settings.h"

using json = nlohmann::json;

void createBrushWindow();
void brushGUI();
void brushWindowRendering();
void mainWindowDrawing();
void mainMenuGUI();
void layerGUI();
void brushWindowDrawing();
void mainWindowEventHandling();
void brushWindowEventHandling();
void getDesktopResolution(int& horizontal, int& vertical);

float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
	return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
}

static const float PI = 3.14159265358979323846f;

sf::Clock deltaClock;

//Brush initialisation
int BRUSH_WIDTH = 512;
float movedDistance = 0;
float brushSize = 0.3f;
float stepsize = 2.0f;
int alpha = 100;
static float col[3] = { 0.5f,0.0f,0.5f };

//Window initialisation
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 800;
sf::RenderWindow mainWindow;
sf::RenderWindow brushWindow;
sf::Event event;

// Layers on which we are temporarily drawing before we assign the drawing layers content to a layer
Layer brushLayer(BRUSH_WIDTH, BRUSH_WIDTH);
Layer drawingLayer(SCREEN_WIDTH, SCREEN_HEIGHT);

typedef std::unique_ptr<Layer> LayerPntr;
std::vector<LayerPntr> layers = {};
std::vector<LayerPntr>::iterator currentLayer; // iterator that points to the layer which gets currently updated

typedef std::unique_ptr<Brush> BrushPntr;
std::vector<BrushPntr> brushes = {};
std::vector<BrushPntr>::iterator currentBrush;

//Buffer for cursor positions
std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0) };

int main() {
	/*getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);

	drawingLayer.updateSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	currentLayer->updateSize(SCREEN_WIDTH, SCREEN_HEIGHT);*/

	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Titlebar);
	mainWindow.setMouseCursorVisible(false);
	mainWindow.setFramerateLimit(120);
	
	ImGui::SFML::Init(mainWindow);

	layers.reserve(21); // Reserve space for 20 Layers
	layers.push_back(std::make_unique<Layer>(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color::White)); // Background Layer
	layers.push_back(std::make_unique<Layer>(SCREEN_WIDTH, SCREEN_HEIGHT)); // One layer to draw on
	currentLayer = std::prev(layers.end());

	brushes.reserve(20);
	brushes.push_back(std::make_unique<Brush>(
		BRUSH_WIDTH, 
		"circle-xxl.png", 
		sf::Color(col[0] * 255, col[0] * 255, col[0] * 255, 255)
		));
	currentBrush = brushes.begin();
	(*currentBrush)->color = sf::Color((sf::Uint8)(col[0] * 255), (sf::Uint8)(col[1] * 255), (sf::Uint8)(col[2] * 255), 255);
	(*currentBrush)->setBrushSize(brushSize);

	while (mainWindow.isOpen())
	{
		mainWindow.clear(sf::Color(0, 0, 0, 0));
		for (auto iter = layers.begin(); iter < layers.end(); std::advance(iter, 1)) {
			if(iter <= currentLayer) mainWindow.draw((*iter)->sprite);
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

		for (auto iter = layers.begin(); iter < layers.end(); std::advance(iter, 1)) {
			if (iter > currentLayer) mainWindow.draw((*iter)->sprite);
		}

		ImGui::SFML::Render(mainWindow);
		mainWindow.display();

		brushWindowRendering();
	}
	return 0;
}

void mainWindowDrawing()
{
	if (mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (isMouseHeld()) {
			drawingLayer.drawLinearOnCanvas(movedDistance, currentBrush, cursorPositions, mainWindow);
			//mainLayer.drawCubicOnCanvas(movedDistance, currentbrush, cursorPositions);
			drawingLayer.sprite.setColor(sf::Color(255, 255, 255, (*currentBrush)->opacity));
			mainWindow.draw(drawingLayer.sprite);
			drawingLayer.sprite.setColor(sf::Color(255, 255, 255, 255));
		}
	}
}

void brushWindowDrawing()
{
	if (brushWindow.hasFocus()) {
		if (sf::Mouse::getPosition(brushWindow).x >= 0 && sf::Mouse::getPosition(brushWindow).y >= 0) {
			if (isMouseHeld()) {
				brushLayer.drawLinearOnCanvas(movedDistance, currentBrush, cursorPositions, brushWindow);
				brushLayer.drawLayer(brushWindow);
			}
		}
		/*else {
			mainWindow.requestFocus();
		}*/
	} else {
		if (sf::Mouse::getPosition(brushWindow).x >= 0 && sf::Mouse::getPosition(brushWindow).y >= 0) {
			brushWindow.requestFocus();
		}
	}
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
	ImGui::End();
}

void brushGUI()
{
	ImGui::SetNextWindowSize(ImVec2(250, SCREEN_HEIGHT - 250));
	ImGui::SetNextWindowPos(ImVec2(0, 50));
	bool windowFlag = true;
	ImGui::Begin("Brush Settings", &windowFlag, ImGuiWindowFlags_NoResize);

	if (ImGui::CollapsingHeader("Colour Picker")) {
		if (ImGui::ColorPicker3("Colour", col)) {
			(*currentBrush)->color.r = (sf::Uint8)(col[0] * 255);
			(*currentBrush)->color.g = (sf::Uint8)(col[1] * 255);
			(*currentBrush)->color.b = (sf::Uint8)(col[2] * 255);
		}
	}
	if (ImGui::CollapsingHeader("Settings")) {
		ImGui::SliderFloat("Spacing", &(*currentBrush)->stepsize, 0, 100);
		if (ImGui::SliderFloat("Size", &brushSize, 0, 1)) (*currentBrush)->setBrushSize(brushSize);
		ImGui::SliderInt("Opacity", &(*currentBrush)->opacity, 0, 255);
		ImGui::SliderInt("Flow", &(*currentBrush)->flow, 0, 255);
	}
	if (ImGui::CollapsingHeader("Scatter")) {
		ImGui::SliderFloat("S-Scatter", &(*currentBrush)->scaterScale, 0, 1);
		ImGui::SliderFloat("P-Scatter", &(*currentBrush)->scaterPos, 0, 2000);
		ImGui::SliderFloat("A-Scatter", &(*currentBrush)->scaterAngle, 0, 180);
	}

	//Button to create a new brush
	if (ImGui::SmallButton("New Brush")) {
		brushLayer.clearLayer();
		createBrushWindow();
	}

	// Draw list of all brushes
	int brushNumber = 1;
	if (brushes.begin() == currentBrush) {
		ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
	}
	else {
		ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150, 150, 150));
	}
	ImGui::Image((*brushes.begin())->sprite, sf::Vector2f(30, 30));

	ImGui::SameLine();
	std::string brushName = "Brush";
	brushName.append(std::to_string(brushNumber));

	//Button that sets the current brush to the current iterator
	if (ImGui::Button(brushName.data())) {
		currentBrush = brushes.begin();
	}
	brushNumber++;
	for (auto iter = std::prev(brushes.end()); iter > brushes.begin() ; std::advance(iter, -1)) {

		//Image of the brush with white border if it is selected
		if (iter == currentBrush) {
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
			currentBrush = iter;
		}

		//Delete button
		if (iter != brushes.begin()) {
			ImGui::SameLine();
			std::string delButton = "Del##";
			delButton.append(brushName); // If we don't append the brush name imgui is confused when we press the button

			if (ImGui::Button(delButton.data())) {
				auto iterDist = std::distance(brushes.begin(), currentBrush);
				currentBrush = brushes.begin();

				// This makes sure that we continue working on the brush we were before deleting
				bool isCurrentBrushBelowIter = brushes.begin() + iterDist < iter;
				iter = brushes.erase(iter);
				if (isCurrentBrushBelowIter) {
					std::advance(currentBrush, iterDist);
				}
				else {
					std::advance(currentBrush, iterDist - 1);
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
			if (layers.size() < 21) {
				layers.push_back(std::make_unique<Layer>(SCREEN_WIDTH, SCREEN_HEIGHT));
			}
			else {
				std::cout << "ERROR! Maxing number of Layers reached!!!" << std::endl;
			}
		}
		int layerNumber = layers.size();

		// Draw list of all the layers
		for (auto iter = std::prev(layers.end()); iter > layers.begin(); std::advance(iter, -1)) {
			
			//Image of the layer with white border if it is selected
			if (iter == currentLayer) {
				ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color::White);
			}
			else {
				ImGui::DrawRect(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(30, 30)), sf::Color(150,150,150));
			}
			ImGui::Image((*iter)->sprite, sf::Vector2f(30, 30));
			
			//Button that sets the current Layer to the current iterator
			ImGui::SameLine();
			std::string layerName;
			if (iter != layers.begin()) {
				layerName = "Layer";
				layerName.append(std::to_string(layerNumber - 1));
			}
			else {
				layerName = "Background";
			}
			if (ImGui::Button(layerName.data())) {
				if (iter != layers.begin()) currentLayer = iter;
			}

			//Delete button
			ImGui::SameLine();
			std::string delButton = "Del##";
			delButton.append(layerName); // If we don't append the layer name imgui is confused when we press the button
			if (ImGui::Button(delButton.data())) {
				auto iterDist = std::distance(layers.begin(), currentLayer);
				currentLayer = layers.begin();

				if (layers.size() == 2) { // Create a new Layer if There are none left
					layers.push_back(std::make_unique<Layer>(SCREEN_WIDTH, SCREEN_HEIGHT));
					iter = layers.erase(iter);
					currentLayer = std::prev(layers.end());
				}
				else { // This makes sure that we continue working on the layer we were before deleting
					bool isCurrentLayerBelowIter = layers.begin() + iterDist < iter;
					iter = layers.erase(iter);
					if (isCurrentLayerBelowIter) {
						std::advance(currentLayer, iterDist);
					}
					else {
						std::advance(currentLayer, iterDist - 1);
					}
				}
			}
			layerNumber--;
		}
	}
	ImGui::End();
}

void createBrushWindow()
{
	brushWindow.create(sf::VideoMode(BRUSH_WIDTH, BRUSH_WIDTH), "Define Brush", sf::Style::Titlebar);
	brushWindow.setFramerateLimit(120);
	brushWindow.clear(sf::Color::Black);
	brushWindow.setPosition(mainWindow.getPosition() +
		sf::Vector2i(SCREEN_WIDTH / 2 - BRUSH_WIDTH / 2, SCREEN_HEIGHT / 2 - BRUSH_WIDTH / 2));
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
		brushLayer.drawLayer(brushWindow);
		brushWindow.display();
	}
}

void mainWindowEventHandling()
{
	if (event.type == sf::Event::Closed)
		mainWindow.close();
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			drawingLayer.clearLayer();

			if (isAltHeld()) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Vector2i pos = sf::Mouse::getPosition(mainWindow);
					
					sf::Texture newTex;
					newTex.create(mainWindow.getSize().x, mainWindow.getSize().y);
					newTex.update(mainWindow);
					newTex.copyToImage();
					(*currentBrush)->color = newTex.copyToImage().getPixel(pos.x, pos.y);

					col[0] = (*currentBrush)->color.r / 255.0f;
					col[1] = (*currentBrush)->color.g / 255.0f;
					col[2] = (*currentBrush)->color.b / 255.0f;
				}
			} else {
				movedDistance = 0.0f;
				setMouseIsHeld();

				sf::Vector2i newPos = sf::Mouse::getPosition(mainWindow);
				cursorPositions[0] = newPos;
				cursorPositions[1] = newPos;
				cursorPositions[2] = newPos;
				cursorPositions[3] = newPos;

				drawingLayer.resetDrawFlag();
				//No need to draw the window here, it gets drawn because the lmb is being held later
			}
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				(*currentLayer)->updateLayer(drawingLayer, currentBrush);
			}
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q): {
			mainWindow.clear(sf::Color(255, 255, 255, 255));
			(*currentLayer)->updateLayer(mainWindow);
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
		}
	}
}

void brushWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.key.code == sf::Mouse::Left) {
			movedDistance = 0.0f;
			setMouseIsHeld();

			sf::Vector2i newPos = sf::Mouse::getPosition(brushWindow);
			cursorPositions[0] = newPos;
			cursorPositions[1] = newPos;
			cursorPositions[2] = newPos;
			cursorPositions[3] = newPos;

			drawingLayer.resetDrawFlag();
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
			sf::RenderTexture renderTex;
			renderTex.create(brushLayer.image.getSize().x, brushLayer.image.getSize().y);
			renderTex.clear(sf::Color(255, 255, 255, 0));

			renderTex.draw(brushLayer.sprite);
			renderTex.display();

			renderTex.getTexture().copyToImage().saveToFile("BrushWindow.png");

			brushes.push_back(std::make_unique<Brush>(
				BRUSH_WIDTH,
				"BrushWindow.png",
				sf::Color(col[0] * 255, col[0] * 255, col[0] * 255, 255)
				));

			brushWindow.close();
		}
	}
}

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}