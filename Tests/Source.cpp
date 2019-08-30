#include <iostream>
#include <algorithm> 
#include <vector> 

int main() {
	std::vector<int> vec = {1 ,2 ,3};
	vec.reserve(3);
	auto iter = vec.end() - 1;

	vec.erase(iter);
	iter = vec.end() - 1;
	vec.erase(iter);
	iter = vec.end() - 1;
	
	std::cout << *iter << std::endl;

	return 0;
}