#include "AStar.h"

void AStar::initializeH()
{
	for (int i = 0; i < TILES_WIDTH; i++)
		for (int j = 0; j < TILES_HEIGHT; j++)
			setH(i, j, -1);
}

//Computes new h values for the vertices surrounding vert
void AStar::updateH(sf::Vector2i& vert) {
	int x = vert.x;
	int y = vert.y;
	if (getH(vert) == -1)
		setH(vert, 0);

	sf::Vector2i offset;

	auto computeSquareNB = [this, vert](sf::Vector2i offset) { 
		return getH(vert + offset) == -1 ? getH(vert) + 10 : getH(vert + offset) < getH(vert) + 10 ? getH(vert + offset) : getH(vert) + 10; };
	auto computeDiamondNB = [this, vert](sf::Vector2i offset) { 
		return getH(vert + offset) == -1 ? getH(vert) + 14 : getH(vert + offset) < getH(vert) + 14 ? getH(vert + offset) : getH(vert) + 14; };
	
	auto setSquareNB = [this, vert, computeSquareNB](sf::Vector2i offset) {
		if (getH(vert + offset) == -1) {
			activeVerteces.push_back(vert + offset);
			setH(vert + offset, computeSquareNB(offset));
		} 
	};
	auto setDiamondNB = [this, vert, computeDiamondNB](sf::Vector2i offset) {
		if (getH(vert + offset) == -1) {
			activeVerteces.push_back(vert + offset);
			setH(vert + offset, computeDiamondNB(offset));
		}
	};

	//Surrounding Square
	if (x > 0) {
		offset = sf::Vector2i(-1,0);
		setSquareNB(offset);
	}
	if (x < TILES_WIDTH - 1) {
		offset = sf::Vector2i(1, 0);
		setSquareNB(offset);
	}
	if (y > 0) {
		offset = sf::Vector2i(0, -1);
		setSquareNB(offset);
	}
	if (y < TILES_HEIGHT - 1) {
		offset = sf::Vector2i(0, +1);
		setSquareNB(offset);
	}

	//Surrounding Diamond
	if (x > 0 && y > 0) {
		offset = sf::Vector2i(-1, -1);
		setDiamondNB(offset);
	}
	if (x < TILES_WIDTH - 1 && y < TILES_HEIGHT - 1) {
		offset = sf::Vector2i(1, 1);
		setDiamondNB(offset);
	}
	if (x > 0 && y < TILES_HEIGHT - 1) {
		offset = sf::Vector2i(-1, 1);
		setDiamondNB(offset);
	}
	if (x < TILES_WIDTH - 1 && y > 0) {
		offset = sf::Vector2i(1, -1);
		setDiamondNB(offset);
	}
}

sf::Vector2i AStar::nextVertex() {
	std::vector<sf::Vector2i> potentialMins(activeVerteces.size());

	// Find a vertex with minimum f value
	sf::Vector2i min = *std::min_element(potentialMins.begin(), potentialMins.end(), [this](sf::Vector2i vert1, sf::Vector2i vert2) {
		return getH(vert1) + getG(vert1) < getH(vert2) + getG(vert2); });

	// Find all vertices with minimum f value
	auto it = std::copy_if(activeVerteces.begin(), activeVerteces.end(), potentialMins.begin(), [this, min](sf::Vector2i vert) {
		return getH(vert) + getG(vert) == getH(min) + getG(min); });
	potentialMins.resize(std::distance(potentialMins.begin(), it));

	// Find a vertex with minimal h value out of subset
	min = *std::min_element(potentialMins.begin(), potentialMins.end(), [this](sf::Vector2i vert1, sf::Vector2i vert2) {
		return getH(vert1) + getG(vert1) < getH(vert2) + getG(vert2); });

	return min;
}

void AStar::computeG(sf::Vector2i& goal) {
	int x;
	int y;

	for (int j = 0; j < TILES_HEIGHT; j++) {
		for (int i = 0; i < TILES_WIDTH; i++) {
			x = i - goal.x;
			y = j - goal.y;

			//Get Absolute value
			x = x >= 0 ? x : -x;
			y = y >= 0 ? y : -y;

			//Set distance
			setG(i, j, x > y ? 14 * y + 10 * (x - y) : 14 * x + 10 * (y - x));
		}
	}
}

int AStar::getH(int x, int y)
{
	return h[y * TILES_WIDTH + x];
}

int AStar::getH(const sf::Vector2i& vec)
{
	return getH(vec.x, vec.y);
}

void AStar::setH(int x, int y, int value)
{
	h[y * TILES_WIDTH + x] = value;
}

void AStar::setH(sf::Vector2i vec, int value)
{
	setH(vec.x, vec.y, value);
}

int AStar::getG(int x, int y)
{
	return g[y * TILES_WIDTH + x];
}

int AStar::getG(const sf::Vector2i& vec)
{
	return getG(vec.x, vec.y);
}

void AStar::setG(int x, int y, int value)
{
	g[y * TILES_WIDTH + x] = value;
}

void AStar::setG(sf::Vector2i vec, int value)
{
	setG(vec.x, vec.y, value);
}

AStar::AStar(sf::Vector2i& start, sf::Vector2i& goal)
{
	h.clear();
	g.clear();
	g.resize(TILES_WIDTH * TILES_HEIGHT);
	h.resize(TILES_WIDTH * TILES_HEIGHT);

	activeVerteces.clear();
	computeG(goal);
	initializeH();
	activeVerteces.push_back(start);
}

AStar::~AStar()
{
}


