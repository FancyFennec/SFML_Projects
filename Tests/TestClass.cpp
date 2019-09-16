#include "TestClass.h"

TestClass::TestClass()
{
	number = 0;
}

TestClass::TestClass(int x)
{
	number = x;
}

int TestClass::number = 0;

TestClass::~TestClass()
{
}
