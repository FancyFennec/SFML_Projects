#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"

namespace fs = std::experimental::filesystem;

unsigned int width = 400;

int main() {

	sf::Image backgroundImg;
	sf::Color backgroundCol(127, 127, 255);
	sf::Texture backgroundTex;
	sf::Sprite backgroundSpr;

	backgroundImg.create(width, width, backgroundCol);
	backgroundTex.create(width, width);
	backgroundTex.loadFromImage(backgroundImg);
	backgroundSpr.setTexture(backgroundTex);

	sf::Image normalImg;
	sf::Color normalCol(255, 127, 127, 100);
	sf::Texture normalTex;
	sf::Sprite normalSpr;

	normalImg.create(width, width, normalCol);
	normalTex.create(width, width);
	normalTex.loadFromImage(normalImg);
	normalSpr.setTexture(normalTex);

	sf::RenderTexture rTex;
	rTex.create(width, width);
	sf::Shader shader;
	sf::RenderStates state(&shader);
	
	if (shader.loadFromFile("./fragment_shader.frag", sf::Shader::Fragment)) {
		std::cout << "Shader loaded" << std::endl;
	}
	else {
		std::cout << "!!!Shader loading failed" << std::endl;
		return -1;
	}

	shader.setUniform("texture1", backgroundTex);
	shader.setUniform("texture2", normalTex);

	rTex.draw(backgroundSpr, state);
	rTex.display();
	sf::Color col = rTex.getTexture().copyToImage().getPixel(0, 0);

	std::cout << (int)col.r << std::endl;
	std::cout << (int)col.g << std::endl;
	std::cout << (int)col.b << std::endl;
	std::cout << (int)col.a << std::endl;

	return 0;
}
