#include "pNoise.h"
#include <SFML/Graphics.hpp>
#include <random>
#include <numeric>
#include <iostream>

const int sideLength = 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 + 1;

int counter = 0;

const int SCREEN_WIDTH = sideLength;
const int SCREEN_HEIGHT = sideLength;

sf::RenderWindow mainWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Height Map Generator");
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
sf::Texture background = sf::Texture();
sf::Sprite backgroundImage = sf::Sprite(background);

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
float terrain[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
float seds[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };

// Functions
void applyPerlinNoise();
void applyErosion();
void computeErosion();
void initialiseTerrain();
void computeVelos();
void computeFlows();
void addRain();
float tilt(int &i, int &j);
float abs(float &a, float &b);
bool isPowerOf2Plus1(int a);
void initialisePixels();
void saveImage();

void drawImage(sf::Sprite &sprite);

int main()
{

	//initialize loop vectors
	std::iota(xVec.begin(), xVec.end(), 0);
	std::iota(yVec.begin(), yVec.end(), 0);

	mainWindow.setFramerateLimit(60);

	if (!isPowerOf2Plus1(sideLength)) {
		std::cout << "ERROR: Sidelength is not a power of 2 plus 1.\n";
		return -1;
	}

	srand(time(NULL));
	background.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite sprite(background);


	initialisePixels();

	while (mainWindow.isOpen())
	{

		while (mainWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mainWindow.close();
		}

		mainWindow.clear();

		applyPerlinNoise();
		applyErosion();

		drawImage(sprite);
		mainWindow.display();

		saveImage();
	}

	return 0;
}

void applyErosion() {

	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::E && !eWasPressed) {

			std::cout << "Computing Erosion.\n";
			eWasPressed = true;

			std::cout << "Initialising Terrain.\n";
			initialiseTerrain();
			for (int i = 0; i < 1; i++) {
				//std::cout << "Adding Rain.\n";
				addRain();
				//std::cout << "Computing Flow.\n";
				computeFlows();
				//std::cout << "Computing velocity vectors.\n";
				computeVelos();
				//std::cout << "Computing Erosion and Decomposition.\n";
				computeErosion();
			}
			for (int i = 0; i < 1; i++) {
				computeFlows();
				//std::cout << "Computing velocity vectors.\n";
				computeVelos();
				//std::cout << "Computing Erosion and Decomposition.\n";
				computeErosion();
			}
			
			std::cout << "Erosion computed.\n";
			std::cout << "Updating pixel values.\n";

			int max = -25500;
			int min = 25500;
			float newValue;

			for (int i : xVec) {
				for (int j : yVec) {
					newValue = (int)std::floor(terrain[j * SCREEN_WIDTH + i] + seds[j * SCREEN_WIDTH + i]);

					if (newValue > max) {
						max = pNoiseValues[i * SCREEN_WIDTH + j];
					}
					if (newValue < min) {
						min = pNoiseValues[i * SCREEN_WIDTH + j];
					}

					pNoiseValues[j * SCREEN_WIDTH + i] = newValue;
				}
			}

			
			int value;
			sf::Uint8 pixelValue;

			for (int i = 0; i < SCREEN_HEIGHT; ++i) {
				for (int j = 0; j < SCREEN_WIDTH; ++j) {

					value = 255 * ((pNoiseValues[i * SCREEN_WIDTH + j] - min) / (max - min));
					value = floor(value);
					pixelValue = (sf::Uint8) std::min(255, std::max(0, (int)value));

					pixels[(i * SCREEN_WIDTH + j) * 4] = pixelValue;
					pixels[(i * SCREEN_WIDTH + j) * 4 + 1] = pixelValue;
					pixels[(i * SCREEN_WIDTH + j) * 4 + 2] = pixelValue;
				}
			}

			std::cout << "Pixels updated.\n";

			qWasPressed = false;
			sWasPressed = true;
		}
	}
}

