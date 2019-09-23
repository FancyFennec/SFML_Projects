#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"

namespace fs = std::experimental::filesystem;

sf::RenderWindow window;
sf::Event event;

int main() {

	window.create(sf::VideoMode(400, 400), "TestWindow");
	window.setFramerateLimit(60);


	const unsigned int width = 400;
	sf::Image normal;
	normal.create(width, width);
	sf::Color color;
	sf::Texture tex;
	tex.create(width, width);
	sf::Sprite sprite;

	for (int j = 0; j < normal.getSize().y; j++) {
		for (int i = 0; i < normal.getSize().x; i++) {
			float x = 2.0f * (i - width / 2.0f) / width;
			float y = 2.0f * (j - width / 2.0f) / width;

			if (x*x + y * y > 1.0f) {
				color = sf::Color(255, 255, 255, 0);
			}
			else {
				float z = 1.0f - sqrt(x*x + y * y);

				x = (x / 2.0f + 0.5f) * 255.0f;
				y = (y / 2.0f + 0.5f) * 255.0f;
				z = (z / 2.0f + 0.5f) * 255.0f;

				color = sf::Color(x, y, z, 255);
			}
			normal.setPixel(i, j, color);
		}
	}

	tex.loadFromImage(normal);
	sprite.setTexture(tex);

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
		}

		window.clear(sf::Color::Black);
		window.draw(sprite);
		window.display();

	}

}
