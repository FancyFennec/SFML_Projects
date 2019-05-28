#include "pch.h"
#include "../PathFindingExperiments/A_Star.h"

TEST(initializeH, test_initializing) {
	initializeH();
  EXPECT_EQ(h[1][1], -1);
  EXPECT_TRUE(true);
}

TEST(initializeG, test_initializing) {
	computeG(sf::Vector2i(1,1));
	EXPECT_EQ(g[0][0], 14);
	EXPECT_EQ(g[0][1], 10);
	EXPECT_EQ(g[4][2], 34);
	EXPECT_TRUE(true);
}