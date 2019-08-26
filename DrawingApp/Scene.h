#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Layer.h"
#include "Brush.h"

typedef std::unique_ptr<Layer> LayerPntr;
typedef std::unique_ptr<Brush> BrushPntr;

class Scene
{
public:

	const unsigned int width;
	const unsigned int height;

	Layer drawingLayer;
	std::vector<LayerPntr> layers = {};
	std::vector<LayerPntr>::iterator currentLayer;
	
	//Brush settings for the current brush
	int brushWidth = 256;
	float brushSize = 0.3f;
	float movedDistance = 0;
	float stepsize = 2.0f;
	int alpha = 100;
	static float currentColor[3];

	Layer brushLayer;
	std::vector<BrushPntr> brushes = {};
	std::vector<BrushPntr>::iterator currentBrush;

	std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0) };

	Scene(unsigned int width, unsigned int height) :
		width(width),
		height(height),
		drawingLayer(Layer(width, height)),
		brushLayer(Layer(brushWidth, brushWidth))
	{
		initialize();
	}

	void updateColor();
	void pickColor(sf::RenderWindow &window);
	void resetCursorPositions(sf::RenderWindow & window, Layer& layer);
	void saveBrush();
	void drawOnDrawingLayer(sf::RenderWindow& mainWindow);
	void drawOnBrushLayer(sf::RenderWindow& brushWindow);

	~Scene() {
	}

private:
	void initialize();
};

float Scene::currentColor[3] = { 0.5f,0.0f,0.5f };

inline void Scene::initialize()
{
	layers.reserve(21); // Reserve space for 20 Layers
	layers.push_back(std::make_unique<Layer>(width, height, sf::Color::White)); // Background Layer
	layers.push_back(std::make_unique<Layer>(width, height)); // One layer to draw on
	currentLayer = std::prev(layers.end());

	brushLayer.useOffset = false;
	brushes.reserve(20);
	brushes.push_back(std::make_unique<Brush>(
		brushWidth,
		"./Brushes/StandardBrush.png"
		)
	);
	brushes.push_back(std::make_unique<Brush>(
		brushWidth,
		"./Brushes/FadingBrush.png"
		)
	);
	currentBrush = brushes.begin();
	(*currentBrush)->color = sf::Color((sf::Uint8)(currentColor[0] * 255), (sf::Uint8)(currentColor[1] * 255), (sf::Uint8)(currentColor[2] * 255), 255);
	(*currentBrush)->setBrushSize(brushSize);
}

inline void Scene::updateColor() {
	(*currentBrush)->color.r = (sf::Uint8)(currentColor[0] * 255);
	(*currentBrush)->color.g = (sf::Uint8)(currentColor[1] * 255);
	(*currentBrush)->color.b = (sf::Uint8)(currentColor[2] * 255);
}

inline void Scene::pickColor(sf::RenderWindow & window) {
	sf::Vector2i pos = sf::Mouse::getPosition(window);

	sf::Texture newTex;
	newTex.create(window.getSize().x, window.getSize().y);
	newTex.update(window);
	(*currentBrush)->color = newTex.copyToImage().getPixel(pos.x, pos.y);

	currentColor[0] = (*currentBrush)->color.r / 255.0f;
	currentColor[1] = (*currentBrush)->color.g / 255.0f;
	currentColor[2] = (*currentBrush)->color.b / 255.0f;
}

inline void Scene::resetCursorPositions(sf::RenderWindow & window, Layer& layer) {
	movedDistance = 0.0f;
	sf::Vector2i newPos = sf::Mouse::getPosition(window);

	if (layer.useOffset) {
		newPos -= (*currentLayer)->offset;
	}

	cursorPositions[0] = newPos;
	cursorPositions[1] = newPos;
	cursorPositions[2] = newPos;
	cursorPositions[3] = newPos;

	drawingLayer.resetDrawFlag();
}

inline void Scene::saveBrush() {
	sf::RenderTexture renderTex;
	renderTex.create(brushLayer.image.getSize().x, brushLayer.image.getSize().y);
	renderTex.clear(sf::Color(255, 255, 255, 0));

	renderTex.draw(brushLayer.sprite);
	renderTex.display();

	renderTex.getTexture().copyToImage().saveToFile("./Brushes/BrushWindow.png");

	brushes.push_back(std::make_unique<Brush>(
		brushWidth,
		"./Brushes/BrushWindow.png"
		)
	);
}

inline void Scene::drawOnDrawingLayer(sf::RenderWindow & mainWindow) {
	drawingLayer.drawLinearOnCanvas(movedDistance, currentBrush, cursorPositions, mainWindow);
	//mainLayer.drawCubicOnCanvas(movedDistance, currentbrush, cursorPositions);

	sf::RenderStates state;
	state.transform.translate(sf::Vector2f((*currentLayer)->offset));
	drawingLayer.sprite.setColor(sf::Color(255, 255, 255, (*currentBrush)->opacity));
	mainWindow.draw(drawingLayer.sprite, state);
	drawingLayer.sprite.setColor(sf::Color(255, 255, 255, 255));
}

inline void Scene::drawOnBrushLayer(sf::RenderWindow & brushWindow) {
	brushLayer.drawLinearOnCanvas(movedDistance, currentBrush, cursorPositions, brushWindow);
	brushLayer.drawLayer(brushWindow);
}
