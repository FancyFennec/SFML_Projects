#pragma once

#include "TestClass.h"
class TopTestClass
{
public:
	TopTestClass();
	TopTestClass(TestClass* test);
	~TopTestClass();

	TestClass* test;
};

