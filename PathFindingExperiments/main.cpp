#include "EventHandler.h"
#include "A_Star.h"

sf::Uint8 pixels[PIXELS];
char tiles[TILES_WIDTH][TILES_HEIGHT];
int coords[SCREEN_HEIGHT * SCREEN_WIDTH];

sf::Texture background = sf::Texture();
sf::Sprite backgroundImage = sf::Sprite(background);

Button button("Button.png", "Clear");

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Mandelbrot");
EventHandler eventHandler;

std::vector <DrawableObject> drawables;

void initialiseTiles();
void initialisePixels();

void eventHandling();
void setTilePixels(const int &tileIndex, const sf::Color &color);
void setPixelColor(const int &pixelIndex, const sf::Color &color);
void drawImage(sf::Sprite &sprite);

int main() {

	window.setFramerateLimit(FRAMERATE);
	background.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite sprite(background);

	initialiseTiles();
	initialisePixels();

	while (window.isOpen())
	{
		while (window.pollEvent(eventHandler.event))
		{
			eventHandler.handleEvents(window, drawables, tiles, button.sprite);
		}

		window.clear();

		initialisePixels();
		drawImage(sprite);
		button.draw(window);

		for (DrawableObject drawable : drawables) {
			drawable.draw(window);
		}

		window.display();
	}

	return 0;
}

void drawImage(sf::Sprite &sprite)
{
	background.update(pixels);
	sprite.setTexture(background);
	window.draw(sprite);
}

void initialisePixels()
{
	for (int j = 0; j < TILES_WIDTH; j++) {
		for (int i = 0; i < TILES_HEIGHT; i++) {
			int tileIndex = (j *TILES_WIDTH * 100 + i * 10) * 4;

			switch (tiles[i][j])
			{
			case 'x':
				setTilePixels(tileIndex, sf::Color::White);
				break;
			case 'r':
				setTilePixels(tileIndex, sf::Color::Red);
				break;
			default:
				setTilePixels(tileIndex, sf::Color::Green);
				break;
			}
		}
	}
}

void setTilePixels(const int &tileIndex, const sf::Color &color) {
	for (int k = 0; k < 10; k++) {
		for (int l = 0; l < 10; l++) {
			int pixelIndex = tileIndex + (k * SCREEN_WIDTH + l) * 4;
			setPixelColor(pixelIndex, color);
		}
	}
}

void setPixelColor(const int &pixelIndex, const sf::Color &color) {
	pixels[pixelIndex] = color.r;
	pixels[pixelIndex + 1] = color.g;
	pixels[pixelIndex + 2] = color.b;
	pixels[pixelIndex + 3] = color.a;
}

void initialiseTiles()
{
	std::ifstream myfile("example.txt");
	std::string line;
	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line))
		{
			int j = 0;
			for (char a : line) {
				tiles[i][j] = a;
				j++;
			}
			i++;
		}
		myfile.close();
	}
}