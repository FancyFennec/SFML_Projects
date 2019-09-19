#pragma once

#include "GlobalVariables.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui-SFML.h"
#include "Settings.h"
#include "Scene.h"
#include "CommandManager.h"
#include "CursorBuffer.h"

void mainWindowEventHandling(Scene& scene);
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
		case(sf::Keyboard::X): { //Swap Colors
			sf::Color swapColor = (*scene.currentBrush)->currentColor;
			(*scene.currentBrush)->currentColor = (*scene.currentBrush)->previousColor;
			(*scene.currentBrush)->previousColor = swapColor;
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
		case(sf::Keyboard::Space): {
			setSpaceNotHeld();
			break;
		}
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
				sf::Vector2i pos = sf::Mouse::getPosition(mainWindow);

				sf::Texture newTex;
				newTex.create(mainWindow.getSize().x, mainWindow.getSize().y);
				newTex.update(mainWindow);
				(*scene.currentBrush)->currentColor = newTex.copyToImage().getPixel(pos.x, pos.y);
				(*scene.currentBrush)->updateGuiBrushColor();
			}
		}
		else {
			setMouseIsHeld();
			scene.cursorPositions[1] = sf::Mouse::getPosition(mainWindow);
			scene.cursorPositions[0] = scene.cursorPositions[1];
			CursorBuffer::isFirstStamp = true;
			CursorBuffer::isBufferBeingCleared = true;
		}
	}
}