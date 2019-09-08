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
unsigned int width = 256;

sf::Image img;
sf::Texture tex;
sf::Sprite sprite;

int step = 0;
bool calculateNewStep = true;
unsigned int fileCounter = 0;

float getAlpha(int x, int y, float p) { 
	return std::powf(1.0f - (float)(x * x + y * y) / (width * width / 4), p);
};

void calculatePixels(double p);

int main() {

	window.create(sf::VideoMode(width, width), "Drawing App", sf::Style::Titlebar);
	window.setMouseCursorVisible(false);
	window.setFramerateLimit(60);

	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Up) {
					calculateNewStep = true;
					step += 1;
				}
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Down) {
					calculateNewStep = true;
					step -= 1;
				}
			}
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::S) {
					std::string filename = "Brush";
					filename.append(std::to_string(fileCounter)).append(".png");
					img.saveToFile(filename);
					fileCounter++;
				}
			}
		}

		if (calculateNewStep) {
			img.create(width, width, sf::Color(255, 255, 255, 0));
			calculatePixels(pow(2, step));
			tex.create(width, width);
			tex.loadFromImage(img);
			sprite.setTexture(tex);

			calculateNewStep = false;
		}

		window.clear(clear);

		window.draw(sprite);

		window.display();
	}
	

	return 0;
}

void calculatePixels(double p)
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < width; j++) {
			int x = width / 2 - i;
			int y = width / 2 - j;

			if ((x * x + y * y) <= (width * width / 4)) {
				float alpha = getAlpha(x, y, p) * 255;
				img.setPixel(i, j, sf::Color(255, 255, 255, alpha));
			}
			else {
				img.setPixel(i, j, sf::Color(255, 255, 255, 0));
			}

		}
	}
}
