#include "pNoise.h"
#include <SFML/Graphics.hpp>
#include <random>
#include <numeric>
#include <iostream>

const int sideLength = 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 + 1;

int counter = 0;

const int SCREEN_WIDTH = sideLength;
const int SCREEN_HEIGHT = sideLength;

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Height Map Generator");
PerlinNoise pNoise = PerlinNoise(3);
sf::Event event;

// Booleans
bool gWasPressed = false;
bool eWasPressed = false;
bool sWasPressed = false;
bool qWasPressed = false;
bool pWasPressed = false;

// Random number generators
const int MAX_RAND = 100;
std::random_device random;
std::default_random_engine engine(random());
std::uniform_int_distribution<int> uniformDist(0, 9);

// Perlin Noise 
double pNoiseValues[SCREEN_WIDTH * SCREEN_HEIGHT];
sf::Uint8 pixels[SCREEN_HEIGHT * SCREEN_WIDTH * 4];
sf::Texture texture = sf::Texture();
sf::Sprite sprite = sf::Sprite(texture);

// Erosion Simulation
std::vector<int> xVec(SCREEN_WIDTH, 0);
std::vector<int> yVec(SCREEN_HEIGHT, 0);

const float pipeConst = 9.81f;
const float sedCap = 1.f;
const float evapConst = 1.f;

float waterLvls[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
float waterFlows[SCREEN_WIDTH * SCREEN_HEIGHT * 4] = { 0 };
float velFields[SCREEN_WIDTH * SCREEN_HEIGHT * 2] = { 0 };
float transpCap[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
float seds[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };

// Functions
void applyPerlinNoise();
void applyErosion();
void computeFlows();
void addRain();
bool isPowerOf2Plus1(int a);
void initialisePixels();
void saveImage();

void drawImage(sf::Sprite &sprite);

int main()
{
	
	//initialize loop vectors
	std::iota(xVec.begin(), xVec.end(), 0);
	std::iota(yVec.begin(), yVec.end(), 0);

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
		applyErosion();

		drawImage(sprite);
		window.display();

		saveImage();
	}

	return 0;
}

void applyErosion() {
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::E && !eWasPressed) {
			std::cout << "Computing Erosion.\n";
			eWasPressed = true;

			std::cout << "Adding Rain.\n";
			addRain();
			std::cout << "Adding Rain.\n";
			computeFlows();
			
			std::cout << "Erosion computed.\n";
			qWasPressed = false;
			sWasPressed = true;
		}
	}
}

void computeFlows()
{
	for (int i : xVec) {
		for (int j : yVec) {

			int L = j * SCREEN_WIDTH + i;
			int R = j * SCREEN_WIDTH + i + 1;
			int B = j * SCREEN_WIDTH + i + 2;
			int T = j * SCREEN_WIDTH + i + 3;

			int hL = (i != 0) ? pNoiseValues[i, j] + waterLvls[i, j] - pNoiseValues[i - 1, j] - waterLvls[i - 1, j] : 0;
			int hR = (i != SCREEN_WIDTH - 1) ? pNoiseValues[i, j] + waterLvls[i, j] - pNoiseValues[i + 1, j] - waterLvls[i + 1, j] : 0;
			int hB = (j != 0) ? pNoiseValues[i, j] + waterLvls[i, j] - pNoiseValues[i, j - 1] - waterLvls[i, j - 1] : 0;
			int hT = (i != SCREEN_HEIGHT - 1) ? pNoiseValues[i, j] + waterLvls[i, j] - pNoiseValues[i, j + 1] - waterLvls[i, j + 1] : 0;

			waterFlows[L] = std::max(0.f, waterFlows[L]) + pipeConst * hL;
			waterFlows[R] = std::max(0.f, waterFlows[R]) + pipeConst * hR;
			waterFlows[B] = std::max(0.f, waterFlows[B]) + pipeConst * hB;
			waterFlows[T] = std::max(0.f, waterFlows[T]) + pipeConst * hT;

			float K = std::min(1.f, waterLvls[i, j] / (waterFlows[L] + waterFlows[R] + waterFlows[B] + waterFlows[T]));

			waterFlows[L] *= K;
			waterFlows[R] *= K;
			waterFlows[B] *= K;
			waterFlows[T] *= K;
		}
	}
}

void addRain()
{
	for (int i : xVec) {
		for (int j : yVec) {
			if (uniformDist(engine) < 5) {
				waterLvls[i, j]++;
			}
		}
	}
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

					//Lava Lamp
					/*pNoiseValue = pNoise.noise((double)i / ((double)sideLength), (double)j / ((double)sideLength), (double)counter / ((double)1000));
					pNoiseValue = 20 * pNoiseValue;
					pNoiseValue = pNoiseValue - floor(pNoiseValue);*/

					//Terrain Generator
					pNoiseValue = pNoise.octavePerlin((double)i / ((double)sideLength), (double)j / ((double)sideLength), (double)counter / ((double)100), 10, 0.5);

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
			eWasPressed = false;
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
			eWasPressed = false;
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
