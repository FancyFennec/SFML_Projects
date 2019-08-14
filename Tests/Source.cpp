#include <iostream>
#include <vector>

int* pntr = new int();

void vectortest() {
	std::vector<int> vec = {};
	vec.push_back(2);

	pntr = &vec.data()[0];

	std::cout << pntr << std::endl;
	std::cout << *pntr << std::endl;
	std::cout << &pntr << std::endl;
}

int main() {

	vectortest();

	std::cout << pntr << std::endl;
	std::cout << *pntr << std::endl;
	std::cout << &pntr << std::endl;

	int a = 4;
	pntr = &a;

	std::cout << pntr << std::endl;
	std::cout << *pntr << std::endl;
	std::cout << &pntr << std::endl;

	pntr = nullptr;
	delete [] pntr;

	return 0;
	
}