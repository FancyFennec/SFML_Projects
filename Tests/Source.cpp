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

std::vector<sf::Uint8> createVector(sf::Image image) {
	sf::Vector2u size = image.getSize();
	int width = size.x;
	int height = size.y;
	std::vector<sf::Uint8> data = {};
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
	
	std::vector<sf::Uint8> a = {};
	sf::Image img;
	int width = 200;
	int height = 200;
	img.create(width, height, sf::Color::Green);
	ostrm.write((char*)&width, sizeof(int));
	ostrm.write((char*)&height, sizeof(int));

	ostrm.write((char*)&img.getPixelsPtr()[0], sizeof(sf::Uint8) * width * height * 4);
	ostrm.close();

	std::ifstream istrm("MyFile.jns", std::ios::in | std::ios::binary);

	int new_width = 0;
	istrm.read((char*)&new_width, sizeof(int));
	int new_height = 0;
	istrm.read((char*)&new_height, sizeof(int));
	

	sf::Image image;
	image.create(new_width, new_height);
	istrm.read((char*)&image.getPixelsPtr()[0], sizeof(sf::Uint8) * new_width * new_height * 4);
	istrm.close();

	image.saveToFile("test.jpg");
	

}
