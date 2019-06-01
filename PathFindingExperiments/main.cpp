#include "EventHandler.h"
#include "AStar.h"

sf::Uint8 pixels[PIXELS];
char tiles[TILES_WIDTH][TILES_HEIGHT];
int coords[SCREEN_HEIGHT * SCREEN_WIDTH];

sf::Texture background = sf::Texture();
sf::Sprite backgroundImage = sf::Sprite(background);

Button button("Button.png", "Clear");

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Path Finding Experiments");
EventHandler eventHandler;

std::vector <DrawableObject> drawables;

void initialiseTiles();
void initialisePixels();

void eventHandling();
void setTilePixels(const int &tileIndex, const sf::Color &color);
void setPixelColor(const int &pixelIndex, const sf::Color &color);
void drawImage(sf::Sprite &sprite);

void drawShortestPath();

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
		drawShortestPath();

		window.display();
	}

	return 0;
}

void drawShortestPath()
{
	for (auto& rows : tiles)
	{
		for (char& tile : rows)
		{
			if (tile == 'b')
				tile = 'x';
		}
	}

	sf::Vector2i start(1, 1);
	sf::Vector2i goal(TILES_WIDTH - 2, TILES_HEIGHT - 2);

	if (drawables.size() == 1) {
		goal = drawables[0].pos;
	}
	else if (drawables.size() == 2) {
		start = drawables[0].pos;
		goal = drawables[1].pos;
	}

	AStar pathFinder(start, goal, tiles);
	pathFinder.computePath();
	for (sf::Vector2i node : pathFinder.path) {
		tiles[node.x][node.y] = 'b';
	}
	button.draw(window);

	for (DrawableObject& drawable : drawables) {
		drawable.draw(window);
	}
}

void drawImage(sf::Sprite &sprite)
{
	background.update(pixels);
	sprite.setTexture(background);
	window.draw(sprite);
}

void initialisePixels()
{
#pragma omp parallel for
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
			case 'b':
				setTilePixels(tileIndex, sf::Color::Black);
				break;
			default:
				setTilePixels(tileIndex, sf::Color::Green);
				break;
			}
		}
	}
}

void setTilePixels(const int &tileIndex, const sf::Color &color) {
	#pragma omp parallel for
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