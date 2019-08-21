#include <iostream>
#include <vector>
#include <windows.h>


int main() {

	MSG msg;

	HWND consoleWindow =  GetConsoleWindow();

	while (GetMessage(&msg, consoleWindow, 0, 0))
	{
		std::cout << "abc" << std::endl;
		std::cout << msg.time << std::endl;
	}

	return 0;
	
}