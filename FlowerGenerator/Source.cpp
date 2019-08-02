#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const float angle = 0.5;
//const float angle = 2.39996;
float dist = 10;
const float radius = 20;

sf::RenderWindow mainWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "FlowerGenerator");
sf::Event event;

std::vector<sf::CircleShape*> circles;

float max(std::vector<float> scalProds) {
	float max = 0.;
	for (float value : scalProds) {
		if (value > max) {
			max = value;
		}
	}
	return max;
}

void addCircle() {
	sf::CircleShape* newCircle = new sf::CircleShape();
	newCircle->setRadius(radius);
	std::vector<float> scalProds;

	for (sf::CircleShape* circle : circles) {
		float yProj = sin(angle * circles.size()) * (circle->getPosition().x - SCREEN_WIDTH / 2 + radius) +
			cos(angle * circles.size()) * (circle->getPosition().y - SCREEN_HEIGHT / 2 + radius);
		float xProj = abs(-cos(angle * circles.size()) * (circle->getPosition().x - SCREEN_WIDTH / 2 + radius) +
			sin(angle * circles.size()) * (circle->getPosition().y - SCREEN_HEIGHT / 2 + radius));
		if (yProj >= 0) {
			if (xProj < 2 * radius) {
				scalProds.push_back(yProj + sqrt(4 * radius * radius - xProj * xProj));
			}
			else {
				scalProds.push_back(yProj);
			}

		}
	}
	if (scalProds.size() > 0) {
		dist = max(scalProds);
	}
	else {
		dist = 0;
	}
	newCircle->setPosition(
		SCREEN_WIDTH / 2 - radius + dist * sin(angle * circles.size()), 
		SCREEN_HEIGHT / 2 - radius + dist * cos(angle * circles.size())
	);

	sf::Uint8 color = 255 * std::max({ 0.f, ((float)SCREEN_WIDTH / 2 - dist) / ((float)SCREEN_WIDTH / 2) });
	newCircle->setFillColor(sf::Color::Color(color,0,0,255));
	circles.push_back(newCircle);
}

void mainWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			addCircle();
		}
	}
}

int main() {
	mainWindow.setFramerateLimit(60);
	addCircle();

	while (mainWindow.isOpen())
	{
		while (mainWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mainWindow.close();
			mainWindowEventHandling();
		}

		mainWindow.clear();

		for (sf::CircleShape* circle : circles) {
			mainWindow.draw(*circle);
		}

		mainWindow.display();
	}

	return 0;
}