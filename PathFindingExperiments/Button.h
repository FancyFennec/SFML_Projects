#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Button
{
public:

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Text text;
	sf::Font font;
	
	void draw(sf::RenderWindow &window);
	void setPosition(float x, float y);

	Button(const char *path);
	Button(const char *path, const char *text);
	~Button();

private:
	Button();
};

