#include "TestClass.h"

void TestClass::setArray() {
	std::cout << "Setting Array.\n";
	for (int i = 0; i < 3; i++)
		array[i] = 0;
}

TestClass::TestClass()
{
	std::cout << "Constructing Object.\n";
	number = 0;
	array = new int[3]();
}

TestClass::TestClass(int i)
{
	std::cout << "Constructing Object.\n";
	number = i;
	array = new int[3]();
}


TestClass::~TestClass()
{
	//delete[] array;
	std::cout << "Deleting Object.\n";
}
