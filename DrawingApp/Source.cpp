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
void getDesktopResolution(int& horizontal, int& vertical);
float distance(const sf::Vector2i& lastPos, const sf::Vector2i& mousePos) {
	return (float)sqrt((lastPos.x - mousePos.x) * (lastPos.x - mousePos.x) + (lastPos.y - mousePos.y) * (lastPos.y - mousePos.y));
}

sf::Clock deltaClock;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
const float PI = 3.14159265358979323846f;

sf::RenderWindow window;
sf::Event event;

sf::Texture currentTexture;
std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;
sf::Sprite sprite;

sf::RenderStates state;

float radius = 20.f;
int stepsize = 40;
float deltaDist = 0;

sf::Vector2i lastPos;
sf::Vector2i oldPos;
sf::Vector2i mousePos;

bool mouseIsHeld = false;
bool ctrlIsPressed = false;

sf::CircleShape circle;

int main() {
	getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);
	//window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App");
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(120);
	ImGui::SFML::Init(window);

	currentTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	textureBuffer.reserve(10);
	textureBuffer.push_back(currentTexture);
	textureIter = textureBuffer.begin();
	sprite.setTexture(currentTexture);

	circle.setRadius(radius);
	circle.setOrigin(sf::Vector2f(radius, radius));
	circle.setOutlineColor(sf::Color::White);
	circle.setFillColor(sf::Color(255, 255, 255, 10));

	mousePos, lastPos, oldPos = sf::Mouse::getPosition();

	while (window.isOpen())
	{
		mousePos = sf::Mouse::getPosition(window);
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
			eventHandling();
		}

		window.clear(sf::Color(25, 25, 25, 255));
		sprite.setTexture(currentTexture);
		window.draw(sprite);

		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			if (mouseIsHeld) {
				deltaDist += distance(lastPos, mousePos);
				if (deltaDist > stepsize) {
					int circles = std::floorf(deltaDist / stepsize);
					deltaDist -= circles * stepsize;

					sf::Vector2f deltaVec = circles == 0 ? sf::Vector2f(0.0f, 0.0f) : sf::Vector2f(mousePos - oldPos) / (float)circles;
					sf::Vector2f circlePos = sf::Vector2f(oldPos);
					oldPos = mousePos;

					for (int i = 0; i < circles; i++) {
						circle.setPosition(circlePos);
						circlePos += deltaVec;
						window.draw(circle);
					}
				}
				currentTexture.update(window);
			}
		}

		circle.setPosition(sf::Vector2f(mousePos));
		window.draw(circle);

		ImGui::SFML::Update(window, deltaClock.restart());
		ImGui::SFML::Render(window);

		window.display();
		lastPos = mousePos;
	}


	return 0;
}



void eventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			deltaDist = 0.0f;
			lastPos, oldPos = mousePos;
			mouseIsHeld = true;
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
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
			currentTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
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
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
