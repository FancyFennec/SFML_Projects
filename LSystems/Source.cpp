#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

void eventHandling();
bool checkString(std::string input);
sf::Vector2f rotate(float alpha, sf::Vector2f vec);
std::string createString(unsigned int n);
void drawString(std::string input);

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
float pi = 3.14159265358979323846f;

sf::RenderStates state;

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "L-Systems");
sf::Event event;

unsigned int step = 1;
std::string result = "";

float angle = pi * 2.0f / 12.0f;
std::string axiom = "B";
std::map<char, std::string> rules = { 
	{'A', "AA"}, {'B', "A[-B]+B"}
	, {'[', "["}, {']', "]"}, {'-', "-"}, {'+', "+"}
};




int main() {

	window.setFramerateLimit(60);
	state.transform.translate(sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 300));

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			eventHandling();
		}

		window.clear();

		drawString(result);

		window.display();
	}

	return 0;
}

bool checkString(std::string input)
{
	std::string brackets = {};

	for (char a : input) {
		if (a == '[') {
			brackets.push_back(a);
		}
		if (a == ']') {
			if (brackets.back() == '[') {
				brackets.pop_back();
			}
			else {
				return false;
			}
		}
	}

	if (brackets.size() > 0) {
		return false;
	}
	else {
		return true;
	}
}

sf::Vector2f rotate(float alpha, sf::Vector2f vec)
{
	return sf::Vector2f(
		cosf(alpha) * vec.x - sinf(alpha) * vec.y,
		sinf(alpha) * vec.x + cosf(alpha) * vec.y
		);
}

std::string createString(unsigned int n)
{
	std::string value = axiom;

	for (int i = 0; i < n; i++) {
		std::string newValue = "";
		for (char c : value) {
			newValue += rules[c];
		}
		value = newValue;
	}

	return value;
}

void drawString(std::string input)
{

	if (!checkString(input)) {
		std::cout << "ERROR!!! String does not have correct format!" << std::endl;
		return;
	}

	sf::Vector2f start(0.0f, -1200.0f / (float)(pow(2, step)));
	std::vector<sf::Vector2f> pool = { start };
	std::vector<sf::Vector2f> savedPos = { sf::Vector2f(0.0f, 0.0f) };
	std::vector<sf::Vertex> line = { sf::Vector2f (0.0f, 0.0f) };

	for (char c : input) {
		switch (c) {
		case ('A'):
		case ('B'): {
			line.push_back(line.back().position + pool.back());
			break;
		}
		case('['): {
			window.draw(line.data(), line.size(), sf::LinesStrip, state);

			savedPos.push_back(line.back().position);
			line = { savedPos.back() };

			break;
		}
		case('-'): {
			pool.push_back(rotate(angle, pool.back()));

			break;
		}
		case(']'): {
			window.draw(line.data(), line.size(), sf::LinesStrip, state);

			line = { savedPos.back() };
			savedPos.pop_back();

			break;
		}
		case('+'): {
			pool.pop_back();
			pool.back() = rotate(-angle, pool.back());

			break;
		}
		}
	}
	window.draw(line.data(), line.size(), sf::LinesStrip, state);
}

void eventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			result = createString(step);
			step++;
		}
	}

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			window.close();
		}
	}
}