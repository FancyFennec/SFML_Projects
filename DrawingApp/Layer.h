#pragma once

#include "Brush.h"

class Layer
{
public:
	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;

	unsigned static int layerCount;

	Layer() {
		initialize();
		layerCount++; 
	};

	Layer(sf::Image image, sf::Texture tex, sf::Sprite sprite) :
		image(image),
		tex(tex),
		sprite(sprite)
	{
		initialize();
		layerCount++;
	};

	Layer(int width, int height) {
		image.create(width, height, sf::Color(255, 255, 255, 0));
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
		initialize();
		layerCount++;
	};

	Layer(int width, int height, sf::Color color) {
		image.create(width, height, color);
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
		initialize();
		layerCount++;
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
		image.create(image.getSize().x, image.getSize().y, sf::Color(255, 255, 255, 0));
		tex.update(image);
		sprite.setTexture(tex);
	}

	void updateLayer(sf::RenderWindow &window) {
		tex.update(window);
	}

	void updateLayer(Layer& newLayer) {
		sf::RenderTexture rTex;
		rTex.create(image.getSize().x, image.getSize().y);
		rTex.clear(sf::Color::Transparent);

		//TODO: This here doesn't work properly yet
		sf::Shader fragShader;

		if (!fragShader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment))
			std::cout << "Could not load shader" << std::endl;
		fragShader.setUniform("texture", sf::Shader::CurrentTexture);

		sf::RenderStates shader(&fragShader);

		rTex.draw(sprite, shader);
		rTex.draw(newLayer.sprite, shader);
		rTex.display();

		tex = rTex.getTexture();
		sprite.setTexture(tex);
	}

	void updateLayer(Layer& newLayer, Brush& brush) {
		
		rTex.clear(sf::Color(255, 255, 255, 0));

		fragShader.setUniform("texture1", tex);
		fragShader.setUniform("texture2", newLayer.tex);
		fragShader.setUniform("alpha", brush.opacity / 255.0f);

		rTex.draw(sprite, shader);
		rTex.display();

		tex = rTex.getTexture();
		sprite.setTexture(tex);
	}

	void drawLayer(sf::RenderWindow& window) {
		window.draw(sprite);
	}

	void resetDrawFlag() { drawFlag = 0; };

	void drawLinearOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions, sf::RenderWindow& window);
	//void drawCubicOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions);

	~Layer();

private:
	unsigned int drawFlag = 0;
	static sf::RenderTexture rTex;
	static sf::Shader fragShader;
	static sf::RenderStates shader;

	void initialize() {
		rTex.create(image.getSize().x, image.getSize().y);

		if (!fragShader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment))
			std::cout << "Could not load shader" << std::endl;

		shader.blendMode = sf::BlendNone;
	}

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
sf::RenderTexture Layer::rTex;
sf::Shader Layer::fragShader;
sf::RenderStates Layer::shader(&fragShader);

inline void Layer::drawLinearOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions, sf::RenderWindow& window)
{
	cursorPositions[3] = sf::Mouse::getPosition(window);
	movedDistance = distance(cursorPositions[2], cursorPositions[3]);

	if (drawFlag == 0) {
		drawFlag = 1;

		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);

		sf::RenderTexture renderTex;
		renderTex.create(image.getSize().x, image.getSize().y);
		renderTex.clear(sf::Color(brush.color.r, brush.color.g, brush.color.b, 0));

		brush.sprite.setColor(brush.color);
		brush.sprite.setPosition(circlePos);

		renderTex.draw(brush.sprite);
		renderTex.display();

		tex = renderTex.getTexture();
		sprite.setTexture(tex);

	} else if (movedDistance > brush.stepsize) {

		int steps = (int)std::floorf(movedDistance / brush.stepsize);

		sf::Vector2f direction = sf::Vector2f(cursorPositions[3] - cursorPositions[2]) / distance(cursorPositions[3], cursorPositions[2]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);

		sf::RenderTexture renderTex;
		renderTex.create(image.getSize().x, image.getSize().y);
		renderTex.clear(sf::Color(brush.color.r, brush.color.g, brush.color.b, 0));
		renderTex.draw(sprite);

		brush.sprite.setColor(brush.color);

		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * brush.stepsize * direction;
			brush.sprite.setPosition(drawingPos);
			renderTex.draw(brush.sprite);
		}

		circlePos += steps * brush.stepsize * direction;
		cursorPositions[2] = sf::Vector2i(circlePos);
		movedDistance -= brush.stepsize * steps;
		
		renderTex.display();
		tex = renderTex.getTexture();
		sprite.setTexture(tex);
	}
}

//inline void Layer::drawCubicOnCanvas(float & movedDistance, Brush & brush, std::vector<sf::Vector2i>& cursorPositions)
//{
//	//TODO: This isn't working properly yet
//	sf::Vector2i newPos = sf::Mouse::getPosition(window);
//	movedDistance = distance(newPos, cursorPositions[3]);
//
//	if (movedDistance > brush.stepsize) {
//		if (drawFlag == 0) {
//			drawFlag++;
//			cursorPositions[1] = cursorPositions[3] - cursorPositions[2];
//			cursorPositions[2] = cursorPositions[3];
//			cursorPositions[3] = newPos;
//
//			cursorPositions[1] += cursorPositions[3] - cursorPositions[2];
//			cursorPositions[1] /= 3;
//			
//			drawLinearOnCanvas(movedDistance, brush, cursorPositions);
//		}
//		else {
//			cursorPositions[0] = cursorPositions[1];
//			cursorPositions[1] = cursorPositions[3] - cursorPositions[2];
//			cursorPositions[2] = cursorPositions[3];
//			cursorPositions[3] = newPos;
//
//			cursorPositions[1] += cursorPositions[3] - cursorPositions[2];
//			cursorPositions[1] /= 3;
//
//			auto bezierFct = getBezier(cursorPositions);
//
//			int steps = (int)std::floorf(movedDistance / brush.stepsize);
//
//			sf::Vector2f oldPos = sf::Vector2f(cursorPositions[2]);
//			sf::Vector2f drawingPos;
//
//			float offset = 0;
//			for (int i = 0; i < steps; i++) {
//				float step = 1.0f / steps;
//
//				//One step of linear interpolation to adjust the moved distance
//				drawingPos = bezierFct(offset + step);
//				float dist = distance(oldPos, drawingPos);
//				step *= brush.stepsize / dist;
//				drawingPos = bezierFct(offset + step);
//
//				drawBrushAt(brush, drawingPos);
//
//				offset += step;
//				movedDistance -= distance(oldPos, drawingPos);
//				oldPos = drawingPos;
//			}
//
//			cursorPositions[2] = sf::Vector2i(drawingPos);
//		}
//	}
//}

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

