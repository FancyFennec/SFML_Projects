#include "pch.h"
//#include "../PathFindingExperiments/A_Star.h"
//#include "../PathFindingExperiments/AStar.h"
#include "../PathFindingExperiments/AStar.cpp"

//TEST(initialize, test_initializing) {
//	initialize(start, goal);
//	EXPECT_EQ(activeVerteces.size(), 1);
//	sf::Vector3i next = step(activeVerteces[0]);
//	EXPECT_EQ(activeVerteces.size(), 9);
//	auto it = std::copy_if(activeVerteces.begin(), activeVerteces.end(), activeVerteces.begin(), [](sf::Vector3i vert) {
//		return vert.z == 1; });
//	activeVerteces.resize(std::distance(activeVerteces.begin(), it));
//	EXPECT_EQ(activeVerteces.size(), 2);
//	EXPECT_EQ(h[next.x][next.y], 10);
//	EXPECT_EQ(next.z, 1);
//	EXPECT_TRUE(true);
//}
//
//TEST(initializeH, test_initializingH) {
//	initializeH();
//  EXPECT_EQ(h[1][1], -1);
//  EXPECT_TRUE(true);
//}
//
//TEST(updateH, test_updatingH) {
//	initializeH();
//	updateH(start);
//	updateH(goal);
//	EXPECT_EQ(h[1][1], 0);
//	EXPECT_EQ(h[0][1], 10);
//	EXPECT_EQ(h[0][0], 14);
//	EXPECT_EQ(h[0][3], -1);
//	EXPECT_EQ(h[3][1], 20);
//	EXPECT_TRUE(true);
//}
//
//TEST(initializeG, test_initializingG) {
//	computeG(start);
//	EXPECT_EQ(g[0][0], 14);
//	EXPECT_EQ(g[0][1], 10);
//	EXPECT_EQ(g[4][2], 34);
//	EXPECT_TRUE(true);
//}

TEST(AStar, test_constructor) {
	sf::Vector2i start = sf::Vector2i(1, 1);
	sf::Vector2i goal = sf::Vector2i(2, 1);

	AStar aStar(start, goal);

	EXPECT_EQ(aStar.getH(2, 1), -1);
	EXPECT_EQ(aStar.getG(2, 1), 0);
	EXPECT_EQ(aStar.getG(2, 2), 10);
	EXPECT_TRUE(true);
}