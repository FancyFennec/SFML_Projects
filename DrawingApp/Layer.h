#pragma once

#include "Brush.h"
#include "Settings.h"
#include "CursorBuffer.h"

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
	void drawLayerinWindow();
	void lerpDrawingOnLayer(std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>::iterator iter);
	~Layer();

private:
	static sf::RenderTexture rTex;
	static sf::Shader alphaBlendingShader;
	static sf::Shader normalBlendingShader;
	static sf::RenderStates alphaBlendingRState;
	static sf::RenderStates normalBlendingRState;

	void initialize(sf::Color& color);
	float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2);
	float getSScatter(std::vector<BrushPntr>::iterator& brush);
	float getPScatter(std::vector<BrushPntr>::iterator& brush);
	float getAScatter(std::vector<BrushPntr>::iterator& brush);
	sf::RenderStates getRenderState(std::vector<BrushPntr>::iterator & brush);
};

sf::RenderTexture Layer::rTex;
sf::Shader Layer::alphaBlendingShader;
sf::Shader Layer::normalBlendingShader;
sf::RenderStates Layer::alphaBlendingRState(&alphaBlendingShader);
sf::RenderStates Layer::normalBlendingRState(&normalBlendingShader);
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

	if (!alphaBlendingShader.loadFromFile(ALPHA_BLENDING_SHADER_PATH, sf::Shader::Fragment))
		std::cout << "Could not load ALphaBlendingShader" << std::endl;
	if (!normalBlendingShader.loadFromFile(NORMAL_BLENDING_SHADER_PATH, sf::Shader::Fragment))
		std::cout << "Could not load NormalBlendingShader" << std::endl;

	alphaBlendingRState.blendMode = sf::BlendNone;
}

inline void Layer::clearLayer() {
	sf::Image image;
	image.create(width, height, sf::Color(255, 255, 255, 0));
	tex.update(image);
	sprite.setTexture(tex);
}

inline void Layer::blendlayers(Layer& newLayer, std::vector<BrushPntr>::iterator& brush) {

	rTex.clear(sf::Color(255, 255, 255, 0));

	alphaBlendingShader.setUniform("texture1", sf::Shader::CurrentTexture);
	alphaBlendingShader.setUniform("texture2", newLayer.tex);
	alphaBlendingShader.setUniform("alpha", (**brush).opacity / 255.0f);

	rTex.draw(sprite, alphaBlendingRState);
	rTex.display();

	tex = rTex.getTexture();
	sprite.setTexture(tex);
}

inline void Layer::drawLayerinWindow() {
	mainWindow.draw(sprite);
}

inline void Layer::lerpDrawingOnLayer(std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>::iterator iter)
{
	sf::RenderTexture renderTex;
	renderTex.create(width, height);
	renderTex.clear(sf::Color((**brush).currentColor.r, (**brush).currentColor.g, (**brush).currentColor.b, 0));

	(**brush).setSpriteColor();
	sf::Vector2f offset2f;
	if (useOffset) {
		offset2f = sf::Vector2f(offset);
	}

	if (CursorBuffer::isFirstStamp) { //This is the case when we just clicked, here we just draw the brushstamp at the cursor position
		CursorBuffer::isFirstStamp = false;

		sf::Vector2f circlePos = sf::Vector2f(*iter);
		(**brush).sprite.setPosition(circlePos - offset2f);
		renderTex.draw((**brush).sprite, getRenderState(brush));
	}
	else { //Here we need to make sure that consecutive brushstamps have a constant distance to each other
		float movedDistance = distance(*iter, *(iter + 1));
		movedDistance = distance(*iter, *(iter + 1)) > 200.0f ? 0 : movedDistance; //TODO: Sometimes iter + 1 accesses something outside the vector, then the moved distance becomse a large number
		float relativeStepSize = (**brush).computeRelativeStepSize();
		int steps = (int)std::floorf(movedDistance / relativeStepSize);

		sf::Vector2f direction = sf::Vector2f((*std::next(iter) - *iter)) / movedDistance;
		sf::Vector2f circlePos = sf::Vector2f(*iter);
		renderTex.draw(sprite);

		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * relativeStepSize * direction;
			(**brush).sprite.setPosition(drawingPos - offset2f);
			renderTex.draw((**brush).sprite, getRenderState(brush));
		}

		circlePos += steps * relativeStepSize * direction;
		*std::next(iter) = sf::Vector2i(circlePos);
	}

	renderTex.display();
	tex = renderTex.getTexture();
	sprite.setTexture(tex);

	(**brush).resetSpriteColor();
}

Layer::~Layer()
{
}

inline float Layer::distance(const sf::Vector2i & vec1, const sf::Vector2i & vec2) {
	return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
}

sf::RenderStates Layer::getRenderState(std::vector<BrushPntr>::iterator & brush)
{
	sf::RenderStates state;
	float scale = getSScatter(brush);
	state.transform.scale(scale, scale, (**brush).sprite.getPosition().x, (**brush).sprite.getPosition().y);
	state.transform.rotate(getAScatter(brush));
	state.transform.translate(getPScatter(brush), getPScatter(brush));

	return state;
}

inline float Layer::getSScatter(std::vector<BrushPntr>::iterator & brush)
{
	float minScale = (**brush).getMinSizeScale();
	float maxScale = (**brush).getMaxSizeScale();

	if ((**brush).useSScatter) {
		return minScale + maxScale * (1.0f - (**brush).scatterScale * (rand() % 10 - 10) / 10.0f);
	}
	else {
		return minScale + maxScale;
	}
}

inline float Layer::getPScatter(std::vector<BrushPntr>::iterator & brush)
{
	if ((**brush).usePScatter) {
		return (**brush).maxSize * (**brush).scatterPos * (rand() % 20 - 10) / 10.0f;
	}
	else {
		return (**brush).maxSize;
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

