#pragma once

#include "Brush.h"

class Layer
{
public:
	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;

	unsigned static int layerCount;

	Layer(sf::RenderWindow& window) :
		window(window) {
		layerCount++; 
	};

	Layer(sf::Image image, sf::Texture tex, sf::Sprite sprite, sf::RenderWindow& window) :
		image(image),
		tex(tex),
		sprite(sprite),
		window(window) {
		layerCount++;
	};

	Layer(int width, int height, sf::RenderWindow& window) :
		window(window) {
		image.create(width, height, sf::Color(0, 0, 0, 0));
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
		layerCount++;
	};

	Layer(int width, int height, sf::RenderWindow& window, sf::Color color) :
		window(window) {
		image.create(width, height, color);
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
		//layerCount++;
	};

	void updateSize(int width, int height) {
		sf::Color color = image.getPixel(1, 1);
		image = sf::Image();
		tex = sf::Texture();
		sprite = sf::Sprite();

		image.create(width, height, color);
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
	}

	void clearLayer() {
		image.create(image.getSize().x, image.getSize().y, sf::Color(0, 0, 0, 0));
		tex.update(image);
		sprite.setTexture(tex);
	}

	void updateLayer() {
		tex.update(window);
	}

	void updateLayer(Layer& newLayer) {
		sf::RenderTexture rTex;
		rTex.create(image.getSize().x, image.getSize().y);
		rTex.draw(sprite);
		rTex.display();
		rTex.draw(newLayer.sprite);
		rTex.display();
		tex = rTex.getTexture();
	}

	void drawLayer() {
		window.draw(sprite);
	}

	void resetDrawFlag() { drawFlag = 0; };

	void drawLinearOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions);
	void drawCubicOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions);
	void drawBrushAt(Brush& brush, sf::Vector2f& cursorPos);

	~Layer();

private:
	unsigned int drawFlag = 0;
	sf::RenderWindow& window;

	std::function<sf::Vector2f (float)> getBezier(std::vector<sf::Vector2i>& cursorPositions);
	float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
		return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
	}
	float distance(const sf::Vector2f& vec1, const sf::Vector2f& vec2) {
		return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
	}

	float scalarProd(const sf::Vector2f& vec1, const sf::Vector2f& vec2) {
		return vec1.x * vec2.x + vec1.y + vec2.y;
	}
};

unsigned int Layer::layerCount = 0;

inline void Layer::drawLinearOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions)
{
	cursorPositions[3] = sf::Mouse::getPosition(window);
	movedDistance = distance(cursorPositions[2], cursorPositions[3]);

	if (movedDistance > brush.stepsize) {

		int steps = (int)std::floorf(movedDistance / brush.stepsize);

		sf::Vector2f direction = sf::Vector2f(cursorPositions[3] - cursorPositions[2]) / distance(cursorPositions[3], cursorPositions[2]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);

		sf::RenderTexture renderTex;
		renderTex.create(image.getSize().x, image.getSize().y);
		renderTex.draw(sprite);
		renderTex.display();

//#pragma omp parallel for
		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * brush.stepsize * direction;
			brush.sprite.setPosition(circlePos);
			renderTex.draw(brush.sprite);
			renderTex.display();
			//drawBrushAt(brush, drawingPos);
		}
		circlePos += steps * brush.stepsize * direction;
		cursorPositions[2] = sf::Vector2i(circlePos);
		movedDistance -= brush.stepsize * steps;

		
		tex = renderTex.getTexture();
		//tex.update(image);
		sprite.setTexture(tex);
	}
}

inline void Layer::drawCubicOnCanvas(float & movedDistance, Brush & brush, std::vector<sf::Vector2i>& cursorPositions)
{
	//TODO: This isn't working properly yet
	sf::Vector2i newPos = sf::Mouse::getPosition(window);
	movedDistance = distance(newPos, cursorPositions[3]);

	if (movedDistance > brush.stepsize) {
		if (drawFlag == 0) {
			drawFlag++;
			cursorPositions[1] = cursorPositions[3] - cursorPositions[2];
			cursorPositions[2] = cursorPositions[3];
			cursorPositions[3] = newPos;

			cursorPositions[1] += cursorPositions[3] - cursorPositions[2];
			cursorPositions[1] /= 3;
			
			drawLinearOnCanvas(movedDistance, brush, cursorPositions);
		}
		else {
			cursorPositions[0] = cursorPositions[1];
			cursorPositions[1] = cursorPositions[3] - cursorPositions[2];
			cursorPositions[2] = cursorPositions[3];
			cursorPositions[3] = newPos;

			cursorPositions[1] += cursorPositions[3] - cursorPositions[2];
			cursorPositions[1] /= 3;

			auto bezierFct = getBezier(cursorPositions);

			int steps = (int)std::floorf(movedDistance / brush.stepsize);

			sf::Vector2f oldPos = sf::Vector2f(cursorPositions[2]);
			sf::Vector2f drawingPos;

			float offset = 0;
			for (int i = 0; i < steps; i++) {
				float step = 1.0f / steps;

				//One step of linear interpolation to adjust the moved distance
				drawingPos = bezierFct(offset + step);
				float dist = distance(oldPos, drawingPos);
				step *= brush.stepsize / dist;
				drawingPos = bezierFct(offset + step);

				drawBrushAt(brush, drawingPos);

				offset += step;
				movedDistance -= distance(oldPos, drawingPos);
				oldPos = drawingPos;
			}

			cursorPositions[2] = sf::Vector2i(drawingPos);
		}
	}
}

inline void Layer::drawBrushAt(Brush& brush, sf::Vector2f& cursorPos)
{
	sf::Color newColour;

	for (int i = 0; i < brush.image.getSize().x; i++) {
		for (int j = 0; j < brush.image.getSize().y; j++) {

			int xPos = cursorPos.x - brush.image.getSize().x / 2 + i;
			int yPos = cursorPos.y - brush.image.getSize().y / 2 + j;

			if (xPos >= 0 && yPos >= 0 && xPos < image.getSize().x && yPos < image.getSize().y) {
				if (image.getPixel(xPos, yPos).a < brush.image.getPixel(i, j).a * brush.opacity / 255.0f) {

					newColour.r = brush.color.r;
					newColour.g = brush.color.g;
					newColour.b = brush.color.b;
					newColour.a = brush.opacity;
					image.setPixel(xPos, yPos, newColour);
				}
			}
		}
	}
}

Layer::~Layer()
{
}

inline std::function<sf::Vector2f(float)> Layer::getBezier(std::vector<sf::Vector2i>& cursorPositions)
{
	return [&](float x) {
		return (
			pow(1 - x, 3) * sf::Vector2f(cursorPositions[2]) +
			pow(x, 3) * sf::Vector2f(cursorPositions[3]) +
			3 * pow(1 - x, 2) * x * sf::Vector2f(cursorPositions[2] + cursorPositions[0]) +
			3 * (1 - x) * pow(x, 2) * sf::Vector2f(cursorPositions[3] - cursorPositions[1])
		);
	};
}

