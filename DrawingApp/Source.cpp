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
void layerGUI();
void initialiseTitle();
void brushWindowDrawing();
void mainWindowEventHandling();
void brushWindowEventHandling();
void getDesktopResolution(int& horizontal, int& vertical);

float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
	return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
}

static const float PI = 3.14159265358979323846f;

sf::Clock deltaClock;
sf::Text title;
sf::Font font;

//Brush initialisation
int BRUSH_WIDTH = 512;
float movedDistance = 0;
int brushSize = 4;
float stepsize = 2.0f;
int alpha = 100;
static float col[3] = { 0.5f,0.0f,0.5f };
Brush currentbrush(BRUSH_WIDTH, "newBrush2.png", sf::Color(col[0] * 255, col[0] * 255, col[0] * 255, 255));

//Window initialisation
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 800;
sf::RenderWindow mainWindow;
sf::RenderWindow brushWindow;
sf::Event event;

Layer brushLayer(BRUSH_WIDTH, BRUSH_WIDTH, brushWindow);
Layer drawingLayer(SCREEN_WIDTH, SCREEN_HEIGHT, mainWindow);

std::vector<Layer> layers = { 
	Layer(SCREEN_WIDTH, SCREEN_HEIGHT, mainWindow, sf::Color::White)
};
std::vector<Layer>::iterator currentLayer = layers.begin();

std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;

sf::RenderStates state;

//Buffer for cursor positions
std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0) };

int main() {
	/*getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);

	drawingLayer.updateSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	currentLayer->updateSize(SCREEN_WIDTH, SCREEN_HEIGHT);*/

	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App");
	mainWindow.setMouseCursorVisible(false);
	mainWindow.setFramerateLimit(120);
	mainWindow.clear(sf::Color::White);
	
	if (!font.loadFromFile("Arial.ttf")) {
		std::cout << "Could not find font." << std::endl;
		return -1;
	}
	else {
		initialiseTitle();
	}

	ImGui::SFML::Init(mainWindow);

	currentbrush.color = sf::Color((sf::Uint8)(col[0] * 255), (sf::Uint8)(col[1] * 255), (sf::Uint8)(col[2] * 255), 255);
	currentbrush.setBrushsize(brushSize);

	textureBuffer.reserve(11);
	textureBuffer.push_back(currentLayer->tex);
	textureIter = textureBuffer.begin();

	while (mainWindow.isOpen())
	{
		for (auto layer : layers) {
			layer.drawLayer();
		}

		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			mainWindowEventHandling();
		}

		brushGUI();
		layerGUI();

		mainWindowDrawing();
		brushWindowDrawing();

		mainWindow.draw(title);
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
			drawingLayer.drawLinearOnCanvas(movedDistance, currentbrush, cursorPositions);
			//mainLayer.drawCubicOnCanvas(movedDistance, currentbrush, cursorPositions);
			drawingLayer.sprite.setColor(sf::Color(255, 255, 255, currentbrush.opacity));
			drawingLayer.drawLayer();
			drawingLayer.sprite.setColor(sf::Color(255, 255, 255, 255));
		}
	}
}

void brushWindowDrawing()
{
	if (brushWindow.hasFocus()) {
		if (isMouseHeld()) {
			brushLayer.drawLinearOnCanvas(movedDistance, currentbrush, cursorPositions);
			brushLayer.drawLayer();
		}
	}
}

void brushGUI()
{
	ImGui::SFML::Update(mainWindow, deltaClock.restart());

	ImGui::Begin("Brush Settings");
	if (ImGui::ColorPicker3("Brush Colour", col)) {
		currentbrush.color.r = (sf::Uint8)(col[0] * 255);
		currentbrush.color.g = (sf::Uint8)(col[1] * 255);
		currentbrush.color.b = (sf::Uint8)(col[2] * 255);
	}
	ImGui::SliderFloat("Spacing", &currentbrush.stepsize, 0, 50);
	if (ImGui::SliderInt("Radius", &brushSize, 0, 8)) {
		currentbrush.setBrushsize(brushSize);
	}
	if (ImGui::SliderInt("Opacity", &currentbrush.opacity, 0, 255)) {
	}
	if (ImGui::SmallButton("New Brush")) {
		createBrushWindow();
	}
	ImGui::End();
}

