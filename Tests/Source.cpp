#include "TestClass.h"
#include "FunctionTest.h"
#include <map>


int main() {

	int a = 2;


	std::vector<TestClass> sepp;
	std::vector<TestClass> hans;
	sepp.push_back(TestClass(1));
	sepp.push_back(TestClass(3));
	sepp.push_back(TestClass(2));
	sepp.push_back(TestClass(2));

	hans.resize(sepp.size());

	auto it = std::copy_if(sepp.begin(), sepp.end(), hans.begin(), [](TestClass elem) {return elem.number == 1; });
	hans.resize(std::distance(hans.begin(), it));

	auto min = std::min_element(hans.begin(), hans.end(), [](TestClass elem1, TestClass elem2) {
		return elem1.number < elem2.number;
	});

	auto ele = std::find_if(sepp.begin(), sepp.end(), [min](TestClass elem) {return elem.number == (*min).number; });

	(*ele).number = 2;

	std::cout << hans[0].number << "\n";
	std::cout << (*ele).number << "\n";

	std::cin >> a;
	return 0;
}