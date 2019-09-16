#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"
namespace fs = std::experimental::filesystem;

void func(int a, void(*f)(int, float)) {
	f(a, 2.0);
}

void print(int a, float b) {
	std::cout << a * b << std::endl;
}

int main() {
	int x = 2;
	TestClass::increaseNumber(x);
	TestClass::increaseNumber(x);
	func(TestClass::getNumber(), print);
}
