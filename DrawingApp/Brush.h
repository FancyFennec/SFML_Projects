#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Brush
{
public:
	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;
	sf::Color color = sf::Color::Black;

	float stepsize = 2.0f;
	int brushsize = 8;
	int opacity = 100;

	void setBrushsize(int newsize) {
		if (9 > newsize && newsize >= 0) {
			image = images[8 - newsize];
			tex.create(image.getSize().x, image.getSize().y);
			tex.update(image);
			sprite.setTexture(tex);
			sprite.setOrigin(sf::Vector2f(image.getSize().x / 2, image.getSize().y / 2));
			//sprite.setColor(brush.color);
		}
	}

	Brush(sf::Image image) : image(image) {
		images.push_back(image);
		createBrushes();
	};

	Brush(int brush_width) {
		for (int i = 0; i < brush_width; i++) {
			for (int j = 0; j < brush_width; j++) {
				float x = i - brush_width / 2;
				float y = j - brush_width / 2;

				int count = 0;

				for (float k = 0.0f; k < 4; k++) {
					for (float l = 0.0f; l < 4; l++) {
						if ((x + k / 4.0f + l / 4.0f) * (x + k / 4.0f + l / 4.0f) +
							(y + k / 4.0f + l / 4.0f) * (y + k / 4.0f + l / 4.0f) < 100 * 100) {
							count++;
						}
					}
				}

				sf::Color pixelColour = sf::Color::Black;
				pixelColour.a = sf::Uint8((255 * count) / 16);
				image.setPixel(i, j, pixelColour);
			}
		}
		images.push_back(image);
		createBrushes();
	}

	Brush(int brush_width, const char* filePath) {
		image.create(brush_width, brush_width);
		image.loadFromFile(filePath);
		images.push_back(image);
		createBrushes();
	}

	~Brush() {};

private:
	std::vector<sf::Image> images = {};

	void createBrushes();
};

inline void Brush::createBrushes() {

	for (int k = 0; k < 8; k++) {
		sf::Image newBrush;
		sf::Vector2u size = images.back().getSize();

		newBrush.create(size.x / 2, size.y / 2, sf::Color(0, 0, 0, 0));

		for (int j = 0; j < size.y / 2; j++) {
			for (int i = 0; i < size.x / 2; i++) {
				sf::Color newColour = images.back().getPixel(2 * i, 2 * j);
				newColour += images.back().getPixel(2 * i + 1, 2 * j);
				newColour += images.back().getPixel(2 * i, 2 * j + 1);
				newColour += images.back().getPixel(2 * i + 1, 2 * j + 1);

				newBrush.setPixel(i, j, newColour);
			}
		}
		images.push_back(newBrush);
	}
}