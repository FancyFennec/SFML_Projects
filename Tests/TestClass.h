#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

class TestClass
{
public:

	int number;
	std::vector<int> vec;

	TestClass();
	TestClass(std::vector<int>& start, int x);
	~TestClass();

private:
	void logging();
};

