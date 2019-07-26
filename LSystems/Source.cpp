#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib> 

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "LSystem.h"
#include "json.hpp"

using json = nlohmann::json;


void eventHandling();
void setColour(std::vector<sf::Vertex>& line);
void loadLSystems(const char* filename);
void createWindow();
void settingsWindow();
void saveLSystems(const char* filename);

sf::Clock deltaClock;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const char* filename = "lsystems.json";
float pi = 3.14159265358979323846f;

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "L-Systems");
sf::RenderStates state;
sf::Event event;

sf::Texture currentTexture;
std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;
sf::Sprite sprite;
sf::Vector2i mousePos;

bool mouseIsHeld = false;
bool ctrlIsPressed = false;
bool isRandom = false;

int step = 1;
int oldStep = 0;
float size = 1.0f;
float oldSize = 1.0f;

static char name[128] = "";
static char axiom[128] = "";
static float angle = 0;

static char key[128] = "";
static char value[128] = "";

std::map<char, std::string> rules;

std::vector<LSystem> lSystems;
LSystem ls;

std::vector<std::vector<sf::Vertex>> lines = {};

int main() {
	loadLSystems(filename);
	ls = lSystems[0];

	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	currentTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	textureBuffer.reserve(10);
	textureBuffer.push_back(currentTexture);
	textureIter = textureBuffer.begin();
	sprite.setTexture(currentTexture);

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

		sprite.setTexture(currentTexture);
		window.draw(sprite);

		mousePos = sf::Mouse::getPosition();

		for (std::vector<sf::Vertex> line : ls.lines) {
			setColour(line);
			state.transform.translate(-570 + mousePos.x, mousePos.y);
			window.draw(line.data(), line.size(), sf::LinesStrip, state);
			state.transform.translate(570 - mousePos.x, -mousePos.y);
		}
		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
			if (mouseIsHeld) {
				currentTexture.update(window);
			}
		}

		settingsWindow();
		createWindow();

		ImGui::SFML::Render(window);

		window.display();
	}

	saveLSystems(filename);

	return 0;
}

void settingsWindow()
{
	ImGui::SFML::Update(window, deltaClock.restart());

	ImGui::Begin("L-System Settings");
	ImGui::InputInt("Step Size", &step);
	ImGui::SliderFloat("Brush Size", &size, 0.0f, 1.0f);
	if (ImGui::Button("Random")) {
		isRandom = !isRandom;
		ls.isRandom = isRandom;
		ls.createLines(size);
	}

	if (ImGui::CollapsingHeader("Select L-System")) {
		for (size_t i = 0; i < lSystems.size(); i++) {
			if (ImGui::Button(lSystems[i].name.data(), sf::Vector2i(80, 20))) {

				ls = lSystems[i];
				ls.isRandom = isRandom;
				ls.createString(step);
				ls.createLines(size);
			}
			ImGui::SameLine();
			ImGui::Dummy(sf::Vector2i(30, 20));

			ImGui::SameLine();
			ImGui::PushID(i);
			if (ImGui::Button("Delete")) {
				lSystems.erase(lSystems.begin() + i);
			}
			ImGui::PopID();
		}
	}

	if (oldStep != step) {
		if (step >= 0) {
			ls.createString(step);
			ls.createLines(size);
			oldStep = step;
		}
	}
	if (oldSize != size) {
		ls.createLines(size);
		oldSize = size;
	}

	ImGui::End();
}

void createWindow()
{
	ImGui::Begin("Create L-System");

	ImGui::InputText("Name : ", name, sizeof(name));
	ImGui::InputFloat("Angle : ", &angle);
	ImGui::InputText("Axiom : ", axiom, sizeof(axiom));

	if (ImGui::Button("Add new Rule")) rules[key[0]] = value;
	ImGui::InputText("Key : ", key, sizeof(key));
	ImGui::InputText("Value : ", value, sizeof(value));

	if (ImGui::CollapsingHeader("Show Rules")) {
		int i = 0;
		for (auto &rule : rules) {
			char key[1] = { rule.first };

			ImGui::Text(key);
			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();
			ImGui::Text(rule.second.data());
			ImGui::SameLine();
			ImGui::PushID(i);

			if (ImGui::Button("Delete")) rules.erase(rule.first);
			ImGui::PopID();
			i++;
		}
	}
	if (ImGui::Button("Add L-System")) {
		std::string lName(name);
		std::string lAxiom(axiom);
		if (!lName.empty() && !lAxiom.empty() && angle != 0.0f && !rules.empty()) {
			lSystems.push_back(LSystem(lName, angle * pi / 180.0f, lAxiom, rules));
		}
		else {
			std::cout << "Error!!! You have missed a field." << std::endl;
		}
	}

	ImGui::End();
}

void loadLSystems(const char* filename)
{
	std::ifstream i(filename);
	json j;
	i >> j;

	for (auto lSys : j["lslist"]) {
		std::string name = lSys["name"].get<std::string>();
		float angle = lSys["angle"].get<float>();
		std::string axiom = lSys["axiom"].get<std::string>();
		std::map<char, std::string> rules;

		for (auto &rule : lSys["rules"].get<json::object_t>()) {
			char key = rule.first[0];
			std::string value = rule.second.get<std::string>();

			rules[key] = value;
		}

		lSystems.push_back(LSystem(name, angle, axiom, rules));
	}
}

void saveLSystems(const char* filename)
{
	json jList;

	for (auto sys : lSystems) {
		json jSys;
		jSys["name"] = sys.name;
		jSys["angle"] = sys.angle;
		jSys["axiom"] = sys.axiom;

		jSys["rules"] = {};
		for (auto &pair : sys.rules) {
			std::string key = { pair.first };
			std::string value = pair.second;

			jSys["rules"][key]=value;
		}

		jList["lslist"].push_back(jSys);
	}

	std::ofstream o(filename);
	o << jList << std::endl;
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
			mouseIsHeld = true;
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (textureIter != textureBuffer.end() - 1) {
				auto iter = textureIter + 1;
				while (iter < textureBuffer.end()) {
					textureBuffer.erase(iter);
				}
			}
			if (textureBuffer.size() >= 10) {
				textureBuffer.erase(textureBuffer.begin());
			}
			textureBuffer.push_back(currentTexture);
			textureIter = textureBuffer.end() - 1;
			mouseIsHeld = false;
		}
	}
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Right) {
			ls.createLines(size);
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Q) {
			currentTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::LControl) {
			ctrlIsPressed = true;
		}
	}
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::LControl) {
			ctrlIsPressed = false;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (ctrlIsPressed && event.key.code == sf::Keyboard::Z) {
			if (textureIter != textureBuffer.begin()) --textureIter;
			currentTexture = *textureIter;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (ctrlIsPressed && event.key.code == sf::Keyboard::Y) {
			if (textureIter != textureBuffer.end() - 1) ++textureIter;
			currentTexture = *textureIter;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			window.close();
		}
	}
}