#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <thread> 

#include "MainMenuGUI.h"
#include "BrushGUI.h"
#include "LayerGUI.h"
#include "EventHandling.h"
#include "GlobalVariables.h"

void initializeGlobalVariables();
bool loadShaders();
void mainRenderLoop();
void sampleMousePositions();
void samplePenPressure();
void sampleNormals();
void dragLayers();
void drawingLoop();
bool notDragingScene();
bool isCursorHoveringLayer();

MSG msg;
UINT32 pointerId;
POINTER_PEN_INFO penInfo;

const unsigned int SCENE_WIDTH = 800;
const unsigned int SCENE_HEIGHT = 1200;
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
	while (mainWindow.isOpen())
	{
		if (isMouseHeld()) CursorBuffer::update(scene.currentBrush);
		sf::sleep(sf::milliseconds(3));
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
			if (CursorBuffer::isFirstStamp && !CursorBuffer::isBufferBeingCleared) {
				scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, CursorBuffer::positions.begin());
			}
			else {
				if (CursorBuffer::positions.size() > 1) {
					size_t positionsToBeRemoved = CursorBuffer::positions.size();
					auto iter = CursorBuffer::positions.begin();
					for (size_t i = 0; i < positionsToBeRemoved - 1 ; i++) {
						scene.drawingLayer.lerpDrawingOnLayer(scene.currentBrush, iter + i);
					}
					CursorBuffer::reset(int(positionsToBeRemoved));
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

bool loadShaders()
{
	bool returnValue = true;

	if (!alphaBlendingShader.loadFromFile(ALPHA_BLENDING_SHADER_PATH, sf::Shader::Fragment)) {
		std::cout << "Could not load ALphaBlendingShader" << std::endl;
		returnValue = false;
	}
	if (!normalBlendingShader.loadFromFile(NORMAL_BLENDING_SHADER_PATH, sf::Shader::Fragment)) {
		std::cout << "Could not load NormalBlendingShader" << std::endl;
		returnValue = false;
	}
	if (!mainRenderShader.loadFromFile(RENDER_SHADER_PATH, sf::Shader::Fragment)) {
		std::cout << "Could not load RenderShader" << std::endl;
		returnValue = false;
	}

	alphaBlendingRState.blendMode = sf::BlendNone;
	normalBlendingRState.blendMode = sf::BlendNone;
	mainRenderState.blendMode = sf::BlendNone;

	return returnValue;
}
