#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

class TestClass
{
public:

	static int number;
	std::vector<int> vec;

	TestClass();
	TestClass(int x);
	~TestClass();

	static void increaseNumber(int& x) {
		number +=x;
	}

	static int getNumber() {
		return number;
	}

private:
};

