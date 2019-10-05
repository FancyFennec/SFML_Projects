#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"

namespace fs = std::experimental::filesystem;


int main() {

	std::string test = "HalloEcho";
	std::ifstream istrm("test", std::ios::in | std::ios::binary);
	std::ofstream ostrm("test" , std::ios::out | std::ios::binary);


	int testInt = 2;
	ostrm.write((char*)test.data(), sizeof(char) * test.length());
	ostrm.write((char*)"#", sizeof(char));
	ostrm.write((char*)&testInt, sizeof(int));
	ostrm.close();

	std::string result;
	int resultIn;
	char newChar;
	istrm.read((char*)&newChar, sizeof(char));
	while (newChar != '#') {
		result.push_back(newChar);
		istrm.read((char*)&newChar, sizeof(char));
	}
	istrm.read((char*)&resultIn, sizeof(int));

	istrm.close();
	std::cout << result << std::endl;
	std::cout << resultIn << std::endl;

	return 0;
}
