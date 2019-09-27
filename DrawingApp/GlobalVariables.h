#pragma once

#include <mutex>
#include <SFML/Graphics.hpp>

static const std::string ALPHA_BLENDING_SHADER_PATH = "./alpha_blending_shader.frag";
static const std::string NORMAL_BLENDING_SHADER_PATH = "./normal_blending.frag";
static const std::string RENDER_SHADER_PATH = "./render_shader.frag";
static const char* BRUSH_DIRECTORY = "./Brushes/";
static const char* SCENES_DIRECTORY = "./Scenes/";

static sf::Clock deltaClock; //For imgui
static sf::Clock clickClock; //For click timing

//Window initialisation
static sf::RenderWindow mainWindow;
static sf::Event event;

sf::Texture normalTex;
sf::Texture samplingTexture;
sf::Vector2i sampledPos;

bool pickNormalValue = false;

sf::Color getSampledColor() {
	samplingTexture.update(mainWindow);
	sampledPos = sf::Mouse::getPosition(mainWindow);
	return samplingTexture.copyToImage().getPixel(sampledPos.x, sampledPos.y);
}