void computeErosion()
{
	float C;
	float Kc = 1.f;
	float Ks = 1.f;
	float Kd = 0.5f;

	for (int i : xVec) {
		for (int j : yVec) {
			
			// Only desolve terrain if there is water
			if (waterLvls[j*SCREEN_WIDTH + i] > 0.1) {
				C = Kc * tilt(i, j) * abs(velFields[j * SCREEN_WIDTH + i], velFields[j * SCREEN_WIDTH + i + 1]);

				terrain[j*SCREEN_WIDTH + i] = terrain[j*SCREEN_WIDTH + i] - Kc * (C - seds[j*SCREEN_WIDTH + i]);
				seds[j*SCREEN_WIDTH + i] = seds[j*SCREEN_WIDTH + i] + Kc * (C - seds[j*SCREEN_WIDTH + i]);

				int newY = (j - (int)velFields[j*SCREEN_WIDTH + i + 1]);
				int newX = (i - (int)velFields[j*SCREEN_WIDTH + i]);

				if ((0 <= newY && newY < SCREEN_HEIGHT) && (0 <=  newX && newX < SCREEN_WIDTH)) {

					/*std::cout << newY << " " << newX << "\n";*/
					seds[j * SCREEN_WIDTH + i] = seds[newY * SCREEN_WIDTH + newX];
				}
			}

			waterLvls[j*SCREEN_WIDTH + i] = j * SCREEN_WIDTH + i * (1 - Kd);
		}
	}
}

float tilt(int &i, int &j) {
	float tL = (i != 0) ? terrain[j * SCREEN_WIDTH + (i - 1)] - terrain[j * SCREEN_WIDTH + i] : 0;
	float tR = (i != SCREEN_WIDTH) ? terrain[j * SCREEN_WIDTH + (i + 1)] - terrain[j * SCREEN_WIDTH + i] : 0;
	float tB = (j != 0) ? terrain[(j - 1) * SCREEN_WIDTH + i] - terrain[j * SCREEN_WIDTH + i] : 0;
	float tT = (j != SCREEN_HEIGHT) ? terrain[(j + 1) * SCREEN_WIDTH + i] - terrain[j * SCREEN_WIDTH + i] : 0;

	return std::max({ sqrt(1. / (1. + tL * tL)), sqrt(1. / (1. + tL * tR)), sqrt(1. / (1. + tL * tB)), sqrt(1. / (1. + tL * tT)) });
}

float abs(float &a, float &b) {
	return sqrt(a * a + b * b);
}

void initialiseTerrain()
{
	float min = 255;
	float max = 0;

	for (int i : xVec) {
		for (int j : yVec) {
			if (pNoiseValues[j * SCREEN_WIDTH + i] < min) {
				min = (float)pNoiseValues[j * SCREEN_WIDTH + i];
			}if (pNoiseValues[j * SCREEN_WIDTH + i] > max) {
				max = (float)pNoiseValues[j * SCREEN_WIDTH + i];
			}
		}
	}

	for (int i : xVec) {
		for (int j : yVec) {
			terrain[j * SCREEN_WIDTH + i] = 255 * ((float)pNoiseValues[j * SCREEN_WIDTH + i] - min) / (max-min);
		}
	}
}

