#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <thread> 

#include "MainMenuGUI.h"
#include "BrushGUI.h"
#include "LayerGUI.h"
#include "EventHandling.h"
#include "GlobalVariables.h"

void initializeGlobalVariables();
void mainRenderLoop();
void sampleMousePositions();
void samplePenPressure();
void sampleNormals();
void dragLayers();
void drawingLoop();
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
	if (!loadShaders()) return -1;
	initializeGlobalVariables();

	positionSamplingLoop.launch();
	mainRenderLoop();

	return 0;
}

void mainRenderLoop()
{
	while (mainWindow.isOpen())
	{
		mainWindow.clear(sf::Color(0, 0, 0, 0));
		mainRenderTex.clear();
		
		ImGui::SFML::Update(mainWindow, deltaClock.restart());

		scene.setGlobalNormalSprite();
		scene.drawLowerLayers();
		scene.drawCurrentLayer();
		scene.drawUpperLayers();

		drawingLoop();
		dragLayers();

		samplePenPressure(); //Needs to be done before processing the events
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
	//TODO: There are still thing which can go wrong here :(
	while (mainWindow.isOpen())
	{
		if (isMouseHeld()) CursorBuffer::update(scene.currentBrush);
		sf::sleep(sf::milliseconds(5));
	}
}

void samplePenPressure()
{
	if (PeekMessageW(&msg, mainWindow.getSystemHandle(), 0, 0, PM_NOREMOVE)) {
		pointerId = GET_POINTERID_WPARAM(msg.wParam);
		if (GetPointerPenInfo(pointerId, &penInfo)) {
			if (penInfo.pressure > 0) {
				if (!isMouseHeld()) lmbPressed(scene);
				(**scene.currentBrush).pressure = penInfo.pressure / 1024.0f;
			}
		}
	}
}

void sampleNormals()
{
	if (pickNormalValue) {
		sf::Color col = getAltSampledColor(scene);

		if (col != sf::Color::Black) {
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

void drawingLoop()
{
	if (isCursorHoveringLayer()) {
		if (notDragingScene()) {
			if (CursorBuffer::isFirstStamp && CursorBuffer::useBuffer1) {
				scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, CursorBuffer::buffer1.begin());
				CursorBuffer::isFirstStamp = false;
			}
			else {
				if (CursorBuffer::useBuffer1 && CursorBuffer::buffer1.size() > 1) {
					CursorBuffer::useBuffer1 = false;
					CursorBuffer::useBuffer2 = true;
					
					for (auto iter = CursorBuffer::buffer1.begin(); iter < CursorBuffer::buffer1.end() - 1 ; iter++) {
						scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, iter);
					}
					CursorBuffer::resetBuffer1 = true;
				}
				if (CursorBuffer::useBuffer2 && CursorBuffer::buffer2.size() > 1) {
					CursorBuffer::useBuffer2 = false;
					CursorBuffer::useBuffer1 = true;

					for (auto iter = CursorBuffer::buffer2.begin(); iter < CursorBuffer::buffer2.end() - 1; iter++) {
						scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, iter);
					}
					CursorBuffer::resetBuffer2 = true;
				}
			}
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

void initializeGlobalVariables()
{
	normalTex.loadFromFile("normalColorPicker.png");
	samplingTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);
	mainRenderTex.create(SCENE_WIDTH, SCENE_HEIGHT);

	if (USE_FULLSCREEN) {
		getDesktopResolution(WINDOW_WIDTH, WINDOW_HEIGHT);
		mainWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drawing App", sf::Style::Fullscreen);
	}
	else {
		mainWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Drawing App", sf::Style::Titlebar);
	}

	mainWindow.setMouseCursorVisible(false);
	mainWindow.setFramerateLimit(FPS);

	CommandManager::initialize(scene);
	ImGui::SFML::Init(mainWindow);
}
