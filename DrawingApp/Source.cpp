#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <thread> 

#include "MainMenuGUI.h"
#include "BrushGUI.h"
#include "LayerGUI.h"
#include "EventHandling.h"

void mousePositionSampling();

void createMainWindow();
void mainRenderLoop();
void moveLayers();
void mainWindowDrawing();
void brushWindowDrawing();
void brushWindowRendering();

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

//TODO: use this seperate thread to implement more smooth mouse poisitions
void mousePositionSampling()
{
	while (mainWindow.isOpen())
	{
		if (isMouseHeld()) { // create stream for the layer class to process
			if (PeekMessageW(&msg, mainWindow.getSystemHandle(), 0, 0, PM_NOREMOVE)) {
				pointerId = GET_POINTERID_WPARAM(msg.wParam);
				if (GetPointerPenInfo(pointerId, &penInfo)) {
					(**scene.currentBrush).pressure = penInfo.pressure / 1024.0f;
				}
			}
			CursorBufferUtils::updateBuffer(scene.currentBrush);
		}
	}
}

void mainRenderLoop()
{
	while (mainWindow.isOpen())
	{
		mousepositions.clear(); //TODO: Clearing should be after the positions have been used
		sf::RenderStates state;
		state.transform.translate(sf::Vector2f(scene.currentLayer->offset));

		mainWindow.clear(sf::Color(0, 0, 0, 0));
		ImGui::SFML::Update(mainWindow, deltaClock.restart());

		for (auto iter = scene.layers.begin(); iter <= scene.currentLayer; std::advance(iter, 1)) {
			mainWindow.draw(iter->sprite, state);
		}
		
		moveLayers();
		mainWindowDrawing();
		brushWindowDrawing();

		if (scene.currentLayer != scene.lastActiveLayer) {
			for (auto iter = std::next(scene.currentLayer); iter <= scene.lastActiveLayer; std::advance(iter, 1)) {
				mainWindow.draw(iter->sprite, state);
			}
		}

		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			mainWindowEventHandling(scene);
		}

		mainMenuGUI(scene);
		brushGUI(scene);
		layerGUI(scene);

		ImGui::SFML::Render(mainWindow);
		mainWindow.display();

		brushWindowRendering();
	}
}

void moveLayers()
{
	if (isMouseHeld() && isSpaceHeld()) {//This allows dragging of the layers
		scene.cursorPositions[2] = scene.cursorPositions[3];
		scene.cursorPositions[3] = sf::Mouse::getPosition(mainWindow);
		scene.currentLayer->offset += scene.cursorPositions[3] - scene.cursorPositions[2];
	}
}

void mainWindowDrawing()
{
	if (mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		if (isMouseHeld() && !isSpaceHeld()) {
			if (CursorBufferUtils::isFirstStamp) {
				scene.drawingLayer.drawLerpOnLayer(scene.currentBrush, CursorBufferUtils::cursorBuffer.begin(), mainWindow);
			}
			else {
				if (CursorBufferUtils::cursorBuffer.size() > 1) {
					auto iter = CursorBufferUtils::cursorBuffer.begin();
					for (int i = 0; i < CursorBufferUtils::cursorBuffer.size() - 1 ; i++) {
						scene.drawingLayer.drawLerpOnLayer(scene.currentBrush, iter + i, mainWindow);
					}
					CursorBufferUtils::resetBuffer();
				}
			}
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
			brushWindowEventHandling(scene);
		}
		brushWindow.clear(sf::Color::Black);
		scene.brushLayer.drawLayerinWindow(brushWindow);
		brushWindow.display();
	}
}
