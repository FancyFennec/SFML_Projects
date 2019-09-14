#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "Layer.h"
#include "Brush.h"
#include "json.hpp"
using json = nlohmann::json;

namespace fs = std::experimental::filesystem;

typedef std::unique_ptr<Layer> LayerPntr;
typedef std::unique_ptr<Brush> BrushPntr;

class Scene
{
public:

	unsigned int width;
	unsigned int height;

	Layer drawingLayer;
	std::vector<Layer> layers = {};
	std::vector<Layer>::iterator currentLayer;
	std::vector<Layer>::iterator lastActiveLayer;
	
	//Auxilliary variables for the brush
	float movedDistance = 0; // keeps track of how much the brush has moved since the last render cicle
	int brushWidth = 256; //Size of the brush image
	
	//Brush settings for the current brush
	float brushSize = 0.3f;
	float stepsize = 2.0f;
	int opacity = 100;

	Layer brushLayer;
	std::vector<BrushPntr> brushes;
	std::vector<BrushPntr>::iterator currentBrush;

	std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0), sf::Vector2i(0,0) };

	Scene(unsigned int width, unsigned int height) :
		width(width),
		height(height),
		drawingLayer(Layer(width, height)),
		brushLayer(Layer(brushWidth, brushWidth)) {}

	void initialize();

	unsigned int getDistance(std::vector<Layer>::iterator iter) { return (unsigned int)std::distance(layers.begin(), iter); }
	unsigned int getDistance() { return getDistance(currentLayer); }
	unsigned int getSize() { return getDistance(lastActiveLayer); }
	
	void resetCursorPositions(sf::RenderWindow & window, Layer& layer);
	void saveBrush();
	void drawOnDrawingLayer(sf::RenderWindow& mainWindow);
	void drawOnBrushLayer(sf::RenderWindow& brushWindow);

	~Scene() {
		json brushesJson;
		brushesJson["Brushes"] = {};

		for (auto p : fs::directory_iterator(BRUSH_DIRECTORY)) {
			if (p.path().string().substr(
				p.path().string().length() - 3,
				p.path().string().length())._Equal("png")
				) {
				fs::remove(p);
			}
		}

		for (auto &brush : brushes) {
			json brushJson;
			brushJson["BrushName"] = brush->brushName;
			brushJson["BrushSize"] = brush->brushSize;
			brushJson["StepSize"] = brush->stepSize;
			brushJson["Oppacity"] = brush->opacity;
			brushJson["Flow"] = brush->flow;

			brushJson["UseSScatter"] = brush->useSScatter;
			brushJson["UsePScatter"] = brush->usePScatter;
			brushJson["UseAScatter"] = brush->useAScatter;

			brushJson["ScatterScale"] = brush->scaterScale;
			brushJson["ScatterPos"] = brush->scaterPos;
			brushJson["ScatterAngle"] = brush->scaterAngle;

			brushesJson["Brushes"].push_back(brushJson);

			brush->tex.copyToImage().saveToFile(std::string(BRUSH_DIRECTORY).append(brush->brushName).append(".png"));
		}
		std::ofstream o(std::string(BRUSH_DIRECTORY).append("brushes.json"));
		o << brushesJson << std::endl;
	}

private:
};

inline void Scene::initialize()
{
	drawingLayer.offset = sf::Vector2i(WINDOW_WIDTH / 2 - width / 2, WINDOW_HEIGHT / 2 - height / 2);
	layers.reserve(MAX_LAYERS + 1); // Reserve space for 20 Layers
	layers.push_back(Layer(width, height, sf::Color::White)); // Background Layer
	for (int i = 0; i < MAX_LAYERS; i++) {
		layers.push_back(Layer(width, height));
		layers.back().clearLayer();
	}
	currentLayer = std::next(layers.begin());
	currentLayer->layerName = "Layer1";
	lastActiveLayer = currentLayer;

	brushLayer.useOffset = false;
	brushes.reserve(20);

	std::ifstream inputStream(std::string(BRUSH_DIRECTORY).append("brushes.json"));
	json brushJson;
	inputStream >> brushJson;
	for (auto& brush : brushJson["Brushes"]) {
		brushes.push_back(std::make_unique<Brush>(
			brushWidth,
			std::string(BRUSH_DIRECTORY).append(brush["BrushName"].get<std::string>()).append(".png").data()
			)
		);
		brushes.back()->brushName = brush["BrushName"].get<std::string>();

		brushes.back()->brushSize = brush["BrushSize"].get<float>();
		brushes.back()->stepSize = brush["StepSize"].get<float>();
		brushes.back()->opacity = brush["Oppacity"].get<int>();
		brushes.back()->flow = brush["Flow"].get<int>();

		brushes.back()->useSScatter = brush["UseSScatter"].get<bool>();
		brushes.back()->usePScatter = brush["UsePScatter"].get<bool>();
		brushes.back()->useAScatter = brush["UseAScatter"].get<bool>();

		brushes.back()->scaterScale = brush["ScatterScale"].get<float>();
		brushes.back()->scaterPos = brush["ScatterPos"].get<float>();
		brushes.back()->scaterAngle = brush["ScatterAngle"].get<float>();
	}
	
	currentBrush = brushes.begin();
	(*currentBrush)->synchronizeColors();
	(*currentBrush)->setSize(brushSize);
}

inline void Scene::resetCursorPositions(sf::RenderWindow & window, Layer& layer) {
	movedDistance = 0.0f;
	sf::Vector2i newPos = sf::Mouse::getPosition(window);

	cursorPositions[0] = newPos;
	cursorPositions[1] = newPos;
	cursorPositions[2] = newPos;
	cursorPositions[3] = newPos;

	drawingLayer.resetDrawFlag();
}

inline void Scene::saveBrush() {
	sf::RenderTexture renderTex;
	renderTex.create(brushLayer.width, brushLayer.height);
	renderTex.clear(sf::Color(255, 255, 255, 0));

	renderTex.draw(brushLayer.sprite);
	renderTex.display();

	renderTex.getTexture().copyToImage().saveToFile(std::string(BRUSH_DIRECTORY).append("NewBrush.png"));

	brushes.push_back(std::make_unique<Brush>(
		brushWidth,
		std::string(BRUSH_DIRECTORY).append("NewBrush.png").data()
		)
	);
	brushes.back()->brushName = "NewBrush";
}

inline void Scene::drawOnDrawingLayer(sf::RenderWindow & mainWindow) {
	drawingLayer.drawLerpOnCanvas(movedDistance, currentBrush, cursorPositions, mainWindow);
	//mainLayer.drawCubicOnCanvas(movedDistance, currentbrush, cursorPositions);

	sf::RenderStates state;
	state.transform.translate(sf::Vector2f(currentLayer->offset));
	drawingLayer.sprite.setColor(sf::Color(255, 255, 255, (*currentBrush)->opacity));
	mainWindow.draw(drawingLayer.sprite, state);
	drawingLayer.sprite.setColor(sf::Color(255, 255, 255, 255));
}

inline void Scene::drawOnBrushLayer(sf::RenderWindow & brushWindow) {
	brushLayer.drawLerpOnCanvas(movedDistance, currentBrush, cursorPositions, brushWindow);
	brushLayer.drawLayerinWindow(brushWindow);
}
