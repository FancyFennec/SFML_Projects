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

		switch (DRAWING_STATE) {
		case ALPHA: {
			mainNormalSprite.setTexture(scene.normalLayer.tex);
			break;
		}
		case NORMAL:{
			sf::RenderTexture rTex;
			rTex.create(SCENE_WIDTH, SCENE_HEIGHT);

			normalBlendingShader.setUniform("texture1", sf::Shader::CurrentTexture);
			normalBlendingShader.setUniform("texture2", scene.drawingLayer.tex);
			normalBlendingShader.setUniform("alpha", (**scene.currentBrush).opacity / 255.0f);

			rTex.draw(scene.normalLayer.sprite, normalBlendingRState);
			rTex.display();

			mainNormalTex = rTex.getTexture();
			mainNormalSprite.setTexture(mainNormalTex);
			break;
		}
		}

		scene.drawLowerLayers();
		mainWindowDrawing();
		scene.drawCurrentLayer();
		scene.drawUpperLayers();

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
		sf::Color col = getSampledColor();

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
