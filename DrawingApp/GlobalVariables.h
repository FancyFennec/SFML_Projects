#pragma once

#include <SFML/Graphics.hpp>
#include "Structs.h"

static DrawingState DRAWING_STATE = ALPHA;
sf::Shader alphaBlendingShader;
sf::Shader normalBlendingShader;
sf::Shader mainRenderShader;
sf::RenderStates alphaBlendingRState(&alphaBlendingShader);
sf::RenderStates normalBlendingRState(&normalBlendingShader);
sf::RenderStates mainRenderState(&mainRenderShader);

static const std::string ALPHA_BLENDING_SHADER_PATH = "./alpha_blending_shader.frag";
static const std::string NORMAL_BLENDING_SHADER_PATH = "./normal_blending_shader.frag";
static const std::string RENDER_SHADER_PATH = "./render_shader.frag";
static const char* BRUSH_DIRECTORY = "./Brushes/";
static const char* SCENES_DIRECTORY = "./Scenes/";

static sf::Clock deltaClock; //For imgui
static sf::Clock clickClock; //For double click timing

//Window initialisation
POINT p;
static sf::RenderWindow mainWindow;
static sf::Event event;

sf::RenderTexture mainRenderTex;
sf::Texture mainNormalTex;
sf::Sprite mainNormalSprite;
sf::Sprite mainRenderSprite;

sf::Texture normalTex; // texture for the normal color picker
sf::Texture samplingTexture; //texture used for sampling the normal color
sf::Vector2i sampledPos;

bool pickNormalValue = false;

inline bool loadShaders()
{
	bool returnValue = true;

	if (!alphaBlendingShader.loadFromFile(ALPHA_BLENDING_SHADER_PATH, sf::Shader::Fragment)) {
		std::cout << "Could not load ALphaBlendingShader" << std::endl;
		returnValue = false;
	}
	if (!normalBlendingShader.loadFromFile(NORMAL_BLENDING_SHADER_PATH, sf::Shader::Fragment)) {
		std::cout << "Could not load NormalBlendingShader" << std::endl;
		returnValue = false;
	}
	if (!mainRenderShader.loadFromFile(RENDER_SHADER_PATH, sf::Shader::Fragment)) {
		std::cout << "Could not load RenderShader" << std::endl;
		returnValue = false;
	}

	alphaBlendingRState.blendMode = sf::BlendNone;
	normalBlendingRState.blendMode = sf::BlendNone;
	mainRenderState.blendMode = sf::BlendNone;

	return returnValue;
}
