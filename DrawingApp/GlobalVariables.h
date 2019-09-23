#pragma once

#include <mutex>
#include <SFML/Graphics.hpp>

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