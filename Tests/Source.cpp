#include <iostream>
#include <vector>
#include <algorithm>
#include "TestClass.h"


int main() {

	int a;

	std::vector <TestClass> vec;

	vec.push_back(TestClass());
	vec.push_back(TestClass());
	vec.push_back(TestClass());

	vec.erase(std::remove_if(vec.begin(), vec.end(), 
		[](TestClass x) {return (x.number == 2); }), vec.end());

	for(auto elem : vec)
	 std::cout << elem.number << "\n";

	std::cin >> a;
	return 0;
}