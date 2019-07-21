#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib> 

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "LSystem.h"


void eventHandling();
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

float angle1 = pi * 25.0f / 360.0f;
std::string axiom1 = "C";
std::map<char, std::string> rules1 = {
	{'A', "AA"}, {'C', "A+[[C]-C]-A[-AC]+C"}
	, {'[', "["}, {']', "]"}, {'-', "-"}, {'+', "+"}
};

float angle2 = pi * 45.0f / 360.0f;
std::string axiom2 = "B";
std::map<char, std::string> rules2 = {
	{'A', "AA"}, {'B', "A[-B]+B"}
	, {'[', "["}, {']', "]"}, {'-', "-"}, {'+', "+"}
};

std::vector<LSystem> lSystems = { LSystem("Wheat", angle1, axiom1, rules1), LSystem("Tree", angle2, axiom2, rules2) };

LSystem ls = lSystems[0];

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

		for (std::vector<sf::Vertex> line : ls.lines) {
			sf::Vector2i mousePos = sf::Mouse::getPosition();

			setColour(line);
			state.transform.translate(-570 + mousePos.x, mousePos.y);
			window.draw(line.data(), line.size(), sf::LinesStrip, state);
			state.transform.translate(570 - mousePos.x, -mousePos.y);
		}
		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			if (mouseIsPressed) {
				tex.update(window);
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Chose L-System Settings");
		ImGui::InputInt("Step Size", &step);
		ImGui::SliderFloat("Brush Size", &size, 0.0f, 1.0f);


		if (ImGui::CollapsingHeader("L-System")) {
			for (size_t i = 0; i < lSystems.size(); i++) {
				if (ImGui::Button(lSystems[i].name.data())) {
					ls = lSystems[i];
				}
			}
		}

		if (ImGui::CollapsingHeader("Brush")) {
			ImGui::ImageButton(sprite, sf::Vector2f(50, 50), 1);
			ImGui::SameLine();
			ImGui::ImageButton(sprite, sf::Vector2f(50, 50), 1);
			ImGui::SameLine();
			ImGui::ImageButton(sprite, sf::Vector2f(50, 50), 1);
		}

		if (oldStep != step) {
			ls.createString(step);
			ls.createLines(size);
			oldStep = step;
		}
		if (oldSize != size) {
			ls.createLines(size);
			oldSize = size;
		}

		ImGui::End();

		ImGui::SFML::Render(window);

		window.display();
	}

	return 0;
}

void setColour(std::vector<sf::Vertex>& line)
{
	for (size_t i = 0; i < line.size(); i++) {
		line[i].color.r = sf::Color::Red.r * (1 + line[i].position.y / SCREEN_HEIGHT) - sf::Color::Green.r * line[i].position.y / SCREEN_HEIGHT;
		line[i].color.g = sf::Color::Red.g * (1 + line[i].position.y / SCREEN_HEIGHT) - sf::Color::Green.g * line[i].position.y / SCREEN_HEIGHT;
		line[i].color.b = sf::Color::Red.b * (1 + line[i].position.y / SCREEN_HEIGHT) - sf::Color::Green.b * line[i].position.y / SCREEN_HEIGHT;
	}
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
			ls.createLines(size);
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