void layerGUI()
{
	ImGui::Begin("Layers");
	{
		if (ImGui::Button("New Layer")) {
			layers.push_back(Layer(SCREEN_WIDTH, SCREEN_HEIGHT, mainWindow));
			currentLayer = layers.end() - 1;
		}
		int layerNumber = layers.size();
		for (auto iter = layers.end() - 1; iter >= layers.begin(); std::advance(iter, -1)) {
			ImGui::Image(iter->sprite, sf::Vector2f(30, 30));
			ImGui::SameLine();
			std::string layerName = "Layer";
			layerName.append(std::to_string(layerNumber));
			if (ImGui::Button(layerName.data())) {
				currentLayer = iter;
			}
			//TODO: Fix this
			/*ImGui::SameLine();
			if (ImGui::Button("Del")) {
			layers.erase(iter);
			}*/
			layerNumber--;
		}
	}
	ImGui::End();
}

void createBrushWindow()
{
	brushWindow.create(sf::VideoMode(BRUSH_WIDTH, BRUSH_WIDTH), "Define Brush", sf::Style::Titlebar);
	brushWindow.setFramerateLimit(120);
	brushWindow.clear(sf::Color::White);
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
		brushWindow.clear(sf::Color::White);
		brushLayer.drawLayer();
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
					sf::Uint8 alpha = currentbrush.color.a;
					sf::Vector2i pos = sf::Mouse::getPosition(mainWindow);

					currentbrush.color = currentLayer->tex.copyToImage().getPixel(pos.x, pos.y);

					col[0] = currentbrush.color.r;
					col[1] = currentbrush.color.g;
					col[2] = currentbrush.color.b;
				}
			} else {
				if (textureIter != std::prev(textureBuffer.end())) {
					auto lastIter = std::prev(textureBuffer.end());

					while (textureIter != lastIter) {
						textureBuffer.erase(lastIter);
						std::advance(lastIter, -1);
					}
				}
				movedDistance = 0.0f;
				setMouseIsHeld();

				sf::Vector2i newPos = sf::Mouse::getPosition(mainWindow);
				cursorPositions[0] = newPos;
				cursorPositions[1] = newPos;
				cursorPositions[2] = newPos;
				cursorPositions[3] = newPos;

				drawingLayer.resetDrawFlag();
				//No need to draw the window here, it gets drawn because the lmb is held later
			}
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				drawingLayer.sprite.setColor(sf::Color(255, 255, 255, currentbrush.opacity));
				currentLayer->updateLayer(drawingLayer);
			}
			if (textureBuffer.size() >= 10) {
				textureBuffer.erase(textureBuffer.begin());
			}
			textureBuffer.push_back(currentLayer->tex);
			textureIter = textureBuffer.end() - 1;
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q): {
			mainWindow.clear(sf::Color(255, 255, 255, 255));
			currentLayer->updateLayer();
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
		case(sf::Keyboard::Z): {
			if (isCtrlHeld()) {
				if (textureIter != textureBuffer.begin()) std::advance(textureIter, -1);
				currentLayer->tex = *textureIter;
			}
			break;
		}
		case(sf::Keyboard::Y): {
			if (isCtrlHeld()) {
				if (textureIter != textureBuffer.end() - 1) std::advance(textureIter, 1);
				currentLayer->tex = *textureIter;
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
			//brushImage.saveToFile("newBrush.png");
			brushWindow.close();
		}
	}
}

void initialiseTitle()
{
	title.setFont(font);
	title.setString(sf::String("Baole Drawing App"));
	title.setFillColor(sf::Color::Black);
	title.setCharacterSize(24);
	sf::FloatRect rec = title.getGlobalBounds();
	title.setOrigin(rec.width / 2, rec.height / 2);
	title.setPosition(SCREEN_WIDTH / 2, 10);
}

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}