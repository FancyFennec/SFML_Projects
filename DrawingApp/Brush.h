#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Brush
{
public:
	sf::Image image;
	sf::Color color = sf::Color::Black;

	float stepsize = 10.0f;
	int brushsize = 8;
	float opacity = 255.0f;

	void setBrushsize(int newsize) {
		if(8 < newsize && newsize < 2)
		image = images[newsize];
	}

	Brush(sf::Image image) : image(image) {
		images.push_back(image);
		createBrushes();
	};

	Brush(const char* filePath) {
		image.loadFromFile(filePath);
		createBrushes();
	}

	~Brush() {};

private:
	std::vector<sf::Image> images;

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