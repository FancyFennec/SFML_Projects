#include "AStar.h"

void AStar::initializeH()
{
	for (int i = 0; i < TILES_WIDTH; i++)
		for (int j = 0; j < TILES_HEIGHT; j++)
			setH(i, j, -1);
}

//Computes new h values for the vertices surrounding vert
void AStar::updateH(sf::Vector2i& vert) {
	//Set as visited
	if (vert != start) {
		auto it = std::find_if(activeVerteces.begin(), activeVerteces.end(), [vert](StarNode node) {
			return node.child.x == vert.x && node.child.y == vert.y;
		});
		(*it).visit();
	}

	int x = vert.x;
	int y = vert.y;
	if (getH(vert) == -1)
		setH(vert, 0);

	sf::Vector2i offset;

	auto setSquareNB = [this, vert](sf::Vector2i offset) {
		if (getH(vert + offset) == -1) {
			setH(vert + offset, getH(vert) + 10);
			activeVerteces.push_back(StarNode(vert + offset, vert));
		}
		else if (getH(vert + offset) != -2) {
			if (getH(vert + offset) > getH(vert) + 10) {
				setH(vert + offset, getH(vert) + 10);
				auto ind = std::find_if(activeVerteces.begin(), activeVerteces.end(), [vert, offset](StarNode node) {return node.child == vert + offset; });
				(*ind).parent = vert;
			}
		}
	};

	auto setDiamondNB = [this, vert](sf::Vector2i offset) {
		if (getH(vert + offset) == -1) {
			setH(vert + offset, getH(vert) + 14);
			activeVerteces.push_back(StarNode(vert + offset, vert));
		}
		else if (getH(vert + offset) != -2) {
			if (getH(vert + offset) > getH(vert) + 14) {
				setH(vert + offset, getH(vert) + 14);
				auto ind = std::find_if(activeVerteces.begin(), activeVerteces.end(), [vert, offset](StarNode node) {return node.child == vert + offset; });
				(*ind).parent = vert;
			}
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

StarNode AStar::nextVertex() {
	std::vector<StarNode> potentialMins;

	for (StarNode node : activeVerteces) {
		if (!node.visited)
			potentialMins.push_back(node);
	}

	StarNode min;

	if (potentialMins.size() != 0) {
		hasNext = true;

		// Find a vertex with minimum f value
		min = *std::min_element(potentialMins.begin(), potentialMins.end(), [this](StarNode vert1, StarNode vert2) {
			return getH(vert1.child) + getG(vert1.child) < getH(vert2.child) + getG(vert2.child); });

		// Find all vertices with minimum f value
		auto it = std::copy_if(potentialMins.begin(), potentialMins.end(), potentialMins.begin(), [this, min](StarNode vert) {
			return getH(vert.child) + getG(vert.child) == getH(min.child) + getG(min.child); });
		potentialMins.resize(std::distance(potentialMins.begin(), it));

		// Find a vertex with minimal g value out of subset
		min = *std::min_element(potentialMins.begin(), potentialMins.end(), [this](StarNode vert1, StarNode vert2) {
			return getG(vert1.child) < getG(vert2.child); });
	}
	else {
		hasNext = false;
	}

	return min;
}

sf::Vector2i AStar::step()
{
	StarNode next = nextVertex();
	updateH(next.child);

	return next.child;
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

std::vector<sf::Vector2i> AStar::computePath()
{
	path.push_back(start);
	sf::Vector2i next = step();

	if (next == goal) {
		path.push_back(next);
		return path;
	}
	else {
		while (hasNext) {
			next = step();
			if (next == goal) {
				//TODO:calculate path and return it
				std::cout << "Shortest path calculated.\n";
				path.push_back(goal);
				StarNode backTracking = *std::find_if(activeVerteces.begin(), activeVerteces.end(), [this](StarNode node) {
					return node.child.x == goal.x && node.child.y == goal.y;
				});
				while (backTracking.parent.x != start.x || backTracking.parent.y != start.y) {
					backTracking = *std::find_if(activeVerteces.begin(), activeVerteces.end(), [backTracking](StarNode node) {
						return node.child.x == backTracking.parent.x && node.child.y == backTracking.parent.y;
					});
					path.push_back(backTracking.child);
				}
				return path;
			}
		}
		std::cout << "Could nto calculate shortest path.\n";
		return path;
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

AStar::AStar(sf::Vector2i& start, sf::Vector2i& goal):
	start(start),
	goal(goal)
{
	h.clear();
	g.clear();
	g.resize(TILES_WIDTH * TILES_HEIGHT);
	h.resize(TILES_WIDTH * TILES_HEIGHT);

	currentVertex = start;
	activeVerteces.clear();
	computeG(goal);
	initializeH();
	updateH(start);
}

AStar::~AStar()
{
}


