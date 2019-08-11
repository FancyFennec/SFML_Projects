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
void brushGUIRendering();
void brushWindowRendering();
void mainWindowDrawing();
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
Brush currentbrush(BRUSH_WIDTH, "newBrush.png");
float movedDistance = 0;
int brushSize = 4;
float stepsize = 2.0f;
int alpha = 100;
static float col[3] = { 0.5f,0.0f,0.5f };

//Window initialisation
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 800;
sf::RenderWindow mainWindow;
sf::RenderWindow brushWindow;
sf::Event event;

//Layers for drawing in the respective windows
Layer brushLayer(BRUSH_WIDTH, BRUSH_WIDTH, brushWindow);
Layer mainLayer(SCREEN_WIDTH, SCREEN_HEIGHT, mainWindow);


sf::Texture backgroundTexture;
std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;
sf::Sprite backgroundSprite;

sf::RenderStates state;

//Buffer for cursor positions
std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0) };

//int SETTINGS = 0;
//int MOUSE_IS_HELD = 1<<1;
//int CTRL_IS_HELD = 1<<2;
//int ALT_IS_HELD = 1<<3;

int main() {
	//getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	//window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);
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

	currentbrush.color = sf::Color((sf::Uint8)(col[0] * 255), (sf::Uint8)(col[1] * 255), (sf::Uint8)(col[2] * 255), alpha);
	currentbrush.setBrushsize(brushSize);

	backgroundTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	backgroundTexture.update(mainWindow);
	backgroundSprite.setTexture(backgroundTexture);

	textureBuffer.reserve(10);
	textureBuffer.push_back(backgroundTexture);
	textureIter = textureBuffer.begin();

	while (mainWindow.isOpen())
	{
		mainWindow.draw(backgroundSprite);

		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			mainWindowEventHandling();
		}

		brushWindowRendering();
		brushGUIRendering();

		mainWindowDrawing();
		brushWindowDrawing();

		if (event.type == sf::Event::MouseButtonReleased) {
			backgroundTexture.update(mainWindow);
		}

		mainWindow.draw(title);
		ImGui::SFML::Render(mainWindow);
		mainWindow.display();
	}
	return 0;
}

void mainWindowDrawing()
{
	if (mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (isMouseHeld()) {
			mainLayer.drawLinearOnCanvas(movedDistance, currentbrush, cursorPositions);
			//mainLayer.drawCubicOnCanvas(movedDistance, currentbrush, cursorPositions);
			mainWindow.draw(mainLayer.sprite);
		}
	}
}

void brushWindowDrawing()
{
	if (brushWindow.hasFocus()) {
		if (isMouseHeld()) {
			std::cout << "Drawing in Brush Window" << std::endl;
			brushLayer.drawLinearOnCanvas(movedDistance, currentbrush, cursorPositions);
			brushWindow.draw(brushLayer.sprite);
		}
	}
}

void brushGUIRendering()
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
		brushWindow.draw(brushLayer.sprite);
		brushWindow.display();
	}
}

void mainWindowEventHandling()
{
	if (event.type == sf::Event::Closed)
		mainWindow.close();
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			mainLayer.clearLayer();

			if (isAltHeld()) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Uint8 alpha = currentbrush.color.a;
					sf::Vector2i pos = sf::Mouse::getPosition(mainWindow);

					currentbrush.color = backgroundTexture.copyToImage().getPixel(pos.x, pos.y);
					currentbrush.color.a = alpha;

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
				sf::Vector2f currentPos = sf::Vector2f(newPos);

				mainLayer.drawBrushAt(currentbrush, currentPos);
				mainLayer.counter = 0;
				//No need to draw the window here, it gets drawn because the lmb is held later
			}
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				mainWindow.draw(backgroundSprite);
				mainWindow.draw(mainLayer.sprite);
				backgroundTexture.update(mainWindow);
			}
			if (textureBuffer.size() >= 10) {
				textureBuffer.erase(textureBuffer.begin());
			}
			textureBuffer.push_back(backgroundTexture);
			textureIter = textureBuffer.end() - 1;
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q): {
			mainWindow.clear(sf::Color(255, 255, 255, 255));
			backgroundTexture.update(mainWindow);
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
				backgroundTexture = *textureIter;
			}
			break;
		}
		case(sf::Keyboard::Y): {
			if (isCtrlHeld()) {
				if (textureIter != textureBuffer.end() - 1) std::advance(textureIter, 1);
				backgroundTexture = *textureIter;
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

			sf::Vector2f circlePos = sf::Vector2f(newPos);
			brushLayer.drawBrushAt(currentbrush, circlePos);
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