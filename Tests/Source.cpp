#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
namespace fs = std::experimental::filesystem;

sf::RenderWindow window;
sf::Event event;
sf::Color clear(0, 0, 0, 255);
unsigned int width = 512;
unsigned int radius = 16;

sf::CircleShape circle;
sf::Sprite sprite;
unsigned int steps = 32;
bool stepsizeChanged = true;

int main() {

	window.create(sf::VideoMode(width, width/2), "Test Window", sf::Style::Titlebar);
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(60);

	sf::Color col = sf::Color::Red;
	col.a = 30;
	circle.setFillColor(col);
	circle.setRadius(radius);
	circle.setOrigin(sf::Vector2f(radius / 2, radius / 2));

	sf::RenderTexture rTex;
	rTex.create(width, width/2);

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			eventHandling();
		}

		updatesprite(rTex);

		window.clear(clear);

		window.draw(sprite);

		window.display();
	}
	

	return 0;
}

void updatesprite(sf::RenderTexture &rTex)
{
	if (stepsizeChanged) {

		rTex.clear(sf::Color::White);

		for (int i = 0; i <= steps; i++) {
			float scalingFactor = ((float)i - steps / 2) / (steps / 2);

			circle.setScale(1 - abs(scalingFactor), 1 - abs(scalingFactor));
			circle.setPosition(
				sf::Vector2f(
					scalingFactor * 200 + width / 2,
					50.0f * sin(scalingFactor * 3.145) + width / 4
				)
			);
			rTex.draw(circle);
		}

		rTex.display();
		sprite.setTexture(rTex.getTexture());

		stepsizeChanged = false;
	}
}

void eventHandling()
{
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			window.close();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Up) {
			std::cout << "Size increased" << std::endl;
			steps *= 2;
			stepsizeChanged = true;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Down) {
			if (steps > 2) {
				std::cout << "Size reduced" << std::endl;
				steps /= 2;
				stepsizeChanged = true;
			}
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::S) {
		}
	}
}
