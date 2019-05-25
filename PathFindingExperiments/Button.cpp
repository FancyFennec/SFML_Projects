#include "Button.h"

void Button::draw(sf::RenderWindow & window)
{
	if (sf::Vector2u(0, 0) != texture.getSize())
	{
		window.draw(sprite);
		if (!text.getString().isEmpty()) {
			window.draw(text);
		}
	}
}

void Button::setPosition(float x, float y)
{
	sprite.setPosition(x, y);
	text.setPosition(x + 7.0f, y + 10.0f);
}

Button::Button()
{
}

Button::Button(const char path[])
{
	if (!texture.loadFromFile(path))
		std::cout << "Can't find the image" << std::endl;
	else
	{
		sprite.setPosition(0.0f, 0.0f);
		sprite.setScale(0.2, 0.2);
		sprite.setTexture(texture);
	}
}

Button::Button(const char *path, const char *title)
{
	if (!texture.loadFromFile(path))
	{
		std::cout << "Can't find the image" << std::endl;
	}
	else
	{
		sprite.setPosition(0.0f, 0.0f);

		sprite.setScale(0.2, 0.2);
		sprite.setTexture(texture);
	}

	if (!font.loadFromFile("arial.ttf"))
	{
		std::cout << "Can't find the font file" << std::endl;
	}
	else 
	{
		text.setFont(font);
		text.setStyle(sf::Text::Bold);
		text.setString(title);
		text.setFillColor(sf::Color::Black);
		text.setCharacterSize(48);
		text.setPosition(7.0f, 10.0f);
	}
}

Button::~Button()
{
}
