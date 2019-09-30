#pragma once

#include "Scene.h"
#include "GlobalVariables.h"

typedef std::unique_ptr<Brush> BrushPntr;

//TODO: Maybe create a struct here that contains the pen pressure as well
class CursorBuffer
{
public:
	static bool isFirstStamp;
	static bool useBuffer1;
	static bool useBuffer2;
	static bool resetBuffer1;
	static bool resetBuffer2;

	static std::vector<sf::Vector2i> buffer1;
	static std::vector<sf::Vector2i> buffer2;

	static void update(std::vector<BrushPntr>::iterator brush) {
		if (isFirstStamp && !useBuffer1) {
			clearBuffers();
		} 
		else if(useBuffer1) {
			if (currentCursorDistance() > (**brush).computeRelativeStepSize()) {
				buffer1.push_back(sf::Mouse::getPosition(mainWindow));
				if (!resetBuffer2) {
					buffer2.front() = sf::Mouse::getPosition(mainWindow);
				}
			}
		}
		else if (useBuffer2) {
			if (currentCursorDistance() > (**brush).computeRelativeStepSize()) {
				buffer1.push_back(sf::Mouse::getPosition(mainWindow));
				if (!resetBuffer1) {
					buffer1.front() = sf::Mouse::getPosition(mainWindow);
				}
			}
		}
		resetBuffers();
	}

	static void clearBuffers()
	{
		buffer1.clear();
		buffer1.reserve(50);
		buffer2.clear();
		buffer2.reserve(50);
		
		buffer1.push_back(sf::Mouse::getPosition(mainWindow));
		buffer2.push_back(sf::Mouse::getPosition(mainWindow));

		useBuffer1 = true;
		useBuffer1 = false;
		resetBuffer1 = false;
		resetBuffer2 = false;
	}

	//We have to make sure that the vector is never empty when we try to access it
	static void resetBuffers() {
		if (resetBuffer1) {
			buffer1.clear();
			buffer1.push_back(sf::Mouse::getPosition(mainWindow));
			resetBuffer1 = false;
		}
		if (resetBuffer2) {
			buffer2.clear();
			buffer2.push_back(sf::Mouse::getPosition(mainWindow));
			resetBuffer2 = false;
		}
	}
	
	CursorBuffer();
	~CursorBuffer();

private:
	static float currentCursorDistance() {
		sf::Vector2i cursorPos = sf::Mouse::getPosition(mainWindow);
		if (useBuffer1) {
			return sqrtf(powf((buffer1.back().x - cursorPos.x), 2.0f) + powf(buffer1.back().y - cursorPos.y, 2.0f));
		}
		else {
			return sqrtf(powf((buffer2.back().x - cursorPos.x), 2.0f) + powf(buffer2.back().y - cursorPos.y, 2.0f));
		}
		
	}
};

bool CursorBuffer::isFirstStamp = true;
bool CursorBuffer::useBuffer1 = false;
bool CursorBuffer::useBuffer2 = false;
bool CursorBuffer::resetBuffer1 = false;
bool CursorBuffer::resetBuffer2 = false;
std::vector<sf::Vector2i> CursorBuffer::buffer1 = {};
std::vector<sf::Vector2i> CursorBuffer::buffer2 = {};

CursorBuffer::CursorBuffer()
{
}


CursorBuffer::~CursorBuffer()
{
}
