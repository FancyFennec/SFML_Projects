#include <iostream>
#include <vector>
#include <algorithm>
#include "TestClass.h"
#include "FunctionTest.h"


int main() {

	int a;

	std::vector <TestClass> vec;

	vec.push_back(TestClass());
	vec.push_back(TestClass());
	vec.push_back(TestClass());

	vec.erase(std::remove_if(vec.begin(), vec.end(), 
		[](TestClass x) {return (x.number == 2); }), vec.end());

	std::cout << "Testing vector element removal." << "\n";

	for(auto elem : vec)
		std::cout << elem.number << "\n";

	std::cout << "Testing function call." << "\n";
	std::cout << "Function returns:" << testFunction() << "\n";

	std::cin >> a;
	return 0;
}