void computeVelos()
{
	float V;
	float Wx;
	float Wy;
	float newWaterlvl;

	int L;
	int R;
	int B;
	int T;

	int fL;
	int fR;
	int fB;
	int fT;

	for (int i : xVec) {
		for (int j : yVec) {
			V = 0;

			L = 4 * (j * SCREEN_WIDTH + i);
			R = 4 * (j * SCREEN_WIDTH + i) + 1;
			B = 4 * (j * SCREEN_WIDTH + i) + 2;
			T = 4 * (j * SCREEN_WIDTH + i) + 3;

			fL = 4 * (j * SCREEN_WIDTH + i + 1);;
			fR = 4 * (j * SCREEN_WIDTH + i - 1) + 1;
			fB = 4 * (j * SCREEN_WIDTH + i + 1) + 2;
			fT = 4 * (j * SCREEN_WIDTH + i - 1) + 3;

			V += (i != 0) ? waterFlows[fL] : 0;
			V += (i != SCREEN_WIDTH - 1) ? waterFlows[fR] : 0;
			V += (j != 0) ? waterFlows[fT] : 0;
			V += (i != SCREEN_HEIGHT - 1) ? waterFlows[fB] : 0;

			V -= waterFlows[L] + waterFlows[R] + waterFlows[B] + waterFlows[T];

			newWaterlvl = waterLvls[j * SCREEN_WIDTH + i] + V;

			velFields[j * SCREEN_WIDTH + i] = ((i != SCREEN_WIDTH - 1) ? waterFlows[fR] : 0) - waterFlows[L] + waterFlows[R] - ((i != 0) ? waterFlows[fL] : 0);
			velFields[j * SCREEN_WIDTH + i + 1] = ((i != SCREEN_HEIGHT - 1) ? waterFlows[fB] : 0) - waterFlows[T] + waterFlows[B] - ((j != 0) ? waterFlows[fT] : 0);

			velFields[j * SCREEN_WIDTH + i] /= (newWaterlvl + waterLvls[j * SCREEN_WIDTH + i]) / 2;
			velFields[j * SCREEN_WIDTH + i + 1] /= (newWaterlvl + waterLvls[j * SCREEN_WIDTH + i]) / 2;

			waterLvls[j * SCREEN_WIDTH + i] = newWaterlvl;
		}
	}
}

void computeFlows()
{
	int L;
	int R;
	int B;
	int T;

	int hL;
	int hR;
	int hB;
	int hT;

	float K = 1;

	for (int i : xVec) {
		for (int j : yVec) {

			L = 4 * (j * SCREEN_WIDTH + i);
			R = 4 * (j * SCREEN_WIDTH + i) + 1;
			B = 4 * (j * SCREEN_WIDTH + i) + 2;
			T = 4 * (j * SCREEN_WIDTH + i) + 3;

			hL = (i != 0) ? terrain[j * SCREEN_WIDTH + i] + waterLvls[j * SCREEN_WIDTH + i] - terrain[j * SCREEN_WIDTH + i -1] - waterLvls[j * SCREEN_WIDTH + i -1] : 0;
			hR = (i != SCREEN_WIDTH - 1) ? terrain[j * SCREEN_WIDTH + i] + waterLvls[j * SCREEN_WIDTH + i] - terrain[j * SCREEN_WIDTH + i + 1] - waterLvls[j * SCREEN_WIDTH + i + 1] : 0;
			hB = (j != 0) ? terrain[j * SCREEN_WIDTH + i] + waterLvls[j * SCREEN_WIDTH + i] - terrain[(j - 1) * SCREEN_WIDTH + i] - waterLvls[(j - 1) * SCREEN_WIDTH + i] : 0;
			hT = (i != SCREEN_HEIGHT - 1) ? terrain[j * SCREEN_WIDTH + i] + waterLvls[j * SCREEN_WIDTH + i] - terrain[(j + 1) * SCREEN_WIDTH + i] - waterLvls[(j + 1) * SCREEN_WIDTH + i] : 0;

			waterFlows[L] = std::max(0.f, waterFlows[L]) + pipeConst * hL;
			waterFlows[R] = std::max(0.f, waterFlows[R]) + pipeConst * hR;
			waterFlows[B] = std::max(0.f, waterFlows[B]) + pipeConst * hB;
			waterFlows[T] = std::max(0.f, waterFlows[T]) + pipeConst * hT;

			K = std::min(1.f, waterLvls[i, j] / (waterFlows[L] + waterFlows[R] + waterFlows[B] + waterFlows[T]));

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
				waterLvls[j * SCREEN_WIDTH + i]++;
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
	background.update(pixels);
	sprite.setTexture(background);
	mainWindow.draw(sprite);
}

void saveImage()
{
	if (event.type == sf::Event::KeyReleased) {
		if (event.key.code == sf::Keyboard::S && !sWasPressed) {
			sWasPressed = true;
			std::cout << "Saving Image. \n";
			background.copyToImage().saveToFile("C:/Users/JonasAllemann/Desktop/Learnings/Blender/RandomTests/perliNoiseTest.png");

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
