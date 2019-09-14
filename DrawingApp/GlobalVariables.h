#pragma once

#include <SFML/Graphics.hpp>

static sf::Clock deltaClock; //For imgui
static sf::Clock clickClock; //For click timing

//Window initialisation
static sf::RenderWindow mainWindow;
static sf::RenderWindow brushWindow;
static sf::Event event;