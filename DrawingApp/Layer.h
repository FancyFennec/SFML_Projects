#pragma once

#include "Brush.h"
#include "Settings.h"

typedef std::unique_ptr<Brush> BrushPntr;

class Layer
{
public:
	unsigned int width;
	unsigned int height;

	std::string layerName = "Layer";

	sf::Texture tex;
	sf::Sprite sprite;

	bool useOffset = true;
	static sf::Vector2i offset;

	static sf::RenderStates renderState;

	Layer(){}

	Layer(int width, int height) :
		width(width),
		height(height) {
		sf::Color color(255, 255, 255, 0);
		initialize(color);
	}

	Layer(int width, int height, sf::Color color) :
		width(width),
		height(height) {
		initialize(color);
	}

	void clearLayer();
	void blendlayers(Layer& newLayer, std::vector<BrushPntr>::iterator& brush);
	void drawLayerinWindow(sf::RenderWindow& window);
	void resetDrawFlag() { drawFlag = 0; };
	void drawLerpOnCanvas(float& movedDistance, std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>& cursorPositions, sf::RenderWindow& window);

	~Layer();

private:
	unsigned int drawFlag = 0;
	static sf::RenderTexture rTex;
	static sf::Shader fragShader;

	void initialize(sf::Color& color);
	float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2);
	float getSScatter(std::vector<BrushPntr>::iterator& brush);
	float getPScatter(std::vector<BrushPntr>::iterator& brush);
	float getAScatter(std::vector<BrushPntr>::iterator& brush);
	sf::RenderStates getRenderState(std::vector<BrushPntr>::iterator & brush, sf::Vector2f &drawingPos);
};

sf::RenderTexture Layer::rTex;
sf::Shader Layer::fragShader;
sf::RenderStates Layer::renderState(&fragShader);
sf::Vector2i Layer::offset = sf::Vector2i(0, 0);

inline void Layer::initialize(sf::Color& color) {
	sf::Image image;
	image.create(width, height, color);
	tex.create(width, height);
	tex.update(image);
	sprite.setTexture(tex);

	if (offset.x == 0 && offset.y == 0) { // Since the offset is static, we only need to set it o nthe first layer
		offset = sf::Vector2i(WINDOW_WIDTH / 2 - width / 2, WINDOW_HEIGHT / 2 - height / 2);
	}

	rTex.create(width, height);

	if (!fragShader.loadFromFile("fragment_shader.frag", sf::Shader::Fragment))
		std::cout << "Could not load shader" << std::endl;

	renderState.blendMode = sf::BlendNone;
}

inline void Layer::clearLayer() {
	sf::Image image;
	image.create(width, height, sf::Color(255, 255, 255, 0));
	tex.update(image);
	sprite.setTexture(tex);
}

inline void Layer::blendlayers(Layer& newLayer, std::vector<BrushPntr>::iterator& brush) {

	rTex.clear(sf::Color(255, 255, 255, 0));

	fragShader.setUniform("texture1", tex);
	fragShader.setUniform("texture2", newLayer.tex);
	fragShader.setUniform("alpha", (**brush).opacity / 255.0f);

	rTex.draw(sprite, renderState);
	rTex.display();

	tex = rTex.getTexture();
	sprite.setTexture(tex);
}
inline void Layer::drawLayerinWindow(sf::RenderWindow & window) {
	window.draw(sprite);
}

inline void Layer::drawLerpOnCanvas(float& movedDistance, std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>& cursorPositions, sf::RenderWindow& window)
{
	cursorPositions[3] = sf::Mouse::getPosition(window);
	movedDistance = distance(cursorPositions[2], cursorPositions[3]);
	float relativeStepSize = (**brush).computeRelativeStepSize();

	if (drawFlag == 0 || movedDistance > relativeStepSize) {

		sf::RenderTexture renderTex;
		renderTex.create(width, height);
		renderTex.clear(sf::Color((**brush).currentColor.r, (**brush).currentColor.g, (**brush).currentColor.b, 0));

		(**brush).setSpriteColor();

		if (drawFlag == 0) { //This is the case when we just clicked, here we just draw the brushstamp at the cursor position
			drawFlag = 1;

			sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);
			
			if (useOffset) {
				(**brush).sprite.setPosition(circlePos - sf::Vector2f(offset));
			}
			else {
				(**brush).sprite.setPosition(circlePos);
			}

			renderTex.draw((**brush).sprite, getRenderState(brush, circlePos));
		}
		else { //Here we need to make sure that consecutive brushstamps have a constant distance to each other
			int steps = (int)std::floorf(movedDistance / relativeStepSize);

			sf::Vector2f direction = sf::Vector2f(cursorPositions[3] - cursorPositions[2]) / distance(cursorPositions[3], cursorPositions[2]);
			sf::Vector2f circlePos = sf::Vector2f(cursorPositions[2]);

			renderTex.draw(sprite);

			if (useOffset) {
				(**brush).sprite.setPosition(circlePos - sf::Vector2f(offset));
			}
			else {
				(**brush).sprite.setPosition(circlePos);
			}

			for (int i = 0; i < steps; i++) {
				sf::Vector2f drawingPos = circlePos + (i + 1) * relativeStepSize * direction;

				if (useOffset) {
					(**brush).sprite.setPosition(drawingPos - sf::Vector2f(offset));
				}
				else {
					(**brush).sprite.setPosition(drawingPos);
				}
				renderTex.draw((**brush).sprite, getRenderState(brush, drawingPos));
			}

			circlePos += steps * relativeStepSize * direction;
			cursorPositions[2] = sf::Vector2i(circlePos);
			movedDistance -= relativeStepSize * steps;
		}

		renderTex.display();
		tex = renderTex.getTexture();
		sprite.setTexture(tex);

		(**brush).resetSpriteColor();
	}
}

Layer::~Layer()
{
}

inline float Layer::distance(const sf::Vector2i & vec1, const sf::Vector2i & vec2) {
	return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
}

sf::RenderStates Layer::getRenderState(std::vector<BrushPntr>::iterator & brush, sf::Vector2f &drawingPos)
{
	sf::RenderStates state;

	float scale = getSScatter(brush);
	state.transform.scale(scale, scale, drawingPos.x, drawingPos.y);
	state.transform.rotate(getAScatter(brush));
	state.transform.translate(getPScatter(brush), getPScatter(brush));

	return state;
}

inline float Layer::getSScatter(std::vector<BrushPntr>::iterator & brush)
{
	if ((**brush).useSScatter) {
		return (**brush).pressure * (1.0f + (**brush).scatterScale * (rand() % 20 - 10) / 10.0f);
	}
	else {
		return 1.0f;
	}
}

inline float Layer::getPScatter(std::vector<BrushPntr>::iterator & brush)
{
	if ((**brush).usePScatter) {
		return (**brush).brushSize * (**brush).scatterPos * (rand() % 20 - 10) / 10.0f;
	}
	else {
		return (**brush).brushSize;
	}
}

inline float Layer::getAScatter(std::vector<BrushPntr>::iterator & brush)
{
	if ((**brush).useAScatter) {
		return (**brush).scatterAngle * (rand() % 20 - 10) / 10.0f;
	}
	else {
		return 0.0f;
	}
}

