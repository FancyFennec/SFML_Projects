#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <Windows.h>
#include <thread> 

#include "json.hpp"
#include "Layer.h"
#include "Scene.h"
#include "GUI.h"
#include "CommandManager.h"

using json = nlohmann::json;

void mainWindowEventHandling();
void mainRenderLoop();
void mousePositionSampling();
void brushWindowEventHandling();
void lmbPressed();
void mainWindowDrawing();
void brushWindowDrawing();
void createMainWindow();
void brushWindowRendering();
void getDesktopResolution(int& horizontal, int& vertical);

sf::Clock deltaClock;

//Window initialisation
sf::RenderWindow mainWindow;
sf::RenderWindow brushWindow;
sf::Event event;

//Needed to get the pen pressure
MSG msg;
UINT32 pointerId;
POINTER_PEN_INFO penInfo;

const int SCENE_WIDTH = 800;
const int SCENE_HEIGHT = 1200;
Scene scene(SCENE_WIDTH, SCENE_HEIGHT);

sf::Thread mouseLoopThread(&mousePositionSampling);
std::vector<sf::Vector2i> mousepositions;

int main() {
	createMainWindow();
	scene.initialize();
	CommandManager::initialize(scene);

	ImGui::SFML::Init(mainWindow);

	mouseLoopThread.launch();
	mainRenderLoop();

	return 0;
}

void mousePositionSampling()
{
	while (mainWindow.isOpen())
	{
		if (isMouseHeld()) {
			mousepositions.push_back(sf::Mouse::getPosition(mainWindow));
			//std::cout << mousepositions.size() << std::endl;
		}
	}
}

void mainRenderLoop()
{
	while (mainWindow.isOpen())
	{
		mousepositions.clear();
		sf::RenderStates state;
		state.transform.translate(sf::Vector2f(scene.currentLayer->offset));

		mainWindow.clear(sf::Color(0, 0, 0, 0));
		ImGui::SFML::Update(mainWindow, deltaClock.restart());

		for (auto iter = scene.layers.begin(); iter <= scene.currentLayer; std::advance(iter, 1)) {
			mainWindow.draw(iter->sprite, state);
		}

		mainWindowDrawing();
		brushWindowDrawing();

		if (scene.currentLayer != scene.lastActiveLayer) {
			for (auto iter = std::next(scene.currentLayer); iter <= scene.lastActiveLayer; std::advance(iter, 1)) {
				mainWindow.draw(iter->sprite, state);
			}
		}


		//TODO: put this pen pressure update somewhere else
		if (PeekMessageW(&msg, mainWindow.getSystemHandle(), 0, 0, PM_NOREMOVE)) {
			pointerId = GET_POINTERID_WPARAM(msg.wParam);
			if (GetPointerPenInfo(pointerId, &penInfo)) {
				(**scene.currentBrush).pressure = penInfo.pressure / 1024.0f;
			}
		}
		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			mainWindowEventHandling();
		}

		mainMenuGUI(mainWindow);
		brushGUI(mainWindow, brushWindow, scene);
		layerGUI(scene);

		ImGui::SFML::Render(mainWindow);
		mainWindow.display();

		brushWindowRendering();
	}
}



void mainWindowEventHandling()
{
	if (event.type == sf::Event::Closed)
		mainWindow.close();
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			lmbPressed();
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
				sf::Texture oldTexture = scene.currentLayer->tex;
				scene.currentLayer->updateLayer(scene.drawingLayer, scene.currentBrush);
				CommandManager::updateLayer(
					std::distance(scene.layers.begin(), scene.currentLayer),
					scene.currentLayer->tex,
					oldTexture);
			}
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q) : {
			mainWindow.clear(sf::Color(255, 255, 255, 255));
			scene.currentLayer->updateLayer(mainWindow);
			break;
		}
		case(sf::Keyboard::LAlt) : {
			setAltIsHeld();
			break;
		}
		case(sf::Keyboard::LControl): {
			setCtrlIsHeld();
			break;
		}
		//TODO: Implement Ctrl + Z / Y
		case(sf::Keyboard::Z): {
			if (isCtrlHeld()) {
				CommandManager::moveBackward(scene);
			}
			break;
		}
		case(sf::Keyboard::Y): {
			if (isCtrlHeld()) {
				CommandManager::moveForward(scene);
			}
			break;
		}
		case(sf::Keyboard::Space): {
			setSpaceIsHeld();
			break;
		}
		case(sf::Keyboard::Up): {
			scene.currentLayer->offset += sf::Vector2i(0, -10);
			break;
		}
		case(sf::Keyboard::Down): {
			scene.currentLayer->offset += sf::Vector2i(0, 10);
			break;
		}
		case(sf::Keyboard::Right): {
			scene.currentLayer->offset += sf::Vector2i(10, 0);
			break;
		}
		case(sf::Keyboard::Left): {
			scene.currentLayer->offset += sf::Vector2i(-10, 0);
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
			setAltNotHeld();
			break;
		}
		case(sf::Keyboard::LControl): {
			setCtrlNotHeld();
			break;
		}
		case(sf::Keyboard::Space): {
			setSpaceNotHeld();
			break;
		}
		}
	}
}

void lmbPressed()
{
	CommandManager::clearActions();
	scene.drawingLayer.clearLayer();

	if (isAltHeld()) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			scene.pickColor(mainWindow);
		}
	} else {
		setMouseIsHeld();
		scene.resetCursorPositions(mainWindow, scene.drawingLayer);
		//No need to draw the window here, it gets drawn because the lmb is being held later
	}
}

void brushWindowEventHandling()
{
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.key.code == sf::Mouse::Left) {
			setMouseIsHeld();
			scene.resetCursorPositions(brushWindow, scene.brushLayer);
			//No need to draw the window here, it gets drawn because the lmb is held later
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.key.code == sf::Mouse::Left) {
			setMouseNotHeld();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			scene.saveBrush();
			brushWindow.close();
		}
	}
}

void mainWindowDrawing()
{
	if (mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (isMouseHeld()) {
			scene.drawOnDrawingLayer(mainWindow);
		}
	}
}

void brushWindowDrawing()
{
	if (brushWindow.hasFocus()) {
		if (sf::Mouse::getPosition(brushWindow).x >= 0 && sf::Mouse::getPosition(brushWindow).y >= 0) {
			if (isMouseHeld()) {
				scene.drawOnBrushLayer(brushWindow);
			}
		}
	}
	else {
		if (sf::Mouse::getPosition(brushWindow).x >= 0 && sf::Mouse::getPosition(brushWindow).y >= 0) {
			brushWindow.requestFocus();
		}
	}
}

void createMainWindow()
{
	if (USE_FULLSCREEN) {
		getDesktopResolution(WINDOW_WIDTH, WINDOW_HEIGHT);
		mainWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drawing App", sf::Style::Fullscreen);
	}
	else {
		mainWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drawing App", sf::Style::Titlebar);
	}

	mainWindow.setMouseCursorVisible(false);
	mainWindow.setFramerateLimit(FPS);
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
		brushWindow.clear(sf::Color::Black);
		scene.brushLayer.drawLayer(brushWindow);
		brushWindow.display();
	}
}

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}