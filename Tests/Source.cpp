#include <iostream>
#include <thread> 

int x = 0;

void foo() {
	while (true) {
		x++;
		if (x % 100000 == 0) {
			std::cout << "Foo" << std::endl;
		}
	}
}

void bar() {
	while (true) {
		x++;
		if (x % 1000000 == 0) {
			std::cout << "Boo" << std::endl;
		}
	}
}

int main() {
	std::thread first(foo);
	std::thread second(bar);

	first.join();                // pauses until first finishes
	second.join();

	std::cout << x << std::endl;

	return 0;
}