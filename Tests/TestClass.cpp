#include "TestClass.h"

TestClass::TestClass()
{
	std::cout << "Constructing Object.\n";
	number = 0;
}

TestClass::TestClass(std::vector<int>& start, int x)
{
	number = x;
	vec = start;
	logging();
}


TestClass::~TestClass()
{
	std::cout << "Deleting Object.\n";
}

void TestClass::logging()
{
	for (auto elem : vec)
		std::cout << elem << " Constructing Object.\n";
}
