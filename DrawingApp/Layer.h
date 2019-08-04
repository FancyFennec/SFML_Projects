#pragma once

#include <SFML/Graphics.hpp>
#include "Brush.h"

class Layer
{
public:

	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;

	Layer(sf::RenderWindow& window) :
		window(window) {};

	Layer(sf::Image image, sf::Texture tex, sf::Sprite sprite, sf::RenderWindow& window) :
		image(image),
		tex(tex),
		sprite(sprite),
		window(window) {
	};

	Layer(int width, int height, sf::RenderWindow& window) :
		window(window) {
		image.create(width, height, sf::Color(0, 0, 0, 0));
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
	};

	//TODO: Make a brush class that can get passed to the method
	void drawOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions);
	void drawShapeAt(Brush& brush, sf::Vector2f& cursorPos);

	~Layer();

private:
	sf::RenderWindow& window;

	float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
		return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
	}
};

inline void Layer::drawShapeAt(Brush& brush, sf::Vector2f& cursorPos)
{
	sf::Color newColour;

	for (int i = 0; i < brush.image.getSize().x; i++) {
		for (int j = 0; j < brush.image.getSize().y; j++) {

			int xPos = cursorPos.x - brush.image.getSize().x / 2 + i;
			int yPos = cursorPos.y - brush.image.getSize().y / 2  + j;

			if (xPos >= 0 && yPos >= 0 && xPos < image.getSize().x && yPos < image.getSize().y) {
				if (image.getPixel(xPos, yPos).a < brush.image.getPixel(i, j).a * brush.opacity / 255.0f) {
					
					newColour.r = brush.color.r * brush.image.getPixel(i, j).a * brush.opacity / 255.0f;
					newColour.g = brush.color.r * brush.image.getPixel(i, j).a * brush.opacity / 255.0f;
					newColour.b = brush.color.r * brush.image.getPixel(i, j).a * brush.opacity / 255.0f;
					newColour.a = brush.image.getPixel(i, j).a * brush.opacity / 255.0f;
					image.setPixel(xPos, yPos, newColour);
				}
			}
		}
	}
}

inline void Layer::drawOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions)
{
	sf::Vector2i currentPos = sf::Mouse::getPosition(window);
	movedDistance += distance(cursorPositions[1], currentPos);
	cursorPositions[1] = currentPos;

	if (movedDistance > brush.stepsize) {

		int steps = (int)std::floorf(movedDistance / brush.stepsize);
		float offsetFactor = (steps * brush.stepsize - movedDistance);

		sf::Vector2f direction = sf::Vector2f(cursorPositions[1] - cursorPositions[0]) / distance(cursorPositions[1], cursorPositions[0]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[0]);


#pragma omp parallel for
		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * brush.stepsize * direction;
			drawShapeAt(brush, drawingPos);
			tex.update(image);
			sprite.setTexture(tex);
		}
		circlePos += steps * brush.stepsize * direction;

		cursorPositions[0] = sf::Vector2i(circlePos);
		cursorPositions[1] = currentPos;

		movedDistance -= brush.stepsize * steps;
	}
}

Layer::~Layer()
{
}
