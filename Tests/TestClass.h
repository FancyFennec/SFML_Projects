#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

class TestClass
{
public:

	int number;
	int *array;

	void setArray();

	TestClass();
	TestClass(int i);
	~TestClass();
};

