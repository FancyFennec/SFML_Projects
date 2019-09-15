#pragma once

#include "GlobalVariables.h"
#include "Scene.h"

//TODO: Maybe create a struct here that contains the pen pressure as well
class CursorBufferUtils
{
public:
	static bool isFirstStamp;
	static std::vector<sf::Vector2i> cursorBuffer;

	void initializeBuffer() {
		isFirstStamp = true;
		cursorBuffer.clear();
		cursorBuffer.push_back(sf::Mouse::getPosition(mainWindow));
	}

	void updateBuffer(Scene& scene) {
		isFirstStamp = false;
		if (currentCursorDistance() > (**scene.currentBrush).computeRelativeStepSize) {
			cursorBuffer.push_back(sf::Mouse::getPosition(mainWindow));
		}
	}
	
	void resetBuffer() {
		if (cursorBuffer.size() == 0 || cursorBuffer.size() == 1)
			return;

		auto iter = cursorBuffer.begin(); 
		while (iter != std::prev(cursorBuffer.end())) {
			iter = cursorBuffer.erase(iter);
		}
	}

	CursorBufferUtils();
	~CursorBufferUtils();

private:
	float currentCursorDistance() {
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
