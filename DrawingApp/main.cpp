#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <thread> 

#include "MainMenuGUI.h"
#include "BrushGUI.h"
#include "LayerGUI.h"
#include "EventHandling.h"

void createMainWindow();
void mainRenderLoop();
void sampleMousePositions();
void samplePenPressure();
void sampleNormals();
void dragLayers();
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

sf::Thread positionSamplingLoop(&sampleMousePositions);

int main() {
	createMainWindow();
	scene.initialize();
	CommandManager::initialize(scene);
	normalTex.loadFromFile("normalColorPicker.png");
	samplingTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

	ImGui::SFML::Init(mainWindow);

	positionSamplingLoop.launch();
	mainRenderLoop();

	return 0;
}

void mainRenderLoop()
{
	while (mainWindow.isOpen())
	{
		sf::RenderStates state;
		state.transform.translate(sf::Vector2f(scene.currentLayer->offset));

		mainWindow.clear(sf::Color(0, 0, 0, 0));
		ImGui::SFML::Update(mainWindow, deltaClock.restart());

		for (auto iter = scene.layers.begin(); iter <= scene.currentLayer; std::advance(iter, 1)) {
			mainWindow.draw(iter->sprite, state);
		}
		
		dragLayers();
		mainWindowDrawing();

		if (scene.currentLayer != scene.lastActiveLayer) {
			for (auto iter = std::next(scene.currentLayer); iter <= scene.lastActiveLayer; std::advance(iter, 1)) {
				mainWindow.draw(iter->sprite, state);
			}
		}

		samplePenPressure(); //Needs to be done before we process all the events
		while (mainWindow.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			mainWindowEventHandling(scene);
		}

		mainMenuGUI(scene);
		brushGUI(scene);
		layerGUI(scene);

		ImGui::SFML::Render(mainWindow);
		sampleNormals(); //Needs to be done after the imgui rendering
		
		mainWindow.display();
	}
}

void sampleMousePositions()
{
	while (mainWindow.isOpen())
	{
		if (isMouseHeld()) CursorBuffer::update(scene.currentBrush);
	}
}

void samplePenPressure()
{
	if (PeekMessageW(&msg, mainWindow.getSystemHandle(), 0, 0, PM_NOREMOVE)) {
		pointerId = GET_POINTERID_WPARAM(msg.wParam);
		if (GetPointerPenInfo(pointerId, &penInfo)) {
			std::cout << penInfo.pressure << std::endl;
			(**scene.currentBrush).pressure = penInfo.pressure / 1024.0f;
		}
	}
}

void sampleNormals()
{
	if (pickNormalValue) {
		sf::Color col = getSampledColor();

		if (col != sf::Color(66, 150, 250)) {
			(**scene.currentBrush).currentNormal = col;
		}
		pickNormalValue = false;
	}
}

void dragLayers()
{
	if (isMouseHeld() && isSpaceHeld()) {//This allows dragging of the layers
		scene.cursorPositions[0] = scene.cursorPositions[1];
		scene.cursorPositions[1] = sf::Mouse::getPosition(mainWindow);
		scene.currentLayer->offset += scene.cursorPositions[1] - scene.cursorPositions[0];
	}
}

void mainWindowDrawing()
{
	if (isCursorHoveringLayer()) {
		if (notDragingScene()) {
			if (CursorBuffer::isFirstStamp && !CursorBuffer::isBufferBeingCleared) {
				scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, CursorBuffer::positions.begin());
			}
			else {
				if (CursorBuffer::positions.size() > 1) {
					int positionsToBeRemoved = CursorBuffer::positions.size();
					auto iter = CursorBuffer::positions.begin();
					for (int i = 0; i < positionsToBeRemoved - 1 ; i++) {
						scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, iter + i);
					}
					CursorBuffer::reset(positionsToBeRemoved);
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
