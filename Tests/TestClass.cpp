#include "TestClass.h"

TestClass::TestClass()
{
	std::cout << "Constructing Object.\n";
	number = 0;
}

TestClass::TestClass(int i)
{
	std::cout << "Constructing Object.\n";
	number = i;
}


TestClass::~TestClass()
{
	std::cout << "Deleting Object.\n";
}
