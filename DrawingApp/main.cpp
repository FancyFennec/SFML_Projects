#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <thread> 

#include "MainMenuGUI.h"
#include "BrushGUI.h"
#include "LayerGUI.h"
#include "EventHandling.h"
#include "GlobalVariables.h"

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
	if (!loadShaders()) return -1;
	normalTex.loadFromFile("normalColorPicker.png");
	samplingTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);

	createMainWindow();
	mainRenderTex.create(SCENE_WIDTH, SCENE_HEIGHT);
	CommandManager::initialize(scene);
	ImGui::SFML::Init(mainWindow);

	positionSamplingLoop.launch();
	mainRenderLoop();

	return 0;
}

void mainRenderLoop()
{
	while (mainWindow.isOpen())
	{
		sf::RenderStates renderState(&mainRenderShader);
		sf::RenderStates mainRenderState;
		mainRenderState.transform.translate(sf::Vector2f(scene.currentLayer->offset));
		renderState.blendMode = sf::BlendNone;

		mainWindow.clear(sf::Color(0, 0, 0, 0));
		mainRenderTex.clear();
		sf::Image image;
		image.create(SCENE_WIDTH, SCENE_WIDTH, sf::Color::White);
		sf::Texture tex;
		tex.loadFromImage(image);
		
		ImGui::SFML::Update(mainWindow, deltaClock.restart());

		for (auto iter = scene.layers.begin(); iter <= scene.currentLayer; std::advance(iter, 1)) {
			mainRenderShader.setUniform("normalMap", sf::Shader::CurrentTexture);
			mainRenderShader.setUniform("layerTex", iter == scene.layers.begin() ? tex : iter->tex);

			mainRenderShader.setUniform("lightPos", scene.lightSource.pos);
			mainRenderShader.setUniform("lightCol", scene.lightSource.col);

			mainRenderShader.setUniform("shininess", iter->material.shininess);
			mainRenderShader.setUniform("specInt", iter->material.specInt);
			mainRenderShader.setUniform("ambInt", iter->material.ambInt);
			mainRenderShader.setUniform("difInt", iter->material.difInt);

			mainRenderTex.draw(scene.layers.begin()->sprite, renderState);

			mainRenderTex.display();
			mainSprite.setTexture(mainRenderTex.getTexture());
			mainWindow.draw(mainSprite, mainRenderState);
		}
		
		dragLayers();
		mainWindowDrawing();

		if (scene.currentLayer != scene.lastActiveLayer) {
			for (auto iter = std::next(scene.currentLayer); iter <= scene.lastActiveLayer; std::advance(iter, 1)) {
				mainRenderShader.setUniform("normalMap", sf::Shader::CurrentTexture);
				mainRenderShader.setUniform("layerTex", iter->tex);

				mainRenderShader.setUniform("lightPos", scene.lightSource.pos);
				mainRenderShader.setUniform("lightCol", scene.lightSource.col);

				mainRenderShader.setUniform("shininess", iter->material.shininess);
				mainRenderShader.setUniform("specInt", iter->material.specInt);
				mainRenderShader.setUniform("ambInt", iter->material.ambInt);
				mainRenderShader.setUniform("difInt", iter->material.difInt);

				mainRenderTex.draw(scene.layers.begin()->sprite, renderState);

				mainRenderTex.display();
				mainSprite.setTexture(mainRenderTex.getTexture());
				mainWindow.draw(mainSprite, mainRenderState);
			}
		}

		

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
	scene.renderDrawingLayer();
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
