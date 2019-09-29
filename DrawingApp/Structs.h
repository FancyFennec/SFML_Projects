#pragma once

#include <SFML/Graphics.hpp>

enum COMMAND_TYPE {
	CREATE_LAYER,
	DELETE_LAYER,
	UPDATE_LAYER
};

enum DrawingState {
	ALPHA,
	NORMAL
};

struct LightSource {
	sf::Vector3f pos;
	sf::Vector3f col;

	LightSource() {
		pos = sf::Vector3f(0.0f, 0.0f, 800.0f);
		col = sf::Vector3f(1.0f, 1.0f, 1.0f);
	}

	LightSource(sf::Vector3f lightPos, sf::Vector3f lightCol) :
		pos(lightPos),
		col(lightCol) {}

	void movePos(sf::Vector2f vec) {
		pos += sf::Vector3f(vec.x, vec.y, 0);
	}
};

struct Material {
	int shininess;
	float specInt;
	float ambInt;
	float difInt;

	Material() {
		shininess = 128;
		specInt = 0.2f;
		ambInt = 0.2f;
		difInt = 1.0f;
	}

	Material(int shininess, float specInt, float ambInt, float difInt) :
		shininess(shininess),
		specInt(specInt),
		ambInt(ambInt),
		difInt(difInt) {}
};