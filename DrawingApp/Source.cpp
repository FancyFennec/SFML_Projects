#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <thread> 

#include "json.hpp"
#include "Scene.h"
#include "GUI.h"
#include "EventHandling.h"
#include "CommandManager.h"

using json = nlohmann::json;

void mousePositionSampling();

void createMainWindow();
void mainRenderLoop();
void mainWindowDrawing();
void brushWindowDrawing();
void brushWindowRendering();

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
			mainWindowEventHandling(mainWindow, event, scene);
		}

		mainMenuGUI(mainWindow);
		brushGUI(mainWindow, brushWindow, scene);
		layerGUI(scene);

		ImGui::SFML::Render(mainWindow);
		mainWindow.display();

		brushWindowRendering();
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
			brushWindowEventHandling(brushWindow, event, scene);
		}
		brushWindow.clear(sf::Color::Black);
		scene.brushLayer.drawLayer(brushWindow);
		brushWindow.display();
	}
}
