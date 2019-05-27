#pragma once

#include "Constants.h"

class DrawableObject
{
public:

	sf::Vector2i pos;
	sf::Color color;
	std::vector<sf::Uint8> pixels;

	void draw(sf::RenderWindow &window);

	DrawableObject();
	DrawableObject(sf::Vector2i pos);
	~DrawableObject();

private:
	sf::Texture texture;
	sf::Sprite sprite;

	void updatePixels();
	void setPixelColor(const int &pixelIndex);
};

