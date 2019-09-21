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
	static bool isBufferBeingReset;

	static std::vector<sf::Vector2i> positions;

	static void update(std::vector<BrushPntr>::iterator brush) {
		if (isFirstStamp && isBufferBeingCleared && !isBufferBeingReset) {
			clearBuffer();
		} 
		else if(!positions.empty() && positions.size() < 50 && !isBufferBeingReset) {
			if (currentCursorDistance() > (**brush).computeRelativeStepSize()) {
				positions.push_back(sf::Mouse::getPosition(mainWindow));
			}
		}
		else if (positions.size() >= 50) { //Clear the buffer in case it becomes too big
			isBufferBeingCleared = true;
		}
	}

	static void clearBuffer()
	{
		positions.clear();
		positions.reserve(50);
		positions.push_back(sf::Mouse::getPosition(mainWindow));
		isBufferBeingCleared = false;
	}

	//We have to make sure that the vector is never empty when we try to access it
	static void reset(int positionsToBeRemoved) {
		isBufferBeingReset = true;
		if (!positions.empty()) {
			for (int i = 0; i < positionsToBeRemoved - 1; i++) {
				positions.erase(positions.begin());
			}
			positions.reserve(50);
		}
		isBufferBeingReset = false;
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
bool CursorBuffer::isBufferBeingReset = false;
std::vector<sf::Vector2i> CursorBuffer::positions = {};

CursorBuffer::CursorBuffer()
{
}


CursorBuffer::~CursorBuffer()
{
}
