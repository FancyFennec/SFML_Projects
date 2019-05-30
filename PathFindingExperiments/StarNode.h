#pragma once
#include <SFML/Graphics.hpp>

class StarNode {
public:

	sf::Vector2i child;
	sf::Vector2i parent;
	bool visited;

	StarNode(sf::Vector2i child, sf::Vector2i parent) :
		child(child),
		parent(parent),
		visited(false) {}

	void visit() {
		visited = true;
	}

	~StarNode() {}
};