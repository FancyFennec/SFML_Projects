#include <iostream>

int main() {

	int a;
	int b;

	std::cout << "Please enter a number" << std::endl;
	std::cin >> a;
	int counter = 0;
	for (int i = 2; i < a; a++) {
		if (a%i == 0) {
			counter++;
		}
	}

	if (0 != counter) {
		std::cout << "This number is not prime!" << std::endl;
		return -1;
	}
	else {
		std::cout << "This number is prime, hooray" << std::endl;
		return 0;
	}
	
}