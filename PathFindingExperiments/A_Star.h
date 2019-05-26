#pragma once

#include "EventHandler.h"

// g[i][j] contains the distance from the goal to the point [i][j]
int **g;
//h[i][j] == -1 if it hasn't been added to the active set yet, if it has been added it contains the cost to travel to it
int **h;
std::vector<sf::Vector2i> path;
std::vector<sf::Vector3i> activeVerteces;
sf::Vector3i currentVertex;


//Sceletons
void initialize(sf::Vector2i &start, sf::Vector2i &goal);
void initializeH();
sf::Vector3i nextSelectedVertex();
void computeG(sf::Vector2i goal);
void updateH(sf::Vector2i start);

//Implementation
std::vector<sf::Vector2i> computePath(sf::Vector2i &start, sf::Vector2i &goal)
{
	return path;
}

void initialize(sf::Vector2i &start, sf::Vector2i &goal) {
	computeG(goal);
	updateH(start);
	activeVerteces.push_back(sf::Vector3i(start.x, start.y, 1));

	for (int i = -1; i < 2; i++) 
		for (int j = -1; j < 2; j++)
			if(i != 0 || j != 0)
				if(0 < start.x + i && start.x + i < TILES_WIDTH +1 &&
					0 < start.y + j && start.y + j < TILES_HEIGHT + 1)
				activeVerteces.push_back(sf::Vector3i(start.x + i, start.y + j, -1));

	sf::Vector3i next = nextSelectedVertex();

	//TODO: fix this
	//ptrdiff_t pos = std::find_if(activeVerteces.begin(), activeVerteces.end(), [next](sf::Vector3i vert) { return vert.x == next.x && vert.y == next.y; });
}

sf::Vector3i nextSelectedVertex() {
	std::vector<sf::Vector3i> notVisited(activeVerteces.size());

	 auto it = std::copy_if(activeVerteces.begin(), activeVerteces.end(), notVisited.begin(), [](sf::Vector3i vert) {
		return vert.z == -1; });
	 notVisited.resize(std::distance(notVisited.begin(), it));

	 //TODO: Does this do the right thing?
	currentVertex = *std::min_element(notVisited.begin(), notVisited.end(), [](sf::Vector3i vert) {
		return h[vert.x][vert.y] + g[vert.x][vert.y]; });

	it = std::copy_if(notVisited.begin(), notVisited.end(), notVisited.begin(), [](sf::Vector3i vert) {
		return h[vert.x][vert.y] + g[vert.x][vert.y] == h[currentVertex.x][currentVertex.y] + g[currentVertex.x][currentVertex.y]; });
	notVisited.resize(std::distance(notVisited.begin(), it));

	//TODO: Does this do the right thing?
	currentVertex = *std::min_element(notVisited.begin(), notVisited.end(), [](sf::Vector3i vert) {
		return h[vert.x][vert.y]; });

	return currentVertex;
}

void initializeH() {
	for (int i = 0; i < TILES_WIDTH; i++)
		for (int j = 0; j < TILES_HEIGHT; j++)
			h[i][j] = -1;
}

void computeG(sf::Vector2i goal) {
	for (int i = 0; i < TILES_WIDTH; i++) {
		for (int j = 0; j < TILES_HEIGHT; j++) {
			int x = i - goal.x;
			int y = j - goal.y;

			x = x > 0 ? x : -x;
			y = y > 0 ? y : -y;

			g[i][j] = x > y ? 10 * y + 14 * (x - y) : 10 * x + 14 * (y - x);
		}
	}
}

void updateH(sf::Vector2i vert) {
	int x = vert.x;
	int y = vert.y;

	auto computeSquareNB = [x, y](int r, int u) { return h[x + r][y + u] == -1 ? h[x][y] + 10 : h[x + r][y + u] < h[x][y] + 10 ? h[x + r][y + u] : h[x][y] + 14; };
	auto computeDiamondNB = [x, y](int r, int u) { return h[x + r][y + u] == -1 ? h[x][y] + 14 : h[x + r][y + u] < h[x][y] + 14 ? h[x + r][y + u] : h[x][y] + 14; };



	//Square
	if (x > 0) {
		h[x - 1][y] = computeSquareNB(-1, 0);
	}
	if (x < TILES_WIDTH - 1) {
		h[x + 1][y] = computeSquareNB(1, 0);
	}
	if (y > 0) {
		h[x][y - 1] = computeSquareNB(0, -1);
	}
	if (y < TILES_HEIGHT - 1) {
		h[x][y + 1] = computeSquareNB(0, 1);
	}

	//Diamond
	if (x > 0 && y > 0) {
		h[x - 1][y - 1] = computeDiamondNB(-1, -1);
	}
	if (x < TILES_WIDTH - 1 && y < TILES_HEIGHT - 1) {
		h[x + 1][y + 1] = computeDiamondNB(1, 1);
	}
	if (x > 0 && y < TILES_HEIGHT - 1) {
		h[x -1][y + 1] = computeDiamondNB(-1, 1);
	}
	if (x < TILES_WIDTH - 1 && y > 0) {
		h[x + 1][y - 1] = computeDiamondNB(1, -1);
	}
}