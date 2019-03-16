#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>

//const int sideLength = 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 + 1;
const int sideLength = 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 + 1;

const int SCREEN_WIDTH = sideLength;
const int SCREEN_HEIGHT = sideLength;

sf::Texture texture = sf::Texture();
sf::Sprite sprite = sf::Sprite(texture);

sf::Uint8 pixels[SCREEN_HEIGHT * SCREEN_WIDTH * 4];
sf::Uint8 newPixels[SCREEN_HEIGHT * SCREEN_WIDTH] = { 0 };
int percolationMatrix[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
float diamondMatrix[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Height Map Generator");
sf::Event event;
bool gWasPressed = false;
bool dWasPressed = false;
bool qWasPressed = false;
bool pWasPressed = false;

const float probability = 0.58;
const int averaging = 200;

const int MAX_RAND = 100;
std::random_device random;
std::default_random_engine engine(random());
std::uniform_int_distribution<int> uniformDist(-MAX_RAND, MAX_RAND);

float getRand();
float softenNoise();

void diamondQuad();
void diamondQuadStep(const int &stepLength);
void diamondStep(const int &x, const int &y, const int &stepLength);
void diamond(const int &x, const int &y, const int &stepLength);
void quadStep(const int &x, const int &y, const int &stepLength);
void setDiamonSquarePixels();

void applyPercolation();
void findConnectedComponents(bool &connectedComponentFound);
bool isConnected(const int &j, const int &i);
void updatePixels();
void paintPixelsWithMouse();
void averagePixels();
bool isPowerOf2Plus1(int a);

int noise = 1;


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
	

	for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH * 4; i += 4) {
		pixels[i] = 0;
		pixels[i + 1] = 0;
		pixels[i + 2] = 0;
		pixels[i + 3] = 255;
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		paintPixelsWithMouse();

		// Diamond Square Algorithm
		if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Q && !qWasPressed) {
				qWasPressed = true;
				noise = 1;
				std::cout << "Diamond Square algorithm. \n";
				diamondQuad();
				std::cout << "Updating Pixels. \n";
				setDiamonSquarePixels();
				pWasPressed = false;
			}
		}

		// Percolation
		if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::G && !gWasPressed) {
				gWasPressed = true;
				std::cout << "Applying percolation. \n";
				applyPercolation();
				std::cout << "Updating Pixels. \n";
				updatePixels();
				pWasPressed = false;
			}
		}

		// Smooth out pixels
		if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::D && !dWasPressed) {
				dWasPressed = true;
				std::cout << "Averaging out the pixels. \n";
				averagePixels();
			}
		}

		// Save image
		if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::P && !pWasPressed) {
				pWasPressed = true;
				std::cout << "Saving Image. \n";
				texture.copyToImage().saveToFile("C:/Users/JonasAllemann/Desktop/Learnings/Blender/RandomTests/smoothNoise.png");
				qWasPressed = false;
			}
		}

		texture.update(pixels);
		sprite.setTexture(texture);
		window.draw(sprite);

		window.display();
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------

// Implementation of the Diamond Square algorithm

float softenNoise() {
	return (float)1 / (std::pow(2, noise));
}


float getRand() {
	//gets a uniformly distributed float between -1 and 1
	return (float)uniformDist(engine) / MAX_RAND;
}

void setDiamonSquarePixels() {
	float max = 0;
	float min = 0;

	for (int j = 0; j < sideLength; j++) {
		for (int i = 0; i < sideLength; i++) {
			if (diamondMatrix[j * sideLength + i] > max) {
				max = diamondMatrix[j * sideLength + i];
			}
			if (diamondMatrix[j * sideLength + i] < min) {
				min = diamondMatrix[j * sideLength + i];
			}
		}
	}

std::cout << "Max: " << max << "\n";
std::cout << "Min: " << min << "\n";

	for (int j = 0; j < sideLength; j++) {
		for (int i = 0; i < sideLength; i++) {
			int value = std::max(0, (int)(((diamondMatrix[j * sideLength + i] - min) * 255) / (max - min)));
			pixels[(j * sideLength + i) * 4 + 0] = value;
			pixels[(j * sideLength + i) * 4 + 1] = value;
			pixels[(j * sideLength + i) * 4 + 2] = value;
		}
	}
}

void diamondQuad() {
	int a = sideLength - 1;
	while (a > 1) {
		diamondQuadStep(a);
		a = a / 2;
		++noise;
	}
}

