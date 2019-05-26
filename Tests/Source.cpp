#include <iostream>
#include <vector>
#include <algorithm>
#include "TestClass.h"
#include "FunctionTest.h"


int main() {

	int a = 2;

	std::vector <TestClass> vec;

	vec.push_back(TestClass(1));
	vec.push_back(TestClass(2));
	vec.push_back(TestClass(1));
	vec.push_back(TestClass(3));
	vec.push_back(TestClass(2));
	vec.push_back(TestClass(1));

	std::vector <TestClass> filteredVec(vec.size());

	/*vec.erase(std::remove_if(vec.begin(), vec.end(), 
		[](TestClass x) {return (x.number == 2); }), vec.end());*/
	auto it = std::copy_if(vec.begin(), vec.end(), filteredVec.begin(), [a](TestClass elem) {
		return elem.number == a; });

	filteredVec.resize(std::distance(filteredVec.begin(), it));

	std::vector<TestClass>::iterator result = std::min_element(vec.begin(), vec.end(), [](TestClass elem1, TestClass elem2) {
		return elem1.number < elem2.number; });

	std::cout << "Testing vector filtering." << "\n";

	std::cout << std::distance(vec.begin(), result) << "\n";

	std::vector<TestClass>::iterator iter = std::find_if(vec.begin(), vec.end(), [](TestClass elem) {
		return elem.number == 3; });

	(*iter).number = 2;

	for(auto elem : vec)
		std::cout << "Logging filtered elements: " << "Count: " << elem.count << " Saved Number: " << elem.number << "\n";

	std::cout << "Looking at returned iterator." << "\n";
	std::cout << (*iter).number << "\n";

	std::cin >> a;
	return 0;
}