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
using json = nlohmann::json;


void eventHandling();
void drawOnCanvas();
void getDesktopResolution(int& horizontal, int& vertical);
float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
	return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
}
void drawShapeAt(sf::Vector2f &circlePos);

sf::Clock deltaClock;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
const float PI = 3.14159265358979323846f;

sf::RenderWindow window;
sf::Event event;

sf::Image canvasImage;
sf::Texture canvasTex;
sf::Sprite canvasSprite;

sf::Texture currentTexture;
std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;
sf::Sprite sprite;

sf::RenderStates state;

float radius = 10.f;
float stepsize = 20;
float movedDistance = 0;

std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0) };

bool mouseIsHeld = false;
bool ctrlIsPressed = false;

int alpha = 100;
float col[3] = { 0.5f,0.0f,0.5f };
sf::Color brushColour((sf::Uint8)(col[0] * 255), (sf::Uint8)(col[1] * 255), (sf::Uint8)(col[2] * 255), alpha);

int main() {
	getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);
	/*SCREEN_WIDTH = 800;
	SCREEN_HEIGHT = 800;
	window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App");*/
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(120);
	window.clear(sf::Color(255, 255, 255, 255));

	ImGui::SFML::Init(window);

	currentTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	currentTexture.update(window);
	sprite.setTexture(currentTexture);

	canvasImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color(0,0,0,0));
	canvasTex.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	canvasTex.update(canvasImage);
	canvasSprite.setTexture(canvasTex);

	textureBuffer.reserve(10);
	textureBuffer.push_back(currentTexture);
	textureIter = textureBuffer.begin();

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
			eventHandling();
		}

		sprite.setTexture(currentTexture);
		window.draw(sprite);

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Brush Settings");
		if (ImGui::ColorPicker3("Brush Colour", col)) {
			brushColour.r = (sf::Uint8)(col[0] * 255);
			brushColour.g = (sf::Uint8)(col[1] * 255);
			brushColour.b = (sf::Uint8)(col[2] * 255);
		}
		ImGui::SliderFloat("Spacing", &stepsize, 0, 50);
		ImGui::SliderFloat("Radius", &radius, 0, 50);
		if (ImGui::SliderInt("Opacity", &alpha, 0, 255)) {
			brushColour.a = alpha;
		}
		ImGui::End();

		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			if (mouseIsHeld) {
				drawOnCanvas();
				window.draw(canvasSprite);
			}
		}

		if (event.type == sf::Event::MouseButtonReleased) {
			currentTexture.update(window);
		}

		ImGui::SFML::Render(window);

		window.display();
	}
	return 0;
}

void drawOnCanvas()
{
	sf::Vector2i currentPos = sf::Mouse::getPosition(window);
	movedDistance += distance(cursorPositions[1], currentPos);
	cursorPositions[1] = currentPos;

	if (movedDistance > stepsize) {

		int steps = (int)std::floorf(movedDistance / stepsize);
		float offsetFactor = (steps * stepsize - movedDistance);

		sf::Vector2f direction = sf::Vector2f(cursorPositions[1] - cursorPositions[0]) / distance(cursorPositions[1], cursorPositions[0]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[0]);

		
#pragma omp parallel for
		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * stepsize * direction;
			drawShapeAt(drawingPos);
		}
		circlePos += steps * stepsize * direction;

		cursorPositions[0] = sf::Vector2i(circlePos);
		cursorPositions[1] = currentPos;
		
		movedDistance -= stepsize * steps;
	}
}

void drawShapeAt(sf::Vector2f &circlePos)
{
	int xPos;
	int yPos;

	int rad = std::ceilf(radius);
	for (int i = -rad; i < rad; i++) {
		for (int j = -rad; j < rad; j++) {

			xPos = (int)circlePos.x + i;
			yPos = (int)circlePos.y + j;

			if (xPos >= 0 && yPos >= 0 && xPos < SCREEN_WIDTH && yPos < SCREEN_HEIGHT) {
				canvasImage.setPixel(xPos, yPos, i*i + j * j > radius * radius ?
					canvasImage.getPixel(xPos, yPos) :
					canvasImage.getPixel(xPos, yPos).a >= brushColour.a ?
					canvasImage.getPixel(xPos, yPos) :
					brushColour
				);
			}
		}
	}
	canvasTex.update(canvasImage);
	canvasSprite.setTexture(canvasTex);
}

void eventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			movedDistance = 0.0f;
			mouseIsHeld = true;
			canvasImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color(0, 0, 0, 0));
			canvasTex.update(canvasImage);
			canvasSprite.setTexture(canvasTex);

			sf::Vector2i newPos = sf::Mouse::getPosition(window);
			cursorPositions[0] = newPos;
			cursorPositions[1] = newPos;

			sf::Vector2f circlePos = sf::Vector2f(newPos);
			drawShapeAt(circlePos);
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			currentTexture.update(window);
		}

		if (event.mouseButton.button == sf::Mouse::Left) {
			if (textureIter != textureBuffer.end() - 1) {
				auto iter = textureIter + 1;
				while (iter < textureBuffer.end()) {
					textureBuffer.erase(iter);
				}
			}
			if (textureBuffer.size() >= 10) {
				textureBuffer.erase(textureBuffer.begin());
			}
			textureBuffer.push_back(currentTexture);
			textureIter = textureBuffer.end() - 1;
			mouseIsHeld = false;
		}
	}
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Right) {
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Q) {
			window.clear(sf::Color(255, 255, 255, 255));
			currentTexture.update(window);
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::LControl) {
			ctrlIsPressed = true;
		}
	}
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::LControl) {
			ctrlIsPressed = false;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (ctrlIsPressed && event.key.code == sf::Keyboard::Z) {
			if (textureIter != textureBuffer.begin()) --textureIter;
			currentTexture = *textureIter;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (ctrlIsPressed && event.key.code == sf::Keyboard::Y) {
			if (textureIter != textureBuffer.end() - 1) ++textureIter;
			currentTexture = *textureIter;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			window.close();
		}
	}
}

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}