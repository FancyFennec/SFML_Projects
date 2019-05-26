#include "EventHandler.h"




void EventHandler::handleEvents(sf::Window &window, std::vector <DrawableObject> &drawables, char (&tiles)[TILES_WIDTH][TILES_HEIGHT], sf::Sprite &buttonImage)
{
	if (lMousePressed) 
	{
		int tileX = (int)(mousePos.x / 10);
		int tileY = (int)(mousePos.y / 10);

		tiles[tileX][tileY] = 'r';
		if(tileX < TILES_WIDTH -1) {
			tiles[tileX + 1][tileY] = 'r';
		}
		if (tileY < TILES_HEIGHT - 1) {
			tiles[tileX][tileY + 1] = 'r';
		}
		if (tileX < TILES_WIDTH - 1 && tileY < TILES_HEIGHT - 1) {
			tiles[tileX + 1][tileY + 1] = 'r';
		}
	}

	switch (event.type)
	{
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::MouseMoved:
	{
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		if (buttonImage.getGlobalBounds().contains(mousePosF))
		{
			buttonImage.setColor(sf::Color(250, 50, 50));
		}
		else
		{
			buttonImage.setColor(sf::Color(255, 255, 255));
		}
	}
	break;
	case sf::Event::MouseButtonPressed:
	{
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		if (buttonImage.getGlobalBounds().contains(mousePosF))
		{
			std::cout << "Screen cleared, yay!" << std::endl;
			for (auto& rows : tiles)
			{
				for (char& tile : rows)
				{
					if (tile == 'r')
						tile = 'x';
				}
			}

			//TODO: This is a stupid workaround for the memory leak -.-
			for (DrawableObject drawable : drawables)
				delete[] drawable.pixels;
			drawables.clear();

		}
		else if (event.mouseButton.button == sf::Mouse::Left)
		{
			lMousePressed = true;
		}
		else if (event.mouseButton.button == sf::Mouse::Right)
		{
			drawables.push_back(DrawableObject(sf::Vector2i((int)(mousePos.x / 10), (int)(mousePos.y / 10))));
		}
	}
	break;
	case sf::Event::MouseButtonReleased:
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			lMousePressed = false;
		}
	}
	break;
	}
}

EventHandler::EventHandler()
{
	lMousePressed = false;
}


EventHandler::~EventHandler()
{
}
