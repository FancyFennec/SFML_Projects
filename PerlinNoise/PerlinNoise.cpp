#include "pNoise.h"
#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

const int sideLength = 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 + 1;

int counter = 0;

const int SCREEN_WIDTH = sideLength;
const int SCREEN_HEIGHT = sideLength;

sf::Event event;
bool gWasPressed = false;
bool dWasPressed = false;
bool sWasPressed = false;
bool qWasPressed = false;
bool pWasPressed = false;

const int MAX_RAND = 100;
std::random_device random;
std::default_random_engine engine(random());
std::uniform_int_distribution<int> uniformDist(-MAX_RAND, MAX_RAND);

double pNoiseValues[SCREEN_WIDTH * SCREEN_HEIGHT];
sf::Uint8 pixels[SCREEN_HEIGHT * SCREEN_WIDTH * 4];
sf::Texture texture = sf::Texture();
sf::Sprite sprite = sf::Sprite(texture);

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Height Map Generator");
PerlinNoise pNoise = PerlinNoise(3);

// Functions
void applyPerlinNoise();
bool isPowerOf2Plus1(int a);
void initialisePixels();
void saveImage();

void drawImage(sf::Sprite &sprite);

int main()
{

	window.setFramerateLimit(60);

	if (!isPowerOf2Plus1(sideLength)) {
		std::cout << "ERROR: Sidelength is not a power of 2 plus 1.\n";
		return -1;
	}

	srand(time(NULL));
	texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite sprite(texture);


	initialisePixels();

	while (window.isOpen())
	{

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		applyPerlinNoise();

		drawImage(sprite);
		window.display();

		saveImage();
	}

	return 0;
}

void applyPerlinNoise()
{
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::Q && !qWasPressed) {
			//qWasPressed = true;

			double max = 0;
			double min = 255;
			double pNoiseValue;
			double value;
			sf::Uint8 pixelValue;

			for (int i = 0; i < SCREEN_HEIGHT; ++i) {
				for (int j = 0; j < SCREEN_WIDTH; ++j) {

					pNoiseValue = pNoise.noise((double)i / ((double)sideLength), (double)j / ((double)sideLength), (double)counter / ((double)1000));
					pNoiseValue = 20 * pNoiseValue;
					pNoiseValue = pNoiseValue - floor(pNoiseValue);
					//pNoiseValue = pNoise.octavePerlin((double)i / ((double)sideLength), (double)j / ((double)sideLength), (double)counter / ((double)100), 10, 0.5);

					if (pNoiseValue > max) {
						max = pNoiseValue;
					}
					if (pNoiseValue < min) {
						min = pNoiseValue;
					}

					pNoiseValues[i * SCREEN_WIDTH + j] = pNoiseValue;
				}
			}

			for (int i = 0; i < SCREEN_HEIGHT; ++i) {
				for (int j = 0; j < SCREEN_WIDTH; ++j) {

					value = 255 * ((pNoiseValues[i * SCREEN_WIDTH + j] - min) / (max - min));
					value = floor(value);
					pixelValue = (sf::Uint8) std::min(255, std::max(0, (int) value));

					pixels[(i * SCREEN_WIDTH + j) * 4] = pixelValue;
					pixels[(i * SCREEN_WIDTH + j) * 4 + 1] = pixelValue;
					pixels[(i * SCREEN_WIDTH + j) * 4 + 2] = pixelValue;
				}
			}

			counter++;
			sWasPressed = false;
		}
	}
}

void drawImage(sf::Sprite &sprite)
{
	texture.update(pixels);
	sprite.setTexture(texture);
	window.draw(sprite);
}

void saveImage()
{
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::S && !sWasPressed) {
			sWasPressed = true;
			std::cout << "Saving Image. \n";
			texture.copyToImage().saveToFile("C:/Users/JonasAllemann/Desktop/Learnings/Blender/RandomTests/perliNoiseTest.png");

			qWasPressed = false;
		}
	}
}

void initialisePixels()
{
	for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH * 4; i += 4) {
		pixels[i] = 0;
		pixels[i + 1] = 0;
		pixels[i + 2] = 0;
		pixels[i + 3] = 255;
	}
}

bool isPowerOf2Plus1(int a) {
	a = a - 1;
	while (a > 0) {
		if (a % 2 == 0) {
			a = a / 2;
		}
		else if (a % 2 == 1 && a == 1) {
			return true;
		}
		else {
			return false;
		}
	}
}
