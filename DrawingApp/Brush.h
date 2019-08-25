#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <numeric>


class Brush
{
public:
	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;
	sf::Color color = sf::Color::Black;

	float stepsize = 33.0f;
	float brushsize = 0.3f;
	int opacity = 170;
	int flow = 70;
	float pressure = 1.0f;

	float scaterScale = 0.0f;
	float scaterPos = 0.0f;
	float scaterAngle = 0.0f;

	Brush(int brush_width, const char* filePath, sf::Color color) :
		color(color)
	{
		image.create(brush_width, brush_width);
		if (!image.loadFromFile(filePath)) {
			std::cout << "ERROR! unable to find path to file!" << std::endl;
		}
		initialize();
	}

	void setBrushSize(float brushSize);

	~Brush() {};

private:
	void initialize();
};

inline void Brush::initialize() {
	tex.create(image.getSize().x, image.getSize().y);
	tex.update(image);
	sprite.setTexture(tex);
	sprite.setScale(sf::Vector2f(brushsize, brushsize));
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

inline void Brush::setBrushSize(float brushSize) {
	this->brushsize = brushSize;
	sprite.setScale(sf::Vector2f(brushSize, brushSize));
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}
