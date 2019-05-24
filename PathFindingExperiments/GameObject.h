#pragma once

#include "Constants.h"

class GameObject
{
public:
	sf::Vector2i pos;
	sf::Color color;

	void drawGameObject(sf::RenderWindow &window);

	//GameObject();
	GameObject(sf::Vector2i pos);
	//GameObject(sf::Vector2i pos, sf::Color color);
	~GameObject();

private:
	sf::Texture gameTexture;
	sf::Sprite gameSprite;
	sf::Uint8 gamePixels[PIXELS] = { 0 };

	void updatePixels();
	void setPixelColor(const int &pixelIndex);
};

