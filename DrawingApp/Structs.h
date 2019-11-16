#pragma once

#include <SFML/Graphics.hpp>

enum class COMMAND_TYPE {
	CREATE_LAYER,
	DELETE_LAYER,
	UPDATE_LAYER
};

enum class DrawingState {
	ALPHA,
	NORMAL
};

struct LightSource {
	sf::Vector3f pos;
	sf::Vector3f col;

	LightSource() :
		pos(0.0f, 0.0f, 300.0f),
		col(1.0f, 1.0f, 1.0f)
    {}

	LightSource(sf::Vector3f lightPos, sf::Vector3f lightCol) :
		pos(lightPos),
		col(lightCol) 
    {}

	void movePos(sf::Vector2f vec) {
		pos += sf::Vector3f(vec.x, vec.y, 0);
	}
};

struct Material {
	float shininess;
	float specInt;
	float ambInt;
	float difInt;

	Material() : 
		shininess(32.0f),
		specInt(0.3f),
		ambInt(0.3f),
		difInt(1.0f)
     {}

	Material(float shininess, float specInt, float ambInt, float difInt) :
		shininess(shininess),
		specInt(specInt),
		ambInt(ambInt),
		difInt(difInt) 
    {}
};