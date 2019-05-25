#pragma once

#include "EventHandler.h"

void computeG(sf::Vector2i goal);
void updateH(sf::Vector2i start);

int **g;
int **h;

std::vector<sf::Vector2i> computePath() 
{
	return std::vector<sf::Vector2i>();
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

void updateH(sf::Vector2i start) {
	int x = start.x;
	int y = start.y;

	auto computeSquareNB = [](int r, int u, int &x, int &y) { return h[x + r][y + u] == -1 ? h[x][y] + 10 : h[x + r][y + u] < h[x][y] + 10 ? h[x + r][y + u] : h[x][y] + 14; };
	auto computeDiamondNB = [](int r, int u, int &x, int &y) { return h[x + r][y + u] == -1 ? h[x][y] + 14 : h[x + r][y + u] < h[x][y] + 14 ? h[x + r][y + u] : h[x][y] + 14; };

	//Square
	if (x > 0) {
		h[x - 1][y] = computeSquareNB(-1, 0, x, y);
	}
	if (x < TILES_WIDTH - 1) {
		h[x + 1][y] = computeSquareNB(1, 0, x, y);
	}
	if (y > 0) {
		h[x][y - 1] = computeSquareNB(0, -1, x, y);
	}
	if (y < TILES_HEIGHT - 1) {
		h[x][y + 1] = computeSquareNB(0, 1, x, y);
	}

	//Diamond
	if (x > 0 && y > 0) {
		h[x - 1][y - 1] = computeDiamondNB(-1, -1, x, y);
	}
	if (x < TILES_WIDTH - 1 && y < TILES_HEIGHT - 1) {
		h[x + 1][y + 1] = computeDiamondNB(1, 1, x, y);
	}
	if (x > 0 && y < TILES_HEIGHT - 1) {
		h[x -1][y + 1] = computeDiamondNB(-1, 1, x, y);
	}
	if (x < TILES_WIDTH - 1 && y > 0) {
		h[x + 1][y - 1] = computeDiamondNB(1, -1, x, y);
	}
}