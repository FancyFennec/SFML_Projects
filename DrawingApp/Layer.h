#pragma once

#include "Brush.h"
#include "Settings.h"

typedef std::unique_ptr<Brush> BrushPntr;

class Layer
{
public:
	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;
	unsigned int width;
	unsigned int height;
	static sf::Vector2i offset;

	unsigned static int layerCount;

	Layer(int width, int height) :
		width(width),
		height(height) {
		image.create(width, height, sf::Color(255, 255, 255, 0));
		tex.create(width, height);
		tex.update(image);
		sprite.setTexture(tex);
		initialize();
		layerCount++;
	};

	Layer(int width, int height, sf::Color color) :
		width(width),
		height(height) {
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

	void updateLayer(Layer& newLayer, std::vector<BrushPntr>::iterator& brush) {
		
		rTex.clear(sf::Color(255, 255, 255, 0));

		fragShader.setUniform("texture1", tex);
		fragShader.setUniform("texture2", newLayer.tex);
		fragShader.setUniform("alpha", (*brush)->opacity / 255.0f);

		rTex.draw(sprite, shader);
		rTex.display();

		tex = rTex.getTexture();
		sprite.setTexture(tex);
	}

	void drawLayer(sf::RenderWindow& window) {
		window.draw(sprite);
	}

	void resetDrawFlag() { drawFlag = 0; };

	void drawLinearOnCanvas(float& movedDistance, std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>& cursorPositions, sf::RenderWindow& window);
	//void drawCubicOnCanvas(float& movedDistance, Brush& brush, std::vector<sf::Vector2i>& cursorPositions);

	~Layer();

private:
	unsigned int drawFlag = 0;
	static sf::RenderTexture rTex;
	static sf::Shader fragShader;
	static sf::RenderStates shader;

	void initialize() {
		std::cout << offset.x << std::endl; 
		std::cout << offset.y << std::endl;
		if (offset.x == 0 && offset.y ==0) {
			offset = sf::Vector2i(SCREEN_WIDTH / 2 - width / 2, SCREEN_HEIGHT / 2 - height / 2);
		}

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

	sf::RenderStates getRenderState(std::vector<BrushPntr>::iterator & brush, sf::Vector2f &drawingPos);
};

unsigned int Layer::layerCount = 0;
sf::RenderTexture Layer::rTex;
sf::Shader Layer::fragShader;
sf::RenderStates Layer::shader(&fragShader);
sf::Vector2i Layer::offset = sf::Vector2i(0, 0);


inline void Layer::drawLinearOnCanvas(float& movedDistance, std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>& cursorPositions, sf::RenderWindow& window)
{
	cursorPositions[3] = sf::Mouse::getPosition(window) - offset;
	movedDistance = distance(cursorPositions[2], cursorPositions[3]);

	if (drawFlag == 0) {
		drawFlag = 1;

		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);

		sf::RenderTexture renderTex;
		renderTex.create(image.getSize().x, image.getSize().y);
		renderTex.clear(sf::Color((*brush)->color.r, (*brush)->color.g, (*brush)->color.b, 0));

		(*brush)->sprite.setColor(sf::Color(
			(*brush)->color.r,
			(*brush)->color.g,
			(*brush)->color.b,
			(*brush)->pressure * (*brush)->flow));
		(*brush)->sprite.setPosition(circlePos);

		renderTex.draw((*brush)->sprite, getRenderState(brush, circlePos));

		//renderTex.draw((*brush)->sprite);
		renderTex.display();

		tex = renderTex.getTexture();
		sprite.setTexture(tex);

		(*brush)->sprite.setColor((*brush)->color);

	} else if (movedDistance > (*brush)->stepsize * (*brush)->brushsize * (*brush)->pressure) {

		int steps = (int)std::floorf(movedDistance / ((*brush)->stepsize * (*brush)->brushsize * (*brush)->pressure));

		sf::Vector2f direction = sf::Vector2f(cursorPositions[3] - cursorPositions[2]) / distance(cursorPositions[3], cursorPositions[2]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);

		sf::RenderTexture renderTex;
		renderTex.create(image.getSize().x, image.getSize().y);
		renderTex.clear(sf::Color((*brush)->color.r, (*brush)->color.g, (*brush)->color.b, 0));
		renderTex.draw(sprite);

		(*brush)->sprite.setColor(sf::Color(
			(*brush)->color.r,
			(*brush)->color.g,
			(*brush)->color.b,
			(*brush)->pressure * (*brush)->flow));
		(*brush)->sprite.setPosition(circlePos);

		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * (*brush)->stepsize * (*brush)->brushsize * (*brush)->pressure * direction;
			(*brush)->sprite.setPosition(drawingPos);

			renderTex.draw((*brush)->sprite, getRenderState(brush, drawingPos));
		}

		circlePos += steps * (*brush)->stepsize * (*brush)->brushsize * (*brush)->pressure * direction;
		cursorPositions[2] = sf::Vector2i(circlePos);
		movedDistance -= (*brush)->stepsize * (*brush)->brushsize * (*brush)->pressure * steps;
		
		renderTex.display();
		tex = renderTex.getTexture();
		sprite.setTexture(tex);

		(*brush)->sprite.setColor((*brush)->color);
	}
}

sf::RenderStates Layer::getRenderState(std::vector<BrushPntr>::iterator & brush, sf::Vector2f &drawingPos)
{
	sf::RenderStates state;

	float scale = (*brush)->pressure * (1.0f + (*brush)->scaterScale * (rand() % 20 - 10) / 10.0f);
	state.transform.scale(
		scale, scale, drawingPos.x, drawingPos.y);
	state.transform.rotate((*brush)->scaterAngle * (rand() % 20 - 10) / 10.0f, drawingPos);
	state.transform.translate(
		(*brush)->brushsize * (*brush)->scaterPos * (rand() % 20 - 10) / 10.0f,
		(*brush)->brushsize * (*brush)->scaterPos * (rand() % 20 - 10) / 10.0f);

	return state;
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

