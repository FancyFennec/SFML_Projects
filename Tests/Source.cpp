#include <iostream>
#include <algorithm> 
#include <vector> 

int main() {
	
	std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
	auto iter = vec.begin() + 2;
	auto lastIter = vec.begin() + 4;

	std::rotate(iter, lastIter, lastIter  + 1);

	for (auto elem : vec) {
		std::cout << elem << std::endl;
	}

	return 0;
}