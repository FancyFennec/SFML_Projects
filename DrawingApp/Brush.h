#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <numeric>


class Brush
{
public:
	std::string brushName = "Brush";

	sf::Image image;
	sf::Texture tex;
	sf::Sprite sprite;
	static sf::Color currentColor;
	static sf::Color previousColor;

	float stepsize = 33.0f;
	float brushsize = 0.3f;
	int opacity = 170;
	int flow = 70;
	float pressure = 1.0f;

	bool useSScatter = true;
	bool usePScatter = true;
	bool useAScatter = true;

	float scaterScale = 0.0f;
	float scaterPos = 0.0f;
	float scaterAngle = 0.0f;

	Brush(int brush_width, const char* filePath)
	{
		image.create(brush_width, brush_width);
		if (!image.loadFromFile(filePath)) {
			std::cout << "ERROR! unable to find path to file!" << std::endl;
		}
		initialize();
	}

	ImVec4 getCurrentImColorRGBA() {
		return ImVec4(
			currentColor.r / 255.0f,
			currentColor.g / 255.0f,
			currentColor.b / 255.0f,
			currentColor.a / 255.0f
		);
	}

	ImVec4 getCurrentImColorRGB() {
		return ImVec4(
			currentColor.r / 255.0f,
			currentColor.g / 255.0f,
			currentColor.b / 255.0f,
			1.0f
		);
	}

	ImVec4 getPreviousImColorRGBA() {
		return ImVec4(
			previousColor.r / 255.0f,
			previousColor.g / 255.0f,
			previousColor.b / 255.0f,
			previousColor.a / 255.0f
		);
	}

	ImVec4 getPreviousImColorRGB() {
		return ImVec4(
			previousColor.r / 255.0f,
			previousColor.g / 255.0f,
			currentColor.b / 255.0f,
			1.0f
		);
	}

	void updateGuiColor(static float* color) {
		color[0] = currentColor.r / 255.0f;
		color[1] = currentColor.g / 255.0f;
		color[2] = currentColor.b / 255.0f;
	}

	void setColor(static float* color) {
		currentColor.r = (sf::Uint8)(color[0] * 255);
		currentColor.g = (sf::Uint8)(color[1] * 255);
		currentColor.b = (sf::Uint8)(color[2] * 255);
	}

	void setSize(float brushSize);
	void setSpriteColor(){
		sprite.setColor(sf::Color(
			currentColor.r,
			currentColor.g,
			currentColor.b,
			pressure * flow));
	}

	void resetBrushColor() {
		sprite.setColor(currentColor);
	}

	~Brush() {};

private:
	void initialize();
};

sf::Color Brush::currentColor = sf::Color::Black;
sf::Color Brush::previousColor = sf::Color::Black;

inline void Brush::initialize() {
	tex.create(image.getSize().x, image.getSize().y);
	tex.update(image);
	sprite.setTexture(tex);
	sprite.setScale(sf::Vector2f(brushsize, brushsize));
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

inline void Brush::setSize(float brushSize) {
	this->brushsize = brushSize;
	sprite.setScale(sf::Vector2f(brushSize, brushSize));
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}
