#pragma once

#include "GlobalVariables.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "Settings.h"
#include "Scene.h"
#include "CommandManager.h"

void mainWindowEventHandling(Scene& scene);
void brushWindowEventHandling(Scene& scene);
void lmbPressed(Scene& scene);

void mainWindowEventHandling(Scene& scene)
{
	if (event.type == sf::Event::Closed)
		mainWindow.close();
	if (event.type == sf::Event::GainedFocus) {
		setMouseNotHeld();
		setAltNotHeld();
		setCtrlNotHeld();
	}
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			lmbPressed(scene);
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (isMouseHeld()) {
				sf::Texture oldTexture = scene.currentLayer->tex;
				scene.currentLayer->blendlayers(scene.drawingLayer, scene.currentBrush);
				CommandManager::updateLayer(oldTexture);
				setMouseNotHeld();
			}
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
		case(sf::Keyboard::Q): {
			break;
		}
		case(sf::Keyboard::LAlt): {
			setAltIsHeld();
			break;
		}
		case(sf::Keyboard::LControl): {
			setCtrlIsHeld();
			break;
		}
		case(sf::Keyboard::Z): {
			if (isCtrlHeld()) {
				CommandManager::moveBackward();
			}
			break;
		}
		case(sf::Keyboard::Y): {
			if (isCtrlHeld()) {
				CommandManager::moveForward();
			}
			break;
		}
		case(sf::Keyboard::LShift): {
			//TODO: Somehow on my laptop there are som weird effecets when I use space instead of lshift
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
			//mainWindow.close();
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
		case(sf::Keyboard::LShift): {
			setSpaceNotHeld();
			break;
		}
		}
	}
}

void brushWindowEventHandling(Scene& scene)
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

void lmbPressed(Scene& scene)
{
	if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
		CommandManager::clearActions();
		scene.drawingLayer.clearLayer();

		if (isAltHeld()) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				scene.pickColor(mainWindow);
			}
		}
		else {
			setMouseIsHeld();
			scene.resetCursorPositions(mainWindow, scene.drawingLayer);
			//No need to draw the window here, it gets drawn because the lmb is being held later
		}
	}
}