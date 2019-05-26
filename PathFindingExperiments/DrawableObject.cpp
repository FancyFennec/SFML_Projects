#include "DrawableObject.h"

void DrawableObject::draw(sf::RenderWindow &window) {
	updatePixels();

	texture.update(pixels);
	sprite.setTexture(texture);
	window.draw(sprite);
}

void DrawableObject::updatePixels()
{
	for (int i = 0; i < PIXELS; i++) {
		pixels[i] = 0;
	}

	int tileIndex = (pos.y * TILES_WIDTH * 100 + pos.x * 10) * 4;

	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 10; i++) {
			int pixelIndex = tileIndex + (j * SCREEN_WIDTH + i) * 4;
			setPixelColor(pixelIndex);
		}
	}
}

void DrawableObject::setPixelColor(const int &pixelIndex) {
	pixels[pixelIndex] = color.r;
	pixels[pixelIndex + 1] = color.g;
	pixels[pixelIndex + 2] = color.b;
	pixels[pixelIndex + 3] = color.a;
}

DrawableObject::DrawableObject()
{
	this->pos = sf::Vector2i(0,0);
	this->pixels = new sf::Uint8[PIXELS]();
	this->color = sf::Color::Blue;

	texture = sf::Texture();
	texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sprite = sf::Sprite(texture);
}

DrawableObject::DrawableObject(sf::Vector2i pos)
{
	this->pos = pos;
	this->pixels = new sf::Uint8[PIXELS]();
	this->color = sf::Color::Blue;

	texture = sf::Texture();
	texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sprite = sf::Sprite(texture);
}

DrawableObject::~DrawableObject()
{
	//free(pixels);
}
