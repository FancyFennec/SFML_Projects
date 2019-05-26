#include "TestClass.h"

int TestClass::count = 0;

TestClass::TestClass()
{
	number = count;
	count++;
}

TestClass::TestClass(int i)
{
	number = i;
	count++;
}


TestClass::~TestClass()
{
}
