#pragma once

#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>

#include "Brush.h"
#include "Layer.h"
#include "json.hpp"
using nlohmann::json;

namespace fs = std::experimental::filesystem;

using BrushPntr = std::unique_ptr<Brush>;

//TODO: move saving and loading to a seperate class
class Scene
{
public:
	unsigned int width;
	unsigned int height;

	LightSource lightSource;

	Layer normalLayer;
	Layer drawingLayer;

	std::vector<Layer> layers = {};
	std::vector<Layer>::iterator currentLayer;
	std::vector<Layer>::iterator lastActiveLayer;
	
	std::vector<BrushPntr> brushes;
	std::vector<BrushPntr>::iterator currentBrush;

	std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0) };

	Scene(unsigned int width, unsigned int height) :
		width(width),
		height(height),
		normalLayer(Layer(width, height, sf::Color(127, 127, 255))),
		drawingLayer(Layer(width, height)) 
    {
		initialize();
	}

	~Scene() {
		clearBrushDirectory();
		saveBrushesToJSON();
	}

	void setGlobalNormalSprite();
	void drawLowerLayers();
	void drawCurrentLayer();
	void drawUpperLayers();
	void resetScene(unsigned int newWidth, unsigned  int newHeight);
	void reloadLayerSprites();
	unsigned int getLayerDistance(std::vector<Layer>::const_iterator iter) const { return (unsigned int)std::distance(layers.begin(), iter); }
	unsigned int getLayerDistance() const { return getLayerDistance(currentLayer); }
	unsigned int getSize() const { return getLayerDistance(lastActiveLayer); }

private:
	const int BRUSH_WIDTH = 256; //Size of the brush image

	sf::Image clearingImage;
	sf::Texture clearingTexture;

	void initialize();
	void loadBrushesFromJSON();
    void clearBrushDirectory() const;
	void saveBrushesToJSON() const;
};

inline void Scene::initialize()
{
	clearingImage.create(width, height, sf::Color::White);
	clearingTexture.loadFromImage(clearingImage);

	drawingLayer.offset = sf::Vector2i(WINDOW_WIDTH / 2 - width / 2, WINDOW_HEIGHT / 2 - height / 2);
	layers.reserve(MAX_LAYERS + 1); // Reserve space for 20 Layers
	layers.push_back(Layer(width, height, sf::Color::White)); // Background Layer
	for (int i = 0; i < MAX_LAYERS; i++) {
		layers.push_back(Layer(width, height));
		layers.back().clearLayer();
	}
	reloadLayerSprites();
	currentLayer = std::next(layers.begin());
	currentLayer->name = "Layer1";
	lastActiveLayer = currentLayer;

	loadBrushesFromJSON();
}

inline void Scene::setGlobalNormalSprite()
{
	switch (DRAWING_STATE) {
	case DrawingState::ALPHA: {
		mainNormalSprite.setTexture(normalLayer.tex);
		break;
	}
	case DrawingState::NORMAL: {
		sf::RenderTexture rTex;
		rTex.create(width, height);

		normalBlendingShader.setUniform("texture1", sf::Shader::CurrentTexture);
		normalBlendingShader.setUniform("texture2", drawingLayer.tex);
		normalBlendingShader.setUniform("alpha", (**currentBrush).opacity / 255.0f);

		rTex.draw(normalLayer.sprite, normalBlendingRState);
		rTex.display();

		mainNormalTex = rTex.getTexture();
		mainNormalSprite.setTexture(mainNormalTex);
		break;
	}
	}
}

inline void Scene::drawLowerLayers()
{
	sf::RenderStates rs;
	rs.transform.translate(sf::Vector2f(currentLayer->offset));

	for (auto iter = layers.begin(); iter < currentLayer; std::advance(iter, 1)) {

		iter->setRenderUnifroms(lightSource);
		mainRenderTex.draw(mainNormalSprite, mainRenderState);

		mainRenderTex.display();
		mainRenderSprite.setTexture(mainRenderTex.getTexture());
		mainWindow.draw(mainRenderSprite, rs);
	}
}

inline void Scene::drawCurrentLayer() {
	sf::RenderTexture rTex;
	sf::Texture newTex;
	rTex.create(width, height);


	if (DRAWING_STATE == DrawingState::ALPHA) { //In alpha drawing state we first alpha blend the drawing layer with the current layer
		alphaBlendingShader.setUniform("texture1", sf::Shader::CurrentTexture);
		alphaBlendingShader.setUniform("texture2", drawingLayer.tex);
		alphaBlendingShader.setUniform("alpha", (**currentBrush).opacity / 255.0f);

		rTex.draw(currentLayer->sprite, alphaBlendingRState);
		rTex.display();
		newTex = rTex.getTexture(); //If we don't assign the rendertexture to a new texture is upside down
	} // else we just draw the current Layer

	sf::RenderStates renderState;
	renderState.transform.translate(sf::Vector2f(currentLayer->offset));

	currentLayer->setRenderUnifroms(lightSource, DRAWING_STATE == DrawingState::ALPHA ? newTex : currentLayer->tex);
	mainRenderTex.draw(mainNormalSprite, mainRenderState);

	mainRenderTex.display();
	mainRenderSprite.setTexture(mainRenderTex.getTexture());
	mainWindow.draw(mainRenderSprite, renderState);
}