void diamondQuadStep(const int &stepLength) {

	for (int j = 0; j < sideLength - 1; j += stepLength) {
		for (int i = 0; i < sideLength - 1; i += stepLength) {
			quadStep(i, j, stepLength);
		}
	}

	/*std::cout << "Logging result from square step. \n";
	std::cout << "\n";

	for (int j = 0; j < sideLength; ++j) {
		for (int i = 0; i < sideLength; ++i) {
			std::cout << diamondMatrix[j*sideLength + i] << " ";
		}
		std::cout << "\n";
	}*/

	//std::cout << "\n";

	for (int j = 0; j < sideLength - 1; j += stepLength) {
		for (int i = 0; i < sideLength - 1; i += stepLength) {
			diamondStep(i, j, stepLength);
		}
	}

	/*std::cout << "Logging result from diamon step. \n";
	std::cout << "\n";

	for (int j = 0; j < sideLength; ++j) {
		for (int i = 0; i < sideLength; ++i) {
			std::cout << diamondMatrix[j*sideLength + i] << " ";
		}
		std::cout << "\n";
	}*/
}

void quadStep(const int &x, const int &y, const int &stepLength) {
	float upperLeft = diamondMatrix[y * sideLength + x];
	float upperRight = diamondMatrix[y * sideLength + x + stepLength];
	float lowerLeft = diamondMatrix[(y + stepLength) * sideLength + x];
	float lowerRight = diamondMatrix[(y + stepLength) * sideLength + x + stepLength];

	float average = (upperLeft + upperRight + lowerLeft + lowerRight) / 4;
	float random = getRand() * softenNoise();

	diamondMatrix[(y + stepLength / 2) * sideLength + (x + stepLength / 2)] = average + random;
}

void diamondStep(const int &x, const int &y, const int &stepLength) {
	int halfStep = stepLength / 2;

	diamond(x + halfStep, y, stepLength);
	diamond(x, y + halfStep, stepLength);
	diamond(x + halfStep, y + stepLength, stepLength);
	diamond(x + stepLength, y + halfStep, stepLength);
}

void diamond(const int &x, const int &y, const int &stepLength) {
	int halfStep = stepLength / 2;

	if (y == 0) {
		float left = diamondMatrix[y * sideLength + x - halfStep];
		float right = diamondMatrix[y * sideLength + x + halfStep];
		float bottom = diamondMatrix[(y + halfStep) * sideLength + x];

		float average = (left + right + bottom) / 3;
		float random = getRand() * softenNoise();

		diamondMatrix[y * sideLength + x] = average + random;
	} else if (y == sideLength -1) {
		float left = diamondMatrix[y * sideLength + x - halfStep];
		float right = diamondMatrix[y * sideLength + x + halfStep];
		float top = diamondMatrix[(y - halfStep) * sideLength + x];

		float average = (left + right + top) / 3;
		float random = getRand() * softenNoise();

		diamondMatrix[y * sideLength + x] = average + random;
	} else if (x == 0) {
		float bottom = diamondMatrix[y * sideLength + x - halfStep];
		float right = diamondMatrix[y * sideLength + x + halfStep];
		float top = diamondMatrix[(y - halfStep) * sideLength + x];

		float average = (bottom + right + top) / 3;
		float random = getRand() * softenNoise();

		diamondMatrix[y * sideLength + x] = average + random;
	} else if (x == sideLength - 1) {
		float bottom = diamondMatrix[(y + halfStep) * sideLength + x];
		float left = diamondMatrix[y * sideLength + x - halfStep];
		float top = diamondMatrix[(y - halfStep) * sideLength + x];

		float average = (bottom + left + top) / 3;
		float random = getRand() * softenNoise();
		diamondMatrix[y * sideLength + x] = average + random;
	}
	else {
		float bottom = diamondMatrix[(y + halfStep) * sideLength + x];
		float left = diamondMatrix[y * sideLength + x - halfStep];
		float right = diamondMatrix[y * sideLength + x + halfStep];
		float top = diamondMatrix[(y - halfStep) * sideLength + x];

		float average = (bottom + left + right + top) / 4;
		float random = getRand() * softenNoise();

		diamondMatrix[y * sideLength + x] = average + random;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

//Enables draing by clicking and hodling the left mouse button

void paintPixelsWithMouse()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
		if ((0 <= mousePosition.y && mousePosition.y < SCREEN_HEIGHT) && (0 <= mousePosition.x && mousePosition.x < SCREEN_WIDTH)) {
			pixels[(mousePosition.y * SCREEN_WIDTH + mousePosition.x) * 4 + 0] = 255;
			pixels[(mousePosition.y * SCREEN_WIDTH + mousePosition.x) * 4 + 1] = 255;
			pixels[(mousePosition.y * SCREEN_WIDTH + mousePosition.x) * 4 + 2] = 255;
			percolationMatrix[mousePosition.y * SCREEN_WIDTH + mousePosition.x] = 1;
		}
		gWasPressed = false;
		dWasPressed = false;
		qWasPressed = false;
	}
}

