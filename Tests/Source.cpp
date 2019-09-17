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

std::vector<int> createVector(sf::Image image) {
	sf::Vector2u size = image.getSize();
	int width = size.x;
	int height = size.y;
	std::vector<int> data = {};
	data.push_back(width);
	data.push_back(height);

	for (int j = 0; j < width; j++) {
		for (int i = 0; i < height; i++) {
			sf::Color col = image.getPixel(i, j);
			data.push_back(col.r);
			data.push_back(col.g);
			data.push_back(col.b);
			data.push_back(col.a);
		}
	}
	return data;
}

int main() {

	std::ofstream ostrm("MyFile.jns", std::ios::out | std::ios::binary);
	
	std::vector<int> a = {};
	sf::Image img;
	img.create(1200, 1200);
	a = createVector(img);
	ostrm.write((char*)&a[0], a.size() * sizeof(int));
	ostrm.close();

	std::ifstream istrm("MyFile.jns", std::ios::in | std::ios::binary);

	size_t begin = istrm.tellg();
	istrm.seekg(0, std::ifstream::end);
	size_t end = istrm.tellg();
	size_t size = (end - begin)/ sizeof(int);
	int buffer = 0;

	istrm.seekg(0, std::ifstream::beg);
	istrm.read((char*)&buffer, sizeof(int));
	int width = buffer;
	istrm.read((char*)&buffer, sizeof(int));
	int height = buffer;

	std::vector<int> b;
	b.reserve(width * height * 4 + 2);
	b.push_back(width);
	b.push_back(height);
	b.resize(width * height * 4 + 2);

	istrm.read((char*)&b[2], sizeof(int) * (size -2));

	sf::Image image;
	image.create((unsigned int)width, (unsigned int)height, (const sf::Uint8*)&b[2]);

	std::cout << b.size() << std::endl;
	istrm.close();

}
