#pragma once
class SubTestClass
{
public:
	SubTestClass();
	SubTestClass(int a);
	~SubTestClass();

	int number;
};



inline SubTestClass::SubTestClass()
{
	number = 0;
}

inline SubTestClass::SubTestClass(int a) :
	number(a)
{
}


inline SubTestClass::~SubTestClass()
{
}
