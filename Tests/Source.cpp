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

	//Create image and save it as a png
	sf::Image img;
	int width = 1200;
	int height = 800;
	img.create(width, height, sf::Color::Red);
	img.saveToFile("test.png");

	//create input stream for the ng and output stream for the file we want to write to
	std::ifstream istrm("test.png", std::ios::in | std::ios::binary);
	std::ofstream ostrm("MyFile.jns", std::ios::out | std::ios::binary);

	ostrm.write((char*)&width, sizeof(int));
	ostrm.write((char*)&height, sizeof(int));

	istrm.seekg(0, std::ios::end);
	std::streampos end = istrm.tellg();
	istrm.seekg(0, std::ios::beg);

	//Make buffer with the right size
	std::vector<char> buffer;
	buffer.resize(end / sizeof(char));

	//read data into the buffer and write it to the file again
	istrm.read((char*)buffer.data(), end / sizeof(char));
	ostrm.write((char*)buffer.data(), end / sizeof(char));
	
	istrm.close();
	ostrm.close();

	//reading the file back into an image
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

	buffer.resize((end - begin) / sizeof(char));
	newistrm.read((char*)buffer.data(), (end - begin) / sizeof(char));

	newistrm.close();

	image.loadFromMemory(buffer.data(), new_width * new_height);
	image.saveToFile("test2.png");

	sf::IntRect rect(0, 0, 300, 800);
	sf::Texture tex;
	tex.create(new_width, new_height);
	tex.loadFromImage(image);
	sf::Sprite sprite;
	sprite.setTexture(tex);
	sprite.setTextureRect(rect);

	sf::RenderTexture rtex;
	rtex.create(300, 800);
	rtex.draw(sprite);
	rtex.display();
	rtex.getTexture().copyToImage().saveToFile("test3.png");

}
