#pragma once

#include "EventHandler.h"

// g[i][j] contains the distance from the goal to the point [i][j]
int g[TILES_WIDTH][TILES_WIDTH];
//h[i][j] == -1 if it hasn't been added to the active set yet, if it has been added it contains the cost to travel to it
int h[TILES_WIDTH][TILES_WIDTH];
std::vector<sf::Vector2i> path;
std::vector<sf::Vector3i> activeVerteces;
sf::Vector3i currentVertex;


//Sceletons
void initialize(sf::Vector2i &start, sf::Vector2i &goal);
sf::Vector3i step(sf::Vector2i &currentVertex);
sf::Vector3i step(sf::Vector3i &currentVertex);
void initializeH();
sf::Vector3i findClosestVertex();
sf::Vector3i nextVertex();
void computeG(sf::Vector2i goal);
void updateH(sf::Vector2i start);

//Implementation
std::vector<sf::Vector2i> computePath(sf::Vector2i &start, sf::Vector2i &goal)
{
	int previousSize = activeVerteces.size();

	initialize(start, goal);
	sf::Vector3i currentVertex = step(start);

	while (currentVertex.x != goal.x || currentVertex.y != goal.y) {
		if (activeVerteces.size() == previousSize) {
			std::cout << "Could not compute shortest path with A*.\n";
			return std::vector<sf::Vector2i>();
		}
		else {
			currentVertex = step(currentVertex);
		}
	}

	return path;
}

sf::Vector3i step(sf::Vector2i &currentVertex) {
	updateH(currentVertex);
	return  nextVertex();
}

sf::Vector3i step(sf::Vector3i &currentVertex) {
	updateH(sf::Vector2i(currentVertex.x, currentVertex.y));
	return  nextVertex();
}

void initialize(sf::Vector2i &start, sf::Vector2i &goal) {
	computeG(goal);
	initializeH();
	activeVerteces.push_back(sf::Vector3i(start.x, start.y, 1));
}

sf::Vector3i nextVertex() {
	// Find the next value and set its z value to 1
	sf::Vector3i next = findClosestVertex();
	std::vector<sf::Vector3i>::iterator pos = std::find_if(activeVerteces.begin(), activeVerteces.end(), [next](sf::Vector3i vert) {
		return vert.x == next.x && vert.y == next.y; });
	(*pos).z = 1;

	return (*pos);
}

// Finds the next selected vertex out of all active verteces
sf::Vector3i findClosestVertex() {
	std::vector<sf::Vector3i> notVisited(activeVerteces.size());

	// Find all active vertices that have not been visited yet
	auto it = std::copy_if(activeVerteces.begin(), activeVerteces.end(), notVisited.begin(), [](sf::Vector3i vert) {
		return vert.z == -1; });
	notVisited.resize(std::distance(notVisited.begin(), it));

	// Find the one with minimum f value
	currentVertex = *std::min_element(notVisited.begin(), notVisited.end(), [](sf::Vector3i vert1, sf::Vector3i vert2) {
		return h[vert1.x][vert1.y] + g[vert1.x][vert1.y] < h[vert2.x][vert2.y] + g[vert2.x][vert2.y]; });

	// Find all with minimum f value
	it = std::copy_if(notVisited.begin(), notVisited.end(), notVisited.begin(), [](sf::Vector3i vert) {
		return h[vert.x][vert.y] + g[vert.x][vert.y] == h[currentVertex.x][currentVertex.y] + g[currentVertex.x][currentVertex.y]; });
	notVisited.resize(std::distance(notVisited.begin(), it));

	// Find one with minimal h value
	currentVertex = *std::min_element(notVisited.begin(), notVisited.end(), [](sf::Vector3i vert1, sf::Vector3i vert2) {
		return h[vert1.x][vert1.y] + g[vert1.x][vert1.y] < h[vert2.x][vert2.y] + g[vert2.x][vert2.y]; });

	return currentVertex;
}

// Initializes all h values to -1
void initializeH() {
	for (int i = 0; i < TILES_WIDTH; i++)
		for (int j = 0; j < TILES_HEIGHT; j++)
			h[i][j] = -1;
}

//Computes all the distance values g
void computeG(sf::Vector2i goal) {
	for (int i = 0; i < TILES_WIDTH; i++) {
		for (int j = 0; j < TILES_HEIGHT; j++) {
			int x = i - goal.x;
			int y = j - goal.y;

			x = x > 0 ? x : -x;
			y = y > 0 ? y : -y;

			g[i][j] = x > y ? 14 * y + 10 * (x - y) : 14 * x + 10 * (y - x);
		}
	}
}

//Computes new h values for verteces surrounding vert
void updateH(sf::Vector2i vert) {
	int x = vert.x;
	int y = vert.y;

	auto computeSquareNB = [x, y](int r, int u) { return h[x + r][y + u] == -1 ? h[x][y] + 10 : h[x + r][y + u] < h[x][y] + 10 ? h[x + r][y + u] : h[x][y] + 10; };
	auto computeDiamondNB = [x, y](int r, int u) { return h[x + r][y + u] == -1 ? h[x][y] + 14 : h[x + r][y + u] < h[x][y] + 14 ? h[x + r][y + u] : h[x][y] + 14; };

	//Square
	if (x > 0) {
		if(h[x - 1][y] == -1)
			activeVerteces.push_back(sf::Vector3i(x - 1, y, -1));
		h[x - 1][y] = computeSquareNB(-1, 0);
	}
	if (x < TILES_WIDTH - 1) {
		if (h[x + 1][y] == -1)
			activeVerteces.push_back(sf::Vector3i(x + 1, y, -1));
		h[x + 1][y] = computeSquareNB(1, 0);
	}
	if (y > 0) {
		if (h[x][y - 1] == -1)
			activeVerteces.push_back(sf::Vector3i(x, y - 1, -1));
		h[x][y - 1] = computeSquareNB(0, -1);
	}
	if (y < TILES_HEIGHT - 1) {
		if (h[x][y + 1] == -1)
			activeVerteces.push_back(sf::Vector3i(x, y + 1, -1));
		h[x][y + 1] = computeSquareNB(0, 1);
	}

	//Diamond
	if (x > 0 && y > 0) {
		if (h[x - 1][y - 1] == -1)
			activeVerteces.push_back(sf::Vector3i(x - 1, y - 1, -1));
		h[x - 1][y - 1] = computeDiamondNB(-1, -1);
	}
	if (x < TILES_WIDTH - 1 && y < TILES_HEIGHT - 1) {
		if (h[x + 1][y + 1] == -1)
			activeVerteces.push_back(sf::Vector3i(x + 1, y + 1, -1));
		h[x + 1][y + 1] = computeDiamondNB(1, 1);
	}
	if (x > 0 && y < TILES_HEIGHT - 1) {
		if (h[x - 1][y + 1] == -1)
			activeVerteces.push_back(sf::Vector3i(x - 1, y + 1, -1));
		h[x -1][y + 1] = computeDiamondNB(-1, 1);
	}
	if (x < TILES_WIDTH - 1 && y > 0) {
		if (h[x - 1][y - 1] == -1)
			activeVerteces.push_back(sf::Vector3i(x - 1, y - 1, -1));
		h[x + 1][y - 1] = computeDiamondNB(1, -1);
	}
}