inline void Scene::drawUpperLayers()
{
	sf::RenderStates rs;
	rs.transform.translate(sf::Vector2f(currentLayer->offset));

	if (currentLayer != lastActiveLayer) {
		for (auto iter = std::next(currentLayer); iter <= lastActiveLayer; std::advance(iter, 1)) {
			iter->setRenderUnifroms(lightSource);
			mainRenderTex.draw(mainNormalSprite, mainRenderState);

			mainRenderTex.display();
			mainRenderSprite.setTexture(mainRenderTex.getTexture());
			mainWindow.draw(mainRenderSprite, rs);
		}
	}
}

inline void Scene::resetScene(unsigned int newWidth, unsigned  int newHeight)
{
	this->width = newWidth;
	this->height = newHeight;
	
	int counter = 0;
	for (auto layer : layers) {
		layer.name = "Layer";
		layer.name.append(std::to_string(counter));
		counter++;

		layer.width = width;
		layer.height = height;
		layer.clearLayer();
	}
}

inline void Scene::reloadLayerSprites()
{
	for (auto layer = layers.begin(); layer < layers.end(); layer++) {
		layer->sprite.setTexture(layer->tex);
	}
}

inline void Scene::loadBrushesFromJSON()
{
	brushes.reserve(20);

	std::ifstream inputStream(std::string(BRUSH_DIRECTORY).append("brushes.json"));
	json brushJson;
	inputStream >> brushJson;
	for (auto& brush : brushJson["Brushes"]) {
		brushes.push_back(std::make_unique<Brush>(std::string(BRUSH_DIRECTORY).append(brush["BrushName"].get<std::string>()).append(".png").data()));
		brushes.back()->name = brush["BrushName"].get<std::string>();

		brushes.back()->maxSize = brush["BrushSize"].get<float>();
		brushes.back()->stepSize = brush["StepSize"].get<float>();
		brushes.back()->opacity = brush["Oppacity"].get<int>();
		brushes.back()->maxFlow = brush["Flow"].get<int>();

		brushes.back()->useSScatter = brush["UseSScatter"].get<bool>();
		brushes.back()->usePScatter = brush["UsePScatter"].get<bool>();
		brushes.back()->useAScatter = brush["UseAScatter"].get<bool>();

		brushes.back()->scatterScale = brush["ScatterScale"].get<float>();
		brushes.back()->scatterPos = brush["ScatterPos"].get<float>();
		brushes.back()->scatterAngle = brush["ScatterAngle"].get<float>();
	}

	currentBrush = brushes.begin();
	(*currentBrush)->synchronizeBrushColor();
	(*currentBrush)->setSpriteSize();
}

inline void Scene::clearBrushDirectory() const
{
	for (auto p : fs::directory_iterator(BRUSH_DIRECTORY)) {
		if (!fs::is_directory(p)) { // Only delete the pngs
			if (p.path().string().substr(
				p.path().string().length() - 3,
				p.path().string().length())._Equal("png")
				) {
				fs::remove(p);
			}
		}
	}
}

inline void Scene::saveBrushesToJSON() const
{
	json brushesJson;
	brushesJson["Brushes"] = {};

	for (auto &brush : brushes) {
		json brushJson;
		brushJson["BrushName"] = brush->name;
		brushJson["BrushSize"] = brush->maxSize;
		brushJson["StepSize"] = brush->stepSize;
		brushJson["Oppacity"] = brush->opacity;
		brushJson["Flow"] = brush->maxFlow;

		brushJson["UseSScatter"] = brush->useSScatter;
		brushJson["UsePScatter"] = brush->usePScatter;
		brushJson["UseAScatter"] = brush->useAScatter;

		brushJson["ScatterScale"] = brush->scatterScale;
		brushJson["ScatterPos"] = brush->scatterPos;
		brushJson["ScatterAngle"] = brush->scatterAngle;

		brushesJson["Brushes"].push_back(brushJson);

		brush->tex.copyToImage().saveToFile(std::string(BRUSH_DIRECTORY).append(brush->name).append(".png"));
	}
	std::ofstream o(std::string(BRUSH_DIRECTORY).append("brushes.json"));
	o << brushesJson << std::endl;
}

