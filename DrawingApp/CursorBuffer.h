#pragma once

#include "Scene.h"
#include "GlobalVariables.h"

typedef std::unique_ptr<Brush> BrushPntr;

//TODO: Maybe create a struct here that contains the pen pressure as well
class CursorBuffer
{
public:
	static bool isFirstStamp;
	static bool isBufferBeingCleared;

	static std::vector<sf::Vector2i> positions;

	static void update(std::vector<BrushPntr>::iterator brush) {
		if (isFirstStamp && isBufferBeingCleared) {
			positions.clear();
			positions.reserve(20);
			positions.push_back(sf::Mouse::getPosition(mainWindow));
			isBufferBeingCleared = false;
		} else if(!positions.empty()) {
			if (currentCursorDistance() > (**brush).computeRelativeStepSize()) {
				positions.push_back(sf::Mouse::getPosition(mainWindow));
			}
		}
	}

	//We have to make sure that the vector is never empty when we try to access it
	static void reset() {
		if (!positions.empty()) {
			positions.front() = positions.back(); 
			auto iter = positions.begin() + 1;
			while (iter != positions.end()) {
				iter = positions.erase(iter);
			}
		}
	}
	
	CursorBuffer();
	~CursorBuffer();

private:
	static float currentCursorDistance() {
		sf::Vector2i cursorPos = sf::Mouse::getPosition(mainWindow);
		return sqrtf(powf((positions.back().x - cursorPos.x), 2.0f) + powf(positions.back().y - cursorPos.y, 2.0f));
	}
};
bool CursorBuffer::isFirstStamp = true;
bool CursorBuffer::isBufferBeingCleared = true;
std::vector<sf::Vector2i> CursorBuffer::positions = {};

CursorBuffer::CursorBuffer()
{
}


CursorBuffer::~CursorBuffer()
{
}
