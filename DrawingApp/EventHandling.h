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
sf::Color getAltSampledColor(Scene& scene);
sf::Color getAltSampledNormal(Scene& scene);

void mainWindowEventHandling(Scene& scene)
{
	if (event.type == sf::Event::Closed)
		mainWindow.close();
	if (event.type == sf::Event::GainedFocus) {
		setMouseNotHeld();
		setAltNotHeld();
		setCtrlNotHeld();
	}
	if (event.type == sf::Event::Resized) {
		sf::Vector2u newSize = mainWindow.getSize();
		WINDOW_WIDTH = newSize.x;
		WINDOW_HEIGHT = newSize.y;
		//TODO: offset has to be adjusted too
	}
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (!isMouseHeld()) lmbPressed(scene);
		}
	}
	if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (isMouseHeld()) {
				switch (DRAWING_STATE) {
				case(DrawingState::ALPHA): {
					sf::Texture oldTexture = scene.currentLayer->tex;
					scene.currentLayer->blendlayers(scene.drawingLayer, scene.currentBrush);
					CommandManager::updateLayer(oldTexture);
					break;
				}
				case(DrawingState::NORMAL): {
					sf::Texture oldTexture = scene.layers.begin()->tex;
					scene.normalLayer.blendlayers(scene.drawingLayer, scene.currentBrush);
					CommandManager::updateLayer(oldTexture);
				}
				}
				
				scene.drawingLayer.clearLayer();
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
			scene.lightSource.movePos(sf::Vector2f(0,-30));
			break;
		}
		case(sf::Keyboard::Down): {
			scene.lightSource.movePos(sf::Vector2f(0, 30));
			break;
		}
		case(sf::Keyboard::Right): {
			scene.lightSource.movePos(sf::Vector2f(30, 0));
			break;
		}
		case(sf::Keyboard::Left): {
			scene.lightSource.movePos(sf::Vector2f(-30, 0));
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

		if (isAltHeld()) {
			switch (DRAWING_STATE)
			{
			case DrawingState::ALPHA:
				(*scene.currentBrush)->currentColor = getAltSampledColor(scene);
				(*scene.currentBrush)->synchronizeGuiBrushColor();
				break;
			case DrawingState::NORMAL:
				(*scene.currentBrush)->currentNormal = getAltSampledNormal(scene);
				break;
			default:
				break;
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

sf::Color getAltSampledColor(Scene& scene) {
	samplingTexture.update(mainWindow);
	sampledPos = sf::Mouse::getPosition(mainWindow);
	return samplingTexture.copyToImage().getPixel(sampledPos.x, sampledPos.y);
}

sf::Color getAltSampledNormal(Scene& scene) {
	sampledPos = sf::Mouse::getPosition(mainWindow) - scene.currentLayer->offset;
	sf::Image img = scene.normalLayer.tex.copyToImage();
	return img.getPixel(sampledPos.x, sampledPos.y);
}