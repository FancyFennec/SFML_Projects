#pragma once

#include "Scene.h"
#include "GlobalVariables.h"

typedef std::unique_ptr<Brush> BrushPntr;

class CursorBuffer
{
public:
	static bool isFirstStamp;
	static bool isBufferBeingCleared;
	static bool isBufferBeingReset;

	static std::vector<sf::Vector2i> positions;

	static void update(std::vector<BrushPntr>::iterator brush) {
		if (positions.size() >= MAX_POSITIONS) { //Clear the buffer in case it becomes too big
		isBufferBeingCleared = true;
		}
		if (!isBufferBeingReset) {
			if (isFirstStamp || isBufferBeingCleared) {
				clearBuffer();
			}
			else if (!positions.empty() && positions.size() < MAX_POSITIONS) {
				if (currentCursorDistance() > (**brush).computeRelativeStepSize()) {
					positions.push_back(sf::Mouse::getPosition(mainWindow));
				}
			}
		}
	}

	static void clearBuffer()
	{
		positions.clear();
		positions.reserve(MAX_POSITIONS);
		positions.push_back(sf::Mouse::getPosition(mainWindow));
		isBufferBeingCleared = false;
	}

	static void reset(int positionsToBeRemoved) {
		isBufferBeingReset = true; //We have to make sure that the vector is never empty when we try to access it
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
		return sqrtf(powf((positions.back().x - sf::Mouse::getPosition(mainWindow).x), 2.0f) + powf(positions.back().y - sf::Mouse::getPosition(mainWindow).y, 2.0f));
	}
	static int MAX_POSITIONS;
};

int CursorBuffer::MAX_POSITIONS = 50;
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
