#include <SFML/Graphics.hpp>
#include "wtypes.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib> 

#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "json.hpp"
using json = nlohmann::json;


void mainWindowEventHandling();
void drawOnCanvas(sf::RenderWindow & window, sf::Image& canvasImage, sf::Texture &canvasTexture, sf::Sprite &canvasSprite);
void brushWindowRendering();
void brushWindowEventHandling();
void createBrushWindow();
void renderBrushGUI();
void mainWindowDrawing();
void brushWindowDrawing();
void getDesktopResolution(int& horizontal, int& vertical);
float distance(const sf::Vector2i& vec1, const sf::Vector2i& vec2) {
	return sqrtf(powf((vec1.x - vec2.x), 2.0f) + powf(vec1.y - vec2.y, 2.0f));
}
void drawShapeAt(sf::Vector2f &circlePos, sf::Image& canvasImage);

sf::Clock deltaClock;

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
const float PI = 3.14159265358979323846f;

sf::Text title;

sf::RenderWindow mainWindow;
sf::RenderWindow brushWindow;
sf::Event event;

int BRUSH_WIDTH = 512;

sf::Image defaultBrush;

sf::Image brushImage;
sf::Texture brushTex;
sf::Sprite brushSprite;

sf::Image mainCanvasImage;
sf::Texture mainCanvasTex;
sf::Sprite mainCanvasSprite;

sf::Texture currentTexture;
std::vector<sf::Texture> textureBuffer;
std::vector<sf::Texture>::iterator textureIter;
sf::Sprite backgroundSprite;

sf::RenderStates state;

float radius = 3.f;
float stepsize = 2;
float movedDistance = 0;

std::vector<sf::Vector2i> cursorPositions = { sf::Vector2i(0,0), sf::Vector2i(0,0) };

bool mouseIsHeld = false;
bool ctrlIsPressed = false;
bool lAltIsPressed = false;

int alpha = 100;
static float col[3] = { 0.5f,0.0f,0.5f };
static sf::Color brushColour((sf::Uint8)(col[0] * 255), (sf::Uint8)(col[1] * 255), (sf::Uint8)(col[2] * 255), alpha);

int main() {
	//getDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	//window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App", sf::Style::Fullscreen);
	SCREEN_WIDTH = 800;
	SCREEN_HEIGHT = 800;
	mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Drawing App");
	mainWindow.setMouseCursorVisible(false);
	mainWindow.setFramerateLimit(120);
	mainWindow.clear(sf::Color::White);

	sf::Font font;
	font.loadFromFile("Arial.ttf");
	title.setFont(font);
	title.setString(sf::String("Baole Drawing App"));
	title.setFillColor(sf::Color::Black);
	title.setCharacterSize(24);
	sf::FloatRect rec = title.getGlobalBounds();
	title.setOrigin(rec.width / 2, rec.height / 2);
	title.setPosition(SCREEN_WIDTH / 2, 10);

	ImGui::SFML::Init(mainWindow);

	defaultBrush.loadFromFile("newBrush.png");

	brushImage.create(BRUSH_WIDTH, BRUSH_WIDTH, sf::Color(0, 0, 0, 0));
	brushTex.create(BRUSH_WIDTH, BRUSH_WIDTH);
	brushTex.update(brushImage);
	brushSprite.setTexture(brushTex);

	mainCanvasImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color(0,0,0,0));
	mainCanvasTex.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	mainCanvasTex.update(mainCanvasImage);
	mainCanvasSprite.setTexture(mainCanvasTex);

	currentTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	currentTexture.update(mainWindow);
	backgroundSprite.setTexture(currentTexture);

	textureBuffer.reserve(10);
	textureBuffer.push_back(currentTexture);
	textureIter = textureBuffer.begin();

	while (mainWindow.isOpen())
	{
		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
				mainWindow.close();
			mainWindowEventHandling();
		}

		backgroundSprite.setTexture(currentTexture);
		mainWindow.draw(backgroundSprite);

		brushWindowRendering();
		renderBrushGUI();

		mainWindowDrawing();
		brushWindowDrawing();

		if (event.type == sf::Event::MouseButtonReleased) {
			currentTexture.update(mainWindow);
		}

		mainWindow.draw(title);
		ImGui::SFML::Render(mainWindow);
		mainWindow.display();
	}
	return 0;
}

