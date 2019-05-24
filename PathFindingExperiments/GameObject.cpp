#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.h"

//sf::Vector2i pos;
//sf::Color color;

//sf::Texture gameTexture;
//sf::Sprite gameSprite;

void GameObject::drawGameObject(sf::RenderWindow &window) {
	updatePixels();

	gameTexture.update(gamePixels);
	gameSprite.setTexture(gameTexture);
	window.draw(gameSprite);
}

void GameObject::updatePixels()
{
	for (int i = 0; i< PIXELS; i++) {
		gamePixels[i] = 0;
	}

	int xT = (int)pos.x / 10;
	int yT = (int)pos.y / 10;
	int tileIndex = (yT * TILES_WIDTH * 100 + xT * 10) * 4;

	for (int k = 0; k < 10; k++) {
		for (int l = 0; l < 10; l++) {
			int pixelIndex = tileIndex + (k * SCREEN_WIDTH + l) * 4;
			setPixelColor(pixelIndex);
		}
	}
}

void GameObject::setPixelColor(const int &pixelIndex) {
	gamePixels[pixelIndex] = color.r;
	gamePixels[pixelIndex + 1] = color.g;
	gamePixels[pixelIndex + 2] = color.b;
	gamePixels[pixelIndex + 3] = color.a;
}

//GameObject::GameObject()
//{
//	color = sf::Color::Blue;
//	gameTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
//	gameSprite = sf::Sprite(gameTexture);
//}

GameObject::GameObject(sf::Vector2i pos)
{
	this->pos = pos;
	/*this->color = sf::Color::Blue;
	gameTexture = sf::Texture();
	gameTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	gameSprite = sf::Sprite(gameTexture);*/
}

//GameObject::GameObject(sf::Vector2i pos, sf::Color color)
//{
//	this->pos = pos;
//	this->color = color;
//	gameTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
//	gameSprite = sf::Sprite(gameTexture);
//}


GameObject::~GameObject()
{
}
