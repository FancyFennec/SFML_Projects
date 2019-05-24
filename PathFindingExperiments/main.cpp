#include "DrawableObject.h"

sf::Uint8 pixels[PIXELS];
char tiles[TILES_WIDTH][TILES_HEIGHT];
int coords[SCREEN_HEIGHT * SCREEN_WIDTH];
sf::Vector2i mousePos;

sf::Texture background = sf::Texture();
sf::Sprite backgroundImage = sf::Sprite(background);
sf::Texture button;
sf::Sprite buttonImage;
sf::Text buttonText;
sf::Font font;

sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Mandelbrot");
sf::Event event;

bool lMousePressed = false;
std::vector <DrawableObject> drawables;

void initialiseTiles();
void initialisePixels();
void initializeButton();
void initializeText();
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
	initializeButton();
	initializeText();

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			eventHandling();
			if (lMousePressed) {
				tiles[(int)(mousePos.x / 10)][(int)(mousePos.y / 10)] = 'r';
			}
		}

		window.clear();


		initialisePixels();

		drawImage(sprite);
		window.draw(buttonImage);
		window.draw(buttonText);

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

void initializeButton() {
	if (!button.loadFromFile("Button.png"))
		std::cout << "Can't find the image" << std::endl;
	buttonImage.setPosition(0.0f, 0.0f);

	float exitButtonWidth = buttonImage.getLocalBounds().width;
	float exitButtonHeight = buttonImage.getLocalBounds().height;

	buttonImage.setScale(0.2, 0.2);
	buttonImage.setTexture(button);
}

void initializeText() {
	if (!font.loadFromFile("arial.ttf"))
		std::cout << "Can't find the font file" << std::endl;

	buttonText.setFont(font);
	buttonText.setStyle(sf::Text::Bold);
	buttonText.setString("Button");
	buttonText.setFillColor(sf::Color::Black);
	buttonText.setCharacterSize(48);
	buttonText.setPosition(7.0f, 10.0f);
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

void eventHandling()
{
	switch (event.type)
	{
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::MouseMoved:
	{
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		if (buttonImage.getGlobalBounds().contains(mousePosF))
		{
			buttonImage.setColor(sf::Color(250, 50, 50));
		}
		else
		{
			buttonImage.setColor(sf::Color(255, 255, 255));
		}
	}
	break;
	case sf::Event::MouseButtonPressed:
	{
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		if (buttonImage.getGlobalBounds().contains(mousePosF))
		{
			std::cout << "Clicked, yay!" << std::endl;
			
		}
		else if (event.mouseButton.button == sf::Mouse::Left) 
		{
			lMousePressed = true;
		}
		else if (event.mouseButton.button == sf::Mouse::Right)
		{
			DrawableObject drawable(sf::Vector2i((int)(mousePos.x / 10), (int)(mousePos.y / 10)));
			drawables.push_back(drawable);
			std::cout << drawable.pos.x << " " << drawable.pos.y << "\n";
		}
	}
	break;
	case sf::Event::MouseButtonReleased:
	{
		mousePos = sf::Mouse::getPosition(window);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		if (buttonImage.getGlobalBounds().contains(mousePosF))
		{
			std::cout << "Clicked, yay!" << std::endl;

		}
		else if (event.mouseButton.button == sf::Mouse::Left)
		{
			lMousePressed = false;
		}
	}
	break;
	}
}