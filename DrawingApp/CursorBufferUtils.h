#pragma once

#include "Scene.h"
#include "GlobalVariables.h"

typedef std::unique_ptr<Brush> BrushPntr;

//TODO: Maybe create a struct here that contains the pen pressure as well
class CursorBufferUtils
{
public:
	static bool isFirstStamp;
	static std::vector<sf::Vector2i> cursorBuffer;

	static void initializeBuffer() {
		isFirstStamp = true;
		cursorBuffer.clear();
		cursorBuffer.reserve(20);
		cursorBuffer.push_back(sf::Mouse::getPosition(mainWindow));
	}

	static void updateBuffer(std::vector<BrushPntr>::iterator brush) {
		if(!isFirstStamp && !cursorBuffer.empty()) {
			if (currentCursorDistance() > (**brush).computeRelativeStepSize()) {
				cursorBuffer.push_back(sf::Mouse::getPosition(mainWindow));
			}
		}
	}

	static void resetBuffer() {
		if (!cursorBuffer.empty()) {
			sf::Vector2i lastPos = cursorBuffer.back();
			cursorBuffer.clear();
			cursorBuffer.push_back(lastPos);
		}
	}
	
	CursorBufferUtils();
	~CursorBufferUtils();

private:
	static float currentCursorDistance() {
		sf::Vector2i cursorPos = sf::Mouse::getPosition(mainWindow);
		return sqrtf(powf((cursorBuffer.back().x - cursorPos.x), 2.0f) + powf(cursorBuffer.back().y - cursorPos.y, 2.0f));
	}
};
bool CursorBufferUtils::isFirstStamp = true;
std::vector<sf::Vector2i> CursorBufferUtils::cursorBuffer = {};

CursorBufferUtils::CursorBufferUtils()
{
}


CursorBufferUtils::~CursorBufferUtils()
{
}
