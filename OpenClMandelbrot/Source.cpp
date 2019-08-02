#include <SFML/Graphics.hpp>
#include "OpenCL.h"


const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

sf::Uint8 pixls[SCREEN_HEIGHT * SCREEN_WIDTH * 4];
int coords[SCREEN_HEIGHT * SCREEN_WIDTH];
sf::Vector2i mousePos;

double x = -0.5, y = 0;
int nZoom = 0;
int iterations = 1000;

sf::Texture background = sf::Texture();
sf::Sprite backgroundImage = sf::Sprite(background);

sf::RenderWindow mainWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Mandelbrot", sf::Style::Fullscreen);
sf::Event event;

OpenCL test(SCREEN_WIDTH, SCREEN_HEIGHT, "HelloWorld_Kernel.cl");
int* output;

void initialisePixels();
void drawImage(sf::Sprite &sprite);
int getCoords(int i, int j);
void computeMandelbrot();
void mainWindowEventHandling();

int main()
{
	mainWindow.setFramerateLimit(60);
	background.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite sprite(background);

	initialisePixels();
	if (!test.initialise()) {
		return -1;
	}

	while (mainWindow.isOpen())
	{
		while (mainWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mainWindow.close();
			mainWindowEventHandling();
		}

		mainWindow.clear();

		computeMandelbrot();
		drawImage(sprite);

		mainWindow.display();
	}
	return 0;
}

void mainWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			mousePos = sf::Mouse::getPosition(mainWindow);
			x += ((double)mousePos.x - SCREEN_WIDTH / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1));
			y += ((double)mousePos.y - SCREEN_HEIGHT / 2) / (SCREEN_HEIGHT * pow(2, nZoom - 1));
			nZoom++;
		}
		if (event.mouseButton.button == sf::Mouse::Right) {
			mousePos = sf::Mouse::getPosition(mainWindow);
			x += ((double)mousePos.x - SCREEN_WIDTH / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1));
			y += ((double)mousePos.y - SCREEN_HEIGHT / 2) / (SCREEN_HEIGHT * pow(2, nZoom - 1));
			nZoom--;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::S) {
			std::cout << "Saving Image. \n";
			background.copyToImage().saveToFile("Mandelbrot.png");
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			std::cout << "Closing Window. \n";
			mainWindow.close();
		}
	}
}

void computeMandelbrot()
{
	output = test.run(nZoom, x, y);

#pragma omp parallel for
	for (int j = 0; j < SCREEN_HEIGHT; j++) {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			pixls[getCoords(i, j)] = output[j * SCREEN_WIDTH + i];
		}
	}
}

int getCoords(int i, int j) {
	if (i < 0 || i >= SCREEN_WIDTH || j < 0 || j >= SCREEN_HEIGHT) {
		std::cout << "ERROR! i or j out of range.\n";
		std::cout << "i was: " << i << "\n";
		std::cout << "j was: " << j << "\n";
		return -1;
	}

	return (j * SCREEN_WIDTH + i) * 4;
}

void drawImage(sf::Sprite &sprite)
{
	background.update(pixls);
	sprite.setTexture(background);
	mainWindow.draw(sprite);
}

void initialisePixels()
{
#pragma omp parallel for
	for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH * 4; i += 4) {
		pixls[i] = 255;
		pixls[i + 1] = 0;
		pixls[i + 2] = 0;
		pixls[i + 3] = 255;
	}
}
