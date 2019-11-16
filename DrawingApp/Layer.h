#pragma once

#include "Brush.h"
#include "Settings.h"
#include "CursorBuffer.h"

using BrushPntr = std::unique_ptr<Brush>;

class Layer
{
public:
	std::string name = "Layer";
	unsigned int width;
	unsigned int height;

	Material material;
	sf::Texture tex;
	sf::Sprite sprite;

	static bool isOffsetSet;
	bool useOffset = true;
	static sf::Vector2i offset;

	Layer(unsigned int width, unsigned int height, sf::Color color = sf::Color(255, 255, 255, 0)) :
		width(width),
		height(height)
    {
		initialize(color);
	}

	void setRenderUnifroms(const LightSource& lightSource);
	void setRenderUnifroms(const LightSource& lightSource, const sf::Texture& texture);
	void clearLayer();
	void blendlayers(const Layer& drawingLayer, std::vector<BrushPntr>::iterator& brush);
	void drawLayerinWindow();
	void lerpDrawingOnLayer(std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>::iterator iter);

private:
	static sf::RenderTexture rTex;

	void initialize(const sf::Color& color);
	static float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2);
	float getSScatter(std::vector<BrushPntr>::iterator& brush);
	float getPScatter(std::vector<BrushPntr>::iterator& brush);
	float getAScatter(std::vector<BrushPntr>::iterator& brush);
	sf::RenderStates getRenderState(std::vector<BrushPntr>::iterator & brush);
};

bool Layer::isOffsetSet = false;
sf::Vector2i Layer::offset = sf::Vector2i(0, 0);
sf::RenderTexture Layer::rTex;

void Layer::initialize(const sf::Color& color) 
{
	sf::Image image;
	image.create(width, height, color);
	tex.loadFromImage(image);
	sprite.setTexture(tex);

	if (!isOffsetSet) {
		offset = sf::Vector2i(WINDOW_WIDTH / 2 - width / 2, WINDOW_HEIGHT / 2 - height / 2);
		isOffsetSet = true;
	}
	rTex.create(width, height);
}

void Layer::setRenderUnifroms(const LightSource& lightSource)
{
	mainRenderShader.setUniform("normalMap", sf::Shader::CurrentTexture);
	mainRenderShader.setUniform("layerTex", tex);

	mainRenderShader.setUniform("lightPos", lightSource.pos);
	mainRenderShader.setUniform("lightCol", lightSource.col);

	mainRenderShader.setUniform("shininess", material.shininess);
	mainRenderShader.setUniform("specInt", material.specInt);
	mainRenderShader.setUniform("ambInt", material.ambInt);
	mainRenderShader.setUniform("difInt", material.difInt);
}

void Layer::setRenderUnifroms(const LightSource& lightSource, const sf::Texture& texture)
{
	mainRenderShader.setUniform("normalMap", sf::Shader::CurrentTexture);
	mainRenderShader.setUniform("layerTex", texture);

	mainRenderShader.setUniform("lightPos", lightSource.pos);
	mainRenderShader.setUniform("lightCol", lightSource.col);

	mainRenderShader.setUniform("shininess", material.shininess);
	mainRenderShader.setUniform("specInt", material.specInt);
	mainRenderShader.setUniform("ambInt", material.ambInt);
	mainRenderShader.setUniform("difInt", material.difInt);
}

void Layer::clearLayer() 
{
	sf::Image image;
	image.create(width, height, sf::Color(255, 255, 255, 0));
	tex.update(image);
	sprite.setTexture(tex);
}

void Layer::blendlayers(const Layer& drawingLayer, std::vector<BrushPntr>::iterator& brush) 
{
	switch (DRAWING_STATE) {
	case(DrawingState::ALPHA): {
		alphaBlendingShader.setUniform("texture1", sf::Shader::CurrentTexture);
		alphaBlendingShader.setUniform("texture2", drawingLayer.tex);
		alphaBlendingShader.setUniform("alpha", (**brush).opacity / 255.0f);

		rTex.draw(sprite, alphaBlendingRState);
		break;
	}
	case(DrawingState::NORMAL): {
		normalBlendingShader.setUniform("texture1", sf::Shader::CurrentTexture);
		normalBlendingShader.setUniform("texture2", drawingLayer.tex);
		normalBlendingShader.setUniform("alpha", (**brush).opacity / 255.0f);

		rTex.draw(sprite, normalBlendingRState);
		break;
	}
	}
	
	rTex.display();

	tex = rTex.getTexture();
	sprite.setTexture(tex);
}


void Layer::drawLayerinWindow() 
{
	mainWindow.draw(sprite);
}

void Layer::lerpDrawingOnLayer(std::vector<BrushPntr>::iterator& brush, std::vector<sf::Vector2i>::iterator iter)
{
	sf::RenderTexture renderTex;
	renderTex.create(width, height);

	switch (DRAWING_STATE)
	{
	case DrawingState::ALPHA:
		renderTex.clear(sf::Color((**brush).currentColor.r, (**brush).currentColor.g, (**brush).currentColor.b, 0));
		break;
	case DrawingState::NORMAL:
		renderTex.clear(sf::Color((**brush).currentNormal.r, (**brush).currentNormal.g, (**brush).currentNormal.b, 0));
		break;
	default:
		break;
	}

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

float Layer::distance(const sf::Vector2i & vec1, const sf::Vector2i & vec2)
{
	return std::sqrt(std::pow((vec1.x - vec2.x), 2.0f) + std::pow(vec1.y - vec2.y, 2.0f));
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

float Layer::getSScatter(std::vector<BrushPntr>::iterator & brush)
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
