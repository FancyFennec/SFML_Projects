#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
	sf::Vector2i pos;

	GameObject();
	GameObject(sf::Vector2i pos);
	~GameObject();
};

