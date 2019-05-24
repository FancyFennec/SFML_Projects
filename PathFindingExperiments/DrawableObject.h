#pragma once

#include "Constants.h"

class DrawableObject
{
public:

	sf::Vector2i pos;
	sf::Color color;

	void draw(sf::RenderWindow &window);

	DrawableObject(sf::Vector2i pos);
	~DrawableObject();

private:
	DrawableObject();

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Uint8 *pixels;

	void updatePixels();
	void setPixelColor(const int &pixelIndex);
};
