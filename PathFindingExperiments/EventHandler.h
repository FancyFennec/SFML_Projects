#pragma once

#include "Constants.h"
#include "DrawableObject.h"

class EventHandler
{
public:

	sf::Event event;
	bool lMousePressed;

	void handleEvents(sf::Window &window, std::vector <DrawableObject> &drawables, char(&tiles)[TILES_WIDTH][TILES_HEIGHT], sf::Sprite &buttonImage);

	EventHandler();
	~EventHandler();

private:
	sf::Vector2i mousePos;
};

