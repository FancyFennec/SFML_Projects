#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <vector> 
#include <filesystem>
#include "Source.h"
#include "TestClass.h"

namespace fs = std::experimental::filesystem;

unsigned int width = 800;

sf::RenderWindow window;
sf::Event event;

sf::Vector3f lightPos(800, 0, 800);

sf::Image bgNormalImg;
sf::Color bgNormalCol(127, 127, 255, 255);
sf::Texture bgNomalTex;
sf::Sprite bgNormalSprite;

sf::Image fgNormalImg;
sf::Color fgNormalCol(255, 127, 127, 100);
sf::Texture fgNormalTex;
sf::Sprite fgNormalSpr;

sf::Texture normalMapTex;
sf::Sprite normalMapSprite;

sf::Image layerImg;
sf::Color layerCol(255, 127, 127, 100);
sf::Texture layerTex;
sf::Sprite layerSpr;

sf::RenderTexture rTex;
sf::Shader normalBlendingShader;
sf::Shader renderShader;
sf::RenderStates state(&normalBlendingShader);
sf::RenderStates windowRState(&renderShader);

int main() {

	bgNormalImg.create(width, width, bgNormalCol);
	bgNomalTex.create(width, width);
	bgNomalTex.loadFromImage(bgNormalImg);
	bgNormalSprite.setTexture(bgNomalTex);

	fgNormalImg.create(width, width, fgNormalCol);
	fgNormalTex.create(width, width);
	fgNormalTex.loadFromImage(fgNormalImg);
	fgNormalSpr.setTexture(fgNormalTex);

	layerImg.create(width, width, layerCol);
	layerTex.create(width, width);
	layerTex.loadFromImage(layerImg);

	rTex.create(width, width);
	
	if (normalBlendingShader.loadFromFile("./normal_blending.frag", sf::Shader::Fragment)) {
		std::cout << "Shader loaded" << std::endl;
	}
	else {
		std::cout << "!!!Shader loading failed" << std::endl;
		return -1;
	}
	if (renderShader.loadFromFile("./render_shader.frag", sf::Shader::Fragment)) {
		std::cout << "Shader loaded" << std::endl;
	}
	else {
		std::cout << "!!!Shader loading failed" << std::endl;
		return -1;
	}

	normalBlendingShader.setUniform("texture1", bgNomalTex);
	normalBlendingShader.setUniform("texture2", fgNormalTex);

	rTex.draw(bgNormalSprite, state);
	rTex.display();
	sf::Color col = rTex.getTexture().copyToImage().getPixel(0, 0);


	window.create(sf::VideoMode(width, width), "TestWindow");

	normalMapTex.loadFromFile("./normalColorPicker.png");
	normalMapSprite.setTexture(normalMapTex);

	renderShader.setUniform("normalMap", normalMapTex);
	renderShader.setUniform("layerTex", layerTex);

	sf::Vector3f up(0, -30, 0);
	sf::Vector3f right(30, 0, 0);

	while (window.isOpen()) {
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();

			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Up)
				{
					lightPos += up;
				} 
				if (event.key.code == sf::Keyboard::Down)
				{
					lightPos -= up;
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					lightPos += right;
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					lightPos -= right;
				}
			}
		}

		renderShader.setUniform("lightPos", lightPos);

		window.clear(sf::Color::Black);
		window.draw(normalMapSprite, windowRState);
		window.display();
	}

	return 0;
}
