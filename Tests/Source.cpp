#include <iostream>

int main() {

	int test = 1 << 3;
	int a = 1 << 1;

	test = a;


	std::cout << test;

	return 0;
	
}