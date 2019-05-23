#include "GameObject.h"

sf::Vector2i pos;

GameObject::GameObject()
{
}

GameObject::GameObject(sf::Vector2i pos)
{
	this->pos = pos;
}


GameObject::~GameObject()
{
}
