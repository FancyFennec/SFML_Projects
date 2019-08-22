#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <numeric>
#include <iostream>


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

	void setBrushSize(float brushSize) {
		this->brushsize = brushSize;
		sprite.setScale(sf::Vector2f(brushSize, brushSize));
		sprite.setOrigin(sprite.getLocalBounds().width/2, sprite.getLocalBounds().height/2);
	}

	Brush(sf::Image image) : 
		image(image) {
		tex.create(image.getSize().x, image.getSize().y);
		tex.update(image);
		sprite.setTexture(tex);
		setBrushSize();
	}

	//This constructor is not needed right now and is mainly here as a backup
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

				sf::Color pixelColour = sf::Color::White;
				pixelColour.a = sf::Uint8((255 * count) / 16);
				image.setPixel(i, j, pixelColour);
			}
		}
		tex.create(image.getSize().x, image.getSize().y);
		tex.update(image);
		sprite.setTexture(tex);
		setBrushSize();
	}

	Brush(int brush_width, const char* filePath) {
		image.create(brush_width, brush_width);
		image.loadFromFile(filePath);
		tex.create(image.getSize().x, image.getSize().y);
		tex.update(image);
		sprite.setTexture(tex);
		setBrushSize();
	}

	Brush(int brush_width, const char* filePath, sf::Color color) :
		color(color)
	{
		image.create(brush_width, brush_width);
		image.loadFromFile(filePath);
		tex.create(image.getSize().x, image.getSize().y);
		tex.update(image);
		sprite.setTexture(tex);
		setBrushSize();
	}

	~Brush() {};

private:
	void setBrushSize() {
		sprite.setScale(sf::Vector2f(brushsize, brushsize));
		sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	}
};

