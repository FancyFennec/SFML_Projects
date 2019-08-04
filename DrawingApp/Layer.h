#pragma once

#include <SFML/Graphics.hpp>

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

	void drawOnCanvas(float& movedDistance, float& stepsize, std::vector<sf::Vector2i>& cursorPositions);
	void drawShapeAt(sf::Vector2f& cursorPos, sf::Image& brush);

	~Layer();

private:
	sf::RenderWindow& window;

	float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
		return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
	}
};

inline void Layer::drawShapeAt(sf::Vector2f& cursorPos, sf::Image& brush)
{

	//TODO: make it such that we can select the brush size here
	//int rad = std::ceilf(radius);
	for (int i = 0; i < brush.getSize().x; i++) {
		for (int j = 0; j < brush.getSize().y; j++) {

			int xPos = cursorPos.x - brush.getSize().x / 2 + i;
			int yPos = cursorPos.y - brush.getSize().y / 2  + j;

			if (xPos >= 0 && yPos >= 0 && xPos < image.getSize().x && yPos < image.getSize().y) {
				if (image.getPixel(xPos, yPos).a < brush.getPixel(i, j).a) {
					image.setPixel(xPos, yPos, brush.getPixel(i, j));
				}
			}
		}
	}
}

inline void Layer::drawOnCanvas(float& movedDistance, float& stepsize, std::vector<sf::Vector2i>& cursorPositions)
{
	sf::Vector2i currentPos = sf::Mouse::getPosition(window);
	movedDistance += distance(cursorPositions[1], currentPos);
	cursorPositions[1] = currentPos;

	if (movedDistance > stepsize) {

		int steps = (int)std::floorf(movedDistance / stepsize);
		float offsetFactor = (steps * stepsize - movedDistance);

		sf::Vector2f direction = sf::Vector2f(cursorPositions[1] - cursorPositions[0]) / distance(cursorPositions[1], cursorPositions[0]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[0]);


#pragma omp parallel for
		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * stepsize * direction;
			drawShapeAt(drawingPos, image);
			tex.update(image);
			sprite.setTexture(tex);
		}
		circlePos += steps * stepsize * direction;

		cursorPositions[0] = sf::Vector2i(circlePos);
		cursorPositions[1] = currentPos;

		movedDistance -= stepsize * steps;
	}
}

Layer::~Layer()
{
}
