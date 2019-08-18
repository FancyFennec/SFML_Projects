#include <iostream>
#include <vector>
#include <windows.h>

typedef int(*Msg)(HWND, LPCTSTR, LPCTSTR, UINT);

int main() {

	HINSTANCE hDLL = LoadLibrary("User32.dll");
	if (hDLL == NULL) {
		std::cout << "Failed to load the library.\n";
	}
	else {
		std::cout << "Library loaded.\n";
		/*Msg MsgBox = (Msg)GetProcAddress(hDLL, "MessageBoxA");
		MsgBox(0, "https://HelloACM.com", "Hello, World!", 0);*/
	}
	FreeLibrary(hDLL);

	
	UINT32 pointerId = GET_POINTERID_WPARAM(hDLL);

	//UINT32 pointerId = UINT32(1);
	POINTER_PEN_INFO peninfo;
	
	std::cout << GetPointerPenInfo(pointerId, &peninfo) << std::endl;

	return 0;
	
}