void updatePixels() {
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			if (percolationMatrix[j * SCREEN_WIDTH + i] == 1) {
				pixels[(j * SCREEN_WIDTH + i) * 4 + 0] = 255;
				pixels[(j * SCREEN_WIDTH + i) * 4 + 1] = 255;
				pixels[(j * SCREEN_WIDTH + i) * 4 + 2] = 255;
			};
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

// Flattens out the pixels

void averagePixels() {
	gWasPressed = false;

	for (int i = 0; i < averaging; ++i) {

		for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
			for (int i = 1; i < SCREEN_WIDTH - 1; i++) {

				int a = pixels[(j * SCREEN_WIDTH + i + 1) * 4];
				int b = pixels[(j * SCREEN_WIDTH + i - 1) * 4];
				int c = pixels[((j + 1) * SCREEN_WIDTH + i) * 4];
				int d = pixels[((j - 1) * SCREEN_WIDTH + i) * 4];
				int e = pixels[((j + 1) * SCREEN_WIDTH + i + 1) * 4];
				int f = pixels[((j - 1) * SCREEN_WIDTH + i - 1) * 4];
				int g = pixels[((j + 1) * SCREEN_WIDTH + (i - 1)) * 4];
				int h = pixels[((j - 1) * SCREEN_WIDTH + (i + 1)) * 4];

				int average = (a + b + c + d + e + f + g + h ) / 8;
				newPixels[j * SCREEN_WIDTH + i] = average;

			}
		}

		for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
			for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
				pixels[(j * SCREEN_WIDTH + i) * 4 + 0] = newPixels[j * SCREEN_WIDTH + i];
				pixels[(j * SCREEN_WIDTH + i) * 4 + 1] = newPixels[j * SCREEN_WIDTH + i];
				pixels[(j * SCREEN_WIDTH + i) * 4 + 2] = newPixels[j * SCREEN_WIDTH + i];
			}
		}
	}

	int maxPixel = 0;

	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			if (pixels[(j * SCREEN_WIDTH + i) * 4 + 0] > maxPixel) {
				maxPixel = pixels[(j * SCREEN_WIDTH + i) * 4 + 0];
			}
		}
	}

	if (maxPixel != 0) {
		for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
			for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
				pixels[(j * SCREEN_WIDTH + i) * 4 + 0] = std::min((pixels[(j * SCREEN_WIDTH + i) * 4 + 0] * 255) / maxPixel, 255);
				pixels[(j * SCREEN_WIDTH + i) * 4 + 1] = std::min((pixels[(j * SCREEN_WIDTH + i) * 4 + 1] * 255) / maxPixel, 255);
				pixels[(j * SCREEN_WIDTH + i) * 4 + 2] = std::min((pixels[(j * SCREEN_WIDTH + i) * 4 + 2] * 255) / maxPixel, 255);
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

//Percolation

void applyPercolation() {
	dWasPressed = false;
	bool connectedComponentFound = true;

	for (int j= 1; j < SCREEN_HEIGHT - 1; j++) {
		for (int i = 1; i < SCREEN_WIDTH -1; i++) {
			if ((getRand() + 1) / 2 < probability && percolationMatrix[j * SCREEN_WIDTH + i] != 1) {
				percolationMatrix[j * SCREEN_WIDTH + i] = -1;
			};
		}
	}

	while (connectedComponentFound) {
		findConnectedComponents(connectedComponentFound);
	}

	for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
		for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
			if (percolationMatrix[j * SCREEN_WIDTH + i] == -1) {
				percolationMatrix[j * SCREEN_WIDTH + i] = 0;
			};
		}
	}
}

void findConnectedComponents(bool &connectedComponentFound) {
	connectedComponentFound = false;
	for (int j = 1; j < SCREEN_HEIGHT - 1; j++) {
		for (int i = 1; i < SCREEN_WIDTH - 1; i++) {
			if (percolationMatrix[j * SCREEN_WIDTH + i] == -1) {
				if (isConnected(j, i)) {
					percolationMatrix[j * SCREEN_WIDTH + i] = 1;
					connectedComponentFound = true;
				}
			}
		}
	}
}

bool isConnected(const int &j, const int &i) {
	return percolationMatrix[j * SCREEN_WIDTH + i + 1] == 1 ||
		percolationMatrix[j * SCREEN_WIDTH + i - 1] == 1 ||
		percolationMatrix[(j + 1) * SCREEN_WIDTH + i] == 1 ||
		percolationMatrix[(j - 1) * SCREEN_WIDTH + i] == 1;
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

