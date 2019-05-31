#include "pch.h"
#include "../PathFindingExperiments/AStar.cpp"

TEST(AStar, test_onestep) {
	sf::Vector2i start = sf::Vector2i(1, 1);
	sf::Vector2i goal = sf::Vector2i(2, 1);

	AStar aStar(start, goal);
	aStar.computePath();

	std::vector<StarNode> visitedNodes;

	for (StarNode node : aStar.activeVerteces) {
		if (node.visited)
			visitedNodes.push_back(node);
	}
	
	EXPECT_EQ(aStar.activeVerteces.size(), 11);
	EXPECT_EQ(visitedNodes.size(), 3);
	EXPECT_EQ(aStar.path.size(), 2);
	EXPECT_TRUE(true);
}

TEST(AStar, test_twostep) {
	sf::Vector2i start = sf::Vector2i(1, 1);
	sf::Vector2i goal = sf::Vector2i(3, 1);

	AStar aStar(start, goal);
	aStar.computePath();

	EXPECT_EQ(aStar.activeVerteces.size(), 14);
	EXPECT_TRUE(true);
}

TEST(AStar, test_constructor) {
	sf::Vector2i start = sf::Vector2i(1, 1);
	sf::Vector2i goal = sf::Vector2i(2, 1);

	AStar aStar(start, goal);

	EXPECT_EQ(aStar.getH(1, 1), 0);
	EXPECT_EQ(aStar.getH(2, 1), 10);
	EXPECT_EQ(aStar.getG(2, 1), 0);
	EXPECT_EQ(aStar.getG(2, 2), 10);

	EXPECT_EQ(aStar.currentVertex, start);
	EXPECT_EQ(aStar.activeVerteces.size(), 8);
	EXPECT_TRUE(true);
}

TEST(StarNode, test_constructor) {
	sf::Vector2i start = sf::Vector2i(1, 1);
	sf::Vector2i goal = sf::Vector2i(2, 1);

	StarNode node(start, goal);

	EXPECT_EQ(node.visited, false);
	node.visit();
	EXPECT_EQ(node.visited, true);
	EXPECT_TRUE(true);
}