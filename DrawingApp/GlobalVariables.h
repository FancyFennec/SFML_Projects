#pragma once

#include <SFML/Graphics.hpp>

static sf::Clock deltaClock;
static sf::Clock clickClock;

static float guiBrushColor[3] = { 0.5f,0.0f,0.5f }; //Currently needed to pass to the imgui color picker

//Window initialisation
static sf::RenderWindow mainWindow;
static sf::RenderWindow brushWindow;
static sf::Event event;