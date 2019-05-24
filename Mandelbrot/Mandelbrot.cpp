#include <SFML/Graphics.hpp>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

sf::Uint8 pixls[SCREEN_HEIGHT * SCREEN_WIDTH * 4];
int coords[SCREEN_HEIGHT * SCREEN_WIDTH];
sf::Vector2i mousePos;

double x = -0.5, y = 0;
int nZoom = 0;
int iterations = 1000;

sf::Texture background = sf::Texture();
sf::Sprite backgroundImage = sf::Sprite(background);

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Mandelbrot");
sf::Event event;

void initialisePixels();
void drawImage(sf::Sprite &sprite);
int getCoords(int i, int j);
void computeMandelbrot();
void eventHandling();

int main()
{
	window.setFramerateLimit(60);
	background.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite sprite(background);

	initialisePixels();

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			eventHandling();
		}

		window.clear();

		computeMandelbrot();
		drawImage(sprite);

		window.display();
	}
	return 0;
}

void eventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			mousePos = sf::Mouse::getPosition(window);
			x += ((double)mousePos.x - SCREEN_WIDTH / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1));
			y += ((double)mousePos.y - SCREEN_HEIGHT / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1));
			nZoom++;
		}
		if (event.mouseButton.button == sf::Mouse::Right) {
			mousePos = sf::Mouse::getPosition(window);
			x += ((double)mousePos.x - SCREEN_WIDTH / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1));
			y += ((double)mousePos.y - SCREEN_HEIGHT / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1));
			nZoom--;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::S) {
			std::cout << "Saving Image. \n";
			background.copyToImage().saveToFile("C:/Users/JonasAllemann/MandelBrot.png");

		}
	}
}

void computeMandelbrot()
{
	for (unsigned int i = 0; i < SCREEN_WIDTH; i++) {
		for (unsigned int j = 0; j < SCREEN_HEIGHT; j++) {

			double cr = (((double)i - SCREEN_WIDTH / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1))) + x;
			double ci = (((double)j - SCREEN_HEIGHT / 2) / (SCREEN_WIDTH * pow(2, nZoom - 1))) + y;

			double zr = 0, zi = 0;

			unsigned int nIter = 0;

			while (zr * zr + zi * zi < 4 && nIter <= iterations) {
				double tr = zr * zr - zi * zi + cr;
				zi = 2 * zr *zi + ci;
				zr = tr;
				nIter++;
			}

			if (nIter <= iterations) {
				pixls[getCoords(i, j)] = (int)(255 * nIter) / iterations;
			}
			else {
				pixls[getCoords(i, j)] = 0;
			}

		}
	}
}

int getCoords(int i, int j) {
	if (i < 0 || i >= SCREEN_WIDTH || j < 0 || i >= SCREEN_HEIGHT) {
		std::cout << "ERROR! i or j out of range.\n";
		std::cout << "i was: " << i << "\n";
		std::cout << "j was: " << j << "\n";
		return -1;
	}

	return (i + j * SCREEN_WIDTH) * 4;
}

void drawImage(sf::Sprite &sprite)
{
	background.update(pixls);
	sprite.setTexture(background);
	window.draw(sprite);
}

void initialisePixels()
{
	for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH * 4; i += 4) {
		pixls[i] = 255;
		pixls[i + 1] = 0;
		pixls[i + 2] = 0;
		pixls[i + 3] = 255;
	}
}