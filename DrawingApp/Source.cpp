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

bool notDragingScene();

bool isCursorHoveringLayer();

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
			CursorBuffer::update(scene.currentBrush);
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
	if (isCursorHoveringLayer()) {
		if (notDragingScene()) {
			if (CursorBuffer::isFirstStamp) {
				scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, CursorBuffer::positions.begin());
			}
			else {
				if (CursorBuffer::positions.size() > 1) {
					auto iter = CursorBuffer::positions.begin();
					for (int i = 0; i < CursorBuffer::positions.size() - 1 ; i++) {
						scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, iter + i);
					}
					CursorBuffer::reset();
				}
			}
			scene.renderDrawingLayer();
		}
	}
}

bool notDragingScene()
{
	return isMouseHeld() && !isSpaceHeld();
}

bool isCursorHoveringLayer()
{
	return mainWindow.hasFocus() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive();
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
