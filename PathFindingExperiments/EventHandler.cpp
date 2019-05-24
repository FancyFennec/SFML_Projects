#include "EventHandler.h"




void EventHandler::handleEvents(sf::Window &window, std::vector <DrawableObject> &drawables, char (&tiles)[TILES_WIDTH][TILES_HEIGHT], sf::Sprite &buttonImage)
{
	if (lMousePressed) {
		tiles[(int)(mousePos.x / 10)][(int)(mousePos.y / 10)] = 'r';
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
			drawables.clear();

		}
		else if (event.mouseButton.button == sf::Mouse::Left)
		{
			lMousePressed = true;
		}
		else if (event.mouseButton.button == sf::Mouse::Right)
		{
			DrawableObject drawable(sf::Vector2i((int)(mousePos.x / 10), (int)(mousePos.y / 10)));
			drawables.push_back(drawable);
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