void mainWindowDrawing()
{
	if (mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (mouseIsHeld) {
			drawOnCanvas(mainWindow, mainCanvasImage, mainCanvasTex, mainCanvasSprite);
			mainWindow.draw(mainCanvasSprite);
		}
	}
}

void brushWindowDrawing()
{
	if (brushWindow.hasFocus()) {
		if (mouseIsHeld) {
			std::cout << "Drawing in Brush Window" << std::endl;
			drawOnCanvas(brushWindow, brushImage, brushTex, brushSprite);
			//mainWindow.draw(mainCanvasSprite);
		}
	}
}

void renderBrushGUI()
{
	ImGui::SFML::Update(mainWindow, deltaClock.restart());

	ImGui::Begin("Brush Settings");
	if (ImGui::ColorPicker3("Brush Colour", col)) {
		brushColour.r = (sf::Uint8)(col[0] * 255);
		brushColour.g = (sf::Uint8)(col[1] * 255);
		brushColour.b = (sf::Uint8)(col[2] * 255);
	}
	ImGui::SliderFloat("Spacing", &stepsize, 0, 50);
	if (ImGui::SliderFloat("Radius", &radius, 0, 50)) {
	}
	if (ImGui::SliderInt("Opacity", &alpha, 0, 255)) {
		brushColour.a = alpha;
	}
	if (ImGui::SmallButton("New Brush")) {
		createBrushWindow();
	}
	ImGui::End();
}

void createBrushWindow()
{
	brushWindow.create(sf::VideoMode(BRUSH_WIDTH, BRUSH_WIDTH), "Define Brush", sf::Style::Titlebar);
	brushWindow.setFramerateLimit(120);
	brushWindow.clear(sf::Color::White);
	brushWindow.setPosition(mainWindow.getPosition() +
		sf::Vector2i(SCREEN_WIDTH / 2 - BRUSH_WIDTH / 2, SCREEN_HEIGHT / 2 - BRUSH_WIDTH / 2));

	/*for (int i = 0; i < BRUSH_WIDTH; i++) {
		for (int j = 0; j < BRUSH_WIDTH; j++) {
			float x = i - BRUSH_WIDTH / 2;
			float y = j - BRUSH_WIDTH / 2;

			int count = 0;

			for (float k = 0.0f; k < 4; k++) {
				for (float l = 0.0f; l < 4; l++) {
					if ((x + k / 4.0f + l / 4.0f) * (x + k / 4.0f + l / 4.0f) +
						(y + k / 4.0f + l / 4.0f) * (y + k / 4.0f + l / 4.0f) < 100 * 100) {
						count++;
					}
				}
			}

			sf::Color pixelColour = sf::Color::Black;
			pixelColour.a = sf::Uint8((255 * count) / 16);
			brushImage.setPixel(i, j, pixelColour);
		}
	}
	brushTex.update(brushImage);
	brushSprite.setTexture(brushTex);*/
}

void brushWindowRendering()
{
	if (brushWindow.isOpen() && brushWindow.hasFocus()) {
		while (brushWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				brushWindow.close();
			brushWindowEventHandling();
		}
		brushWindow.clear(sf::Color::White);
		brushWindow.draw(brushSprite);
		brushWindow.display();
	}
}

void drawOnCanvas(sf::RenderWindow & window, sf::Image& canvasImage, sf::Texture &canvasTexture, sf::Sprite &canvasSprite)
{
	sf::Vector2i currentPos = sf::Mouse::getPosition(window);
	movedDistance += distance(cursorPositions[1], currentPos);
	cursorPositions[1] = currentPos;

	if (movedDistance > stepsize) {

		int steps = (int)std::floorf(movedDistance / stepsize);
		float offsetFactor = (steps * stepsize - movedDistance);

		sf::Vector2f direction = sf::Vector2f(cursorPositions[1] - cursorPositions[0]) / distance(cursorPositions[1], cursorPositions[0]);
		sf::Vector2f circlePos = sf::Vector2f(cursorPositions[0]);

		
#pragma omp parallel for
		for (int i = 0; i < steps; i++) {
			sf::Vector2f drawingPos = circlePos + (i + 1) * stepsize * direction;
			drawShapeAt(drawingPos, canvasImage);
			mainCanvasTex.update(canvasImage);
			canvasSprite.setTexture(mainCanvasTex);
		}
		circlePos += steps * stepsize * direction;

		cursorPositions[0] = sf::Vector2i(circlePos);
		cursorPositions[1] = currentPos;
		
		movedDistance -= stepsize * steps;
	}
}

