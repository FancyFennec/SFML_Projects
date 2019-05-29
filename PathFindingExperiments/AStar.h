#pragma once

#include "EventHandler.h"
#include <map>


struct Comparator
{
	bool operator() (const sf::Vector2i& a, const sf::Vector2i& b) const
	{
		return a.y * TILES_WIDTH + a.x < b.y * TILES_WIDTH + b.x;
	}

};

class AStar
{
public:
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

	sf::Vector2i currentVertex;
	std::vector<sf::Vector2i> activeVerteces;
	std::map<sf::Vector2i, sf::Vector2i, Comparator> mapping;

	void initializeH();
	void updateH(sf::Vector2i& start);
	sf::Vector2i nextVertex();
	void computeG(sf::Vector2i& goal);
};

