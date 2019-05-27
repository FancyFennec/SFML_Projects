#include "TestClass.h"
#include "FunctionTest.h"


int main() {

	int a = 2;

	std::cout << "Creating Vector.\n";
	std::vector<TestClass> vec;
	vec.reserve(3);

	std::cout << "Push back 1.\n";
	vec.push_back(TestClass(1));
	std::cout << "Push back 2.\n";
	vec.push_back(TestClass(2));
	std::cout << "Push back 3.\n";
	vec.push_back(TestClass(3));

	std::cout << "Setting the Arrays in the vector.\n";
	for (TestClass& elem : vec)
		elem.setArray();

	/*for (TestClass* elem : vec)
		elem->~TestClass();*/

	vec.clear();

	std::cin >> a;
	return 0;
}