void drawShapeAt(sf::Vector2f& circlePos, sf::Image& canvasImage)
{
	int xPos;
	int yPos;

	int rad = std::ceilf(radius);
	for (int i = -rad; i < rad; i++) {
		for (int j = -rad; j < rad; j++) {

			xPos = (int)circlePos.x + i;
			yPos = (int)circlePos.y + j;

			if (xPos >= 0 && yPos >= 0 && xPos < canvasImage.getSize().x && yPos < canvasImage.getSize().y) {
				canvasImage.setPixel(xPos, yPos, i*i + j * j > radius * radius ?
					canvasImage.getPixel(xPos, yPos) :
					canvasImage.getPixel(xPos, yPos).a >= brushColour.a ?
					canvasImage.getPixel(xPos, yPos) :
					brushColour
				);
			}
		}
	}
}

void mainWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			mainCanvasImage.create(SCREEN_WIDTH, SCREEN_HEIGHT, sf::Color(0, 0, 0, 0));
			mainCanvasTex.update(mainCanvasImage);
			mainCanvasSprite.setTexture(mainCanvasTex);

			if (lAltIsPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					sf::Uint8 alpha = brushColour.a;
					sf::Vector2i pos = sf::Mouse::getPosition(mainWindow);

					brushColour = currentTexture.copyToImage().getPixel(pos.x, pos.y);
					brushColour.a = alpha;

					col[0] = brushColour.r;
					col[1] = brushColour.g;
					col[2] = brushColour.b;
				}
			} else {
				if (textureIter != std::prev(textureBuffer.end())) {
					auto lastIter = std::prev(textureBuffer.end());

					while (textureIter != lastIter) {
						textureBuffer.erase(lastIter);
						std::advance(lastIter, -1);
					}
				}
				movedDistance = 0.0f;
				mouseIsHeld = true;

				sf::Vector2i newPos = sf::Mouse::getPosition(mainWindow);
				cursorPositions[0] = newPos;
				cursorPositions[1] = newPos;

				sf::Vector2f circlePos = sf::Vector2f(newPos);
				drawShapeAt(circlePos, mainCanvasImage);
			}
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				mainWindow.draw(backgroundSprite);
				mainWindow.draw(mainCanvasSprite);
				currentTexture.update(mainWindow);
			}
			if (textureBuffer.size() >= 10) {
				textureBuffer.erase(textureBuffer.begin());
			}
			textureBuffer.push_back(currentTexture);
			textureIter = textureBuffer.end() - 1;
			mouseIsHeld = false;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q): {
			mainWindow.clear(sf::Color(255, 255, 255, 255));
			currentTexture.update(mainWindow);
			break;
		}
		case(sf::Keyboard::LAlt) : {
			lAltIsPressed = true;
			break;
		}
		case(sf::Keyboard::LControl): {
			ctrlIsPressed = true;
			break;
		}
		case(sf::Keyboard::Z): {
			if (ctrlIsPressed) {
				if (textureIter != textureBuffer.begin()) std::advance(textureIter, -1);
				currentTexture = *textureIter;
			}
			break;
		}
		case(sf::Keyboard::Y): {
			if (ctrlIsPressed) {
				if (textureIter != textureBuffer.end() - 1) std::advance(textureIter, 1);
				currentTexture = *textureIter;
			}
			break;
		}
		case(sf::Keyboard::Escape): {
			mainWindow.close();
			break;
		}
		}
	}
	if (event.type == sf::Event::KeyReleased) {
		switch (event.key.code) {
		case(sf::Keyboard::LAlt): {
			lAltIsPressed = false;
			break;
		}
		case(sf::Keyboard::LControl): {
			ctrlIsPressed = false;
			break;
		}
		}
	}
}

void brushWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.key.code == sf::Mouse::Left) {
			movedDistance = 0.0f;
			mouseIsHeld = true;

			sf::Vector2i newPos = sf::Mouse::getPosition(brushWindow);
			cursorPositions[0] = newPos;
			cursorPositions[1] = newPos;

			sf::Vector2f circlePos = sf::Vector2f(newPos);
			drawShapeAt(circlePos, brushImage);
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.key.code == sf::Mouse::Left) {
			mouseIsHeld = false;
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			//brushImage.saveToFile("newBrush.png");
			brushWindow.close();
		}
	}
}

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}