#pragma once

#include "EventHandler.h"
#include "StarNode.h"

class AStar
{
public:
	sf::Vector2i currentVertex;
	std::vector<StarNode> activeVerteces;

	std::vector<sf::Vector2i> path;
	std::vector<sf::Vector2i> computePath();

	int getH(int x, int y);
	int getH(const sf::Vector2i& vec);
	void setH(int x, int y, int value);
	void setH(sf::Vector2i vec, int value);

	int getG(int x, int y);
	int getG(const sf::Vector2i& vec);
	void setG(int x, int y, int value);
	void setG(sf::Vector2i vec, int value);

	AStar(sf::Vector2i& start, sf::Vector2i& goal);
	~AStar();

private:
	std::vector<int> h;
	std::vector<int> g;
	sf::Vector2i start;
	sf::Vector2i goal;

	bool hasNext;

	void initializeH();
	void updateH(sf::Vector2i& start);
	StarNode nextVertex();
	sf::Vector2i step();
	void computeG(sf::Vector2i& goal);
};

