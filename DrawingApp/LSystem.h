#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

class LSystem
{
public:
	LSystem();
	LSystem(std::string name, float angle, std::string axiom, std::map<char, std::string> rules);
	~LSystem();

	void createString(unsigned int n);
	void createLines(float size);
	void setAngle(float angle) { this->angle = angle; };

	std::string name;
	float angle;
	std::string axiom;
	std::map<char, std::string> rules;
	bool isRandom;

	std::vector<std::vector<sf::Vertex>> lines = {};

private:
	float pi = 3.14159265358979323846f;

	bool checkString(std::string &input);
	sf::Vector2f rotate(float alpha, sf::Vector2f vec);

	unsigned int step = 0;
	std::string currentLString;
};

LSystem::LSystem() :
	name("please change me"),
	angle(pi / 4.0f),
	isRandom(false),
	axiom("A"),
	rules(std::initializer_list<std::pair<const char, std::string> > { {'A', "AA"} })
{
}

LSystem::LSystem(std::string name, float angle, std::string axiom, std::map<char, std::string> rules):
	name(name),
	angle(angle),
	isRandom(false),
	axiom(axiom),
	rules(rules)
{
}


LSystem::~LSystem()
{
	name.clear();
	axiom.clear();
	rules.clear();
	lines.clear();
	currentLString.clear();
}

inline bool LSystem::checkString(std::string &input)
{
	std::string brackets;

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

inline sf::Vector2f LSystem::rotate(float alpha, sf::Vector2f vec)
{
	return sf::Vector2f(
		cosf(alpha) * vec.x - sinf(alpha) * vec.y,
		sinf(alpha) * vec.x + cosf(alpha) * vec.y
	);
}

inline void LSystem::createString(unsigned int n)
{
	step = n > 0 ? n : 0;
	currentLString = axiom;

	std::string newValue;

	for (int i = 0; i < n; i++) {
		newValue.clear();
		for (char c : currentLString) {
			newValue += rules[c];
		}
		currentLString = newValue;
	}
}

inline void LSystem::createLines(float size)
{
	if (!checkString(currentLString)) {
		std::cout << "ERROR!!! String does not have correct format!" << std::endl;
		return;
	}

	srand(time(0));  // Initialize random number generator.
	float r;
	lines.clear();

	sf::Vector2f currentSegment(0.0f, -size * 400.0f / (float)(pow(2, step)));
	std::vector<sf::Vector2f> lineSegments = { currentSegment };
	std::vector<sf::Vector2f> savedPos = { sf::Vector2f(0.0f, 0.0f) };
	std::vector<sf::Vertex> line = { sf::Vector2f(0.0f, 0.0f) };

	for (char c : currentLString) {

		r = isRandom ? (float)((rand() % 11) + -5) / 20.0f : 0;

		switch (c) {
		case ('A'):
		case ('B'): {
			line.push_back(line.back().position + currentSegment * (1 + r));
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

			break;
		}
		case(']'): {
			if (line.size() > 1) lines.push_back(line);

			currentSegment = lineSegments.back();
			lineSegments.pop_back();

			line = { savedPos.back() };
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
