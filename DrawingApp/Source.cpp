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
float distance(const sf::Vector2i& lastPos, const sf::Vector2i& mousePos) {
	return sqrtf(powf((lastPos.x - mousePos.x), 2.0f) + powf(lastPos.y - mousePos.y, 2.0f));
}
void updateCanvas(sf::Vector2f &circlePos);

sf::Clock deltaClock;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
const float PI = 3.14159265358979323846f;

sf::RenderWindow window;
sf::Event event;

sf::Color brushColour(25, 25, 25, 15);

sf::Image canvasImage;
sf::Texture canvasTex;
sf::Sprite canvasSprite;

sf::Texture currentTexture;
std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;
sf::Sprite sprite;

sf::RenderStates state;

float radius = 20.f;
int stepsize = 10;
float deltaDist = 0;

std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0) };

bool mouseIsHeld = false;
bool ctrlIsPressed = false;

int main() {
	getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	SCREEN_WIDTH = 800; 
	SCREEN_HEIGHT = 800;
	//window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);
	window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App");
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
		cursorPositions.erase(cursorPositions.begin());
		cursorPositions.push_back(sf::Mouse::getPosition(window));

		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
			eventHandling();
		}

		sprite.setTexture(currentTexture);
		window.draw(sprite);

		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			if (mouseIsHeld) {
				drawOnCanvas();
				window.draw(canvasSprite);
			}
		}

		if (event.type == sf::Event::MouseButtonReleased) {
			currentTexture.update(window);
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		ImGui::SFML::Render(window);

		window.display();
	}
	return 0;
}

void drawOnCanvas()
{
	deltaDist += distance(cursorPositions[2], cursorPositions[1]);
	if (deltaDist > stepsize) {
		int circles = std::floorf(deltaDist / stepsize);

		if (circles == 1) {
			sf::Vector2f circlePos = sf::Vector2f(cursorPositions[1]);
			updateCanvas(circlePos);
		}
		else {
			sf::Vector2f control1 = sf::Vector2f(cursorPositions[1])
				+ sf::Vector2f(cursorPositions[1] - cursorPositions[0]) / 3.0f;
			sf::Vector2f control2 = sf::Vector2f(cursorPositions[2])
				+ sf::Vector2f(cursorPositions[2] - cursorPositions[3]) / 3.0f;

			for (int i = 0; i < circles; i++) {
				sf::Vector2f circlePos(0, 0);
				float x = (float)i / circles;

				circlePos += powf(1 - x, 3)*sf::Vector2f(cursorPositions[1]);
				circlePos += 3.0f * powf(1 - x, 2)*x*control1;
				circlePos += 3.0f * (1 - x)*x*x*control2;
				circlePos += x * x*x*sf::Vector2f(cursorPositions[2]);
				updateCanvas(circlePos);
			}
		}
		deltaDist -= stepsize * circles;
	}
}

void updateCanvas(sf::Vector2f &circlePos)
{
	int xPos;
	int yPos;

	for (int i = -std::ceilf(radius); i < std::ceilf(radius); i++) {
		for (int j = -std::ceilf(radius); j < std::ceilf(radius); j++) {

			xPos = circlePos.x + i;
			yPos = circlePos.y + j;

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
			deltaDist = 0.0f;
			mouseIsHeld = true;
			canvasImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color(0, 0, 0, 0));
			canvasTex.update(canvasImage);
			canvasSprite.setTexture(canvasTex);

			sf::Vector2f circlePos = sf::Vector2f(cursorPositions.back());
			updateCanvas(circlePos);
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		currentTexture.update(window);

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