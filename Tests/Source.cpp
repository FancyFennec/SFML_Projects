#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"
#include "msgpack.hpp"
namespace fs = std::experimental::filesystem;

int main() {

	sf::Image img;
	int width = 1200;
	int height = 800;
	img.create(width, height, sf::Color::Red);
	img.saveToFile("test.png");

	std::ifstream istrm("test.png", std::ios::in | std::ios::binary);
	std::ofstream ostrm("MyFile.jns", std::ios::out | std::ios::binary);

	ostrm.write((char*)&width, sizeof(int));
	ostrm.write((char*)&height, sizeof(int));

	istrm.seekg(0, std::ios::end);
	std::streampos end = istrm.tellg();
	istrm.seekg(0, std::ios::beg);

	std::vector<char> buffer;
	buffer.reserve(end / sizeof(char));
	buffer.resize(end / sizeof(char));

	istrm.read((char*)buffer.data(), end / sizeof(char));
	ostrm.write((char*)buffer.data(), end / sizeof(char));
	
	istrm.close();
	ostrm.close();

	std::ifstream newistrm("MyFile.jns", std::ios::out | std::ios::binary);
	int new_width;
	newistrm.read((char*)&new_width, sizeof(int));
	int new_height;
	newistrm.read((char*)&new_height, sizeof(int));

	std::cout << new_width << std::endl;
	std::cout << new_height << std::endl;


	sf::Image image;
	image.create(new_width, new_height);

	std::streampos begin = newistrm.tellg();
	newistrm.seekg(0, std::ios::end);
	end = newistrm.tellg();
	newistrm.seekg(begin);

	buffer.reserve((end - begin)/ sizeof(char));
	buffer.resize((end - begin) / sizeof(char));
	newistrm.read((char*)buffer.data(), (end - begin) / sizeof(char));

	newistrm.close();

	image.loadFromMemory(buffer.data(), new_width * new_height);
	image.saveToFile("test2.png");

}
