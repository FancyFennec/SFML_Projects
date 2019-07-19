#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib> 

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"


void eventHandling();
bool checkString(std::string input);
sf::Vector2f rotate(float alpha, sf::Vector2f vec);
std::string createString(unsigned int n);
void createLines(std::string input);
void setColour(std::vector<sf::Vertex>& line);

sf::Clock deltaClock;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
float pi = 3.14159265358979323846f;

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "L-Systems");
sf::RenderStates state;
sf::Event event;

sf::Texture tex;

bool mouseIsPressed = false;

int step = 1;
int oldStep = 0;
float size = 1.0f;
float oldSize = 1.0f;

std::string result = "";

//float angle = pi * 45.0f / 360.0f;
//std::string axiom = "B";
//std::map<char, std::string> rules = { 
//	{'A', "AA"}, {'B', "A[-B]+B"}
//	, {'[', "["}, {']', "]"}, {'-', "-"}, {'+', "+"}
//};

float angle = pi * 25.0f / 360.0f;
std::string axiom = "C";
std::map<char, std::string> rules = {
	{'A', "AA"}, {'C', "A+[[C]-C]-A[-AC]+C"}
	, {'[', "["}, {']', "]"}, {'-', "-"}, {'+', "+"}
};

std::vector<std::vector<sf::Vertex>> lines = {};

int main() {

	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	tex.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite sprite(tex);

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				window.close();
			eventHandling();
		}

		window.clear(sf::Color(25.5f, 25.5f, 25.5f, 255.0f));

		sprite.setTexture(tex);
		window.draw(sprite);

		for (std::vector<sf::Vertex> line : lines) {
			sf::Vector2i mousePos = sf::Mouse::getPosition();

			state.transform.translate(-570 + mousePos.x, mousePos.y);
			window.draw(line.data(), line.size(), sf::LinesStrip, state);
			state.transform.translate(570 - mousePos.x, -mousePos.y);
		}
		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered()) {
			if (mouseIsPressed) {
				tex.update(window);
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		ImGui::InputInt("L-Ssteps", &step);
		ImGui::SliderFloat("Size", &size, 0.0f, 1.0f);

		if (oldStep != step) {
			result = createString(step);
			createLines(result);
			oldStep = step;
		}
		if (oldSize != size) {
			createLines(result);
			oldSize = size;
		}

		ImGui::End();

		ImGui::SFML::Render(window);

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

void createLines(std::string input)
{

	if (!checkString(input)) {
		std::cout << "ERROR!!! String does not have correct format!" << std::endl;
		return;
	}

	srand(time(0));  // Initialize random number generator.
	float r;
	lines.clear();
	sf::Vector2f currentSegment(0.0f, - size * 400.0f / (float)(pow(2, step)));

	std::vector<sf::Vector2f> lineSegments = { currentSegment };
	std::vector<sf::Vector2f> savedPos = { sf::Vector2f(0.0f, 0.0f) };
	std::vector<sf::Vertex> line = { sf::Vector2f (0.0f, 0.0f) };
	setColour(line);
	

	for (char c : input) {

		r = (float)((rand() % 11) + -5) / 20.0f;

		switch (c) {
		case ('A'):
		case ('B'): {
			line.push_back(line.back().position + currentSegment * (1 + r));
			setColour(line);
			break;
		}
		case('['): {
			if (line.size() > 1) {
				lines.push_back(line);
				lineSegments.push_back(line.back().position - line[line.size() - 2].position);
			}
			else {
				lineSegments.push_back(lineSegments.back());
			}

			currentSegment = lineSegments.back();
			savedPos.push_back(line.back().position);
			line = { savedPos.back() };
			setColour(line);

			break;
		}
		case(']'): {
			if (line.size() > 1) lines.push_back(line);

			currentSegment = lineSegments.back();
			lineSegments.pop_back();

			line = { savedPos.back() };
			setColour(line);
			savedPos.pop_back();

			break;
		}
		case('-'): {
			currentSegment = rotate(angle  * (1 + r), currentSegment);

			break;
		}
		case('+'): {
			currentSegment = rotate(-angle * (1 + r), currentSegment);

			break;
		}
		}
	}
	lines.push_back(line);
}

void setColour(std::vector<sf::Vertex>& line)
{
	line.back().color.r = sf::Color::Green.r * (1 + line.back().position.y / SCREEN_HEIGHT) - sf::Color::Red.r * line.back().position.y / SCREEN_HEIGHT;
	line.back().color.g = sf::Color::Green.g * (1 + line.back().position.y / SCREEN_HEIGHT) - sf::Color::Red.g * line.back().position.y / SCREEN_HEIGHT;
	line.back().color.b = sf::Color::Green.b * (1 + line.back().position.y / SCREEN_HEIGHT) - sf::Color::Red.b * line.back().position.y / SCREEN_HEIGHT;
}

void eventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			mouseIsPressed = true;
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			mouseIsPressed = false;
		}
	}
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Right) {
			createLines(result);
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Q) {
			tex.create(SCREEN_WIDTH, SCREEN_HEIGHT);
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			window.close();
		}
	}
}