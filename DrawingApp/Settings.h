#pragma once

int BIT(int i) { return 1 << i; }

static bool SHOW_GUI = true;

static const char* BRUSH_DIRECTORY = "./Brushes/";
static const char* SCENES_DIRECTORY = "./Scenes/";

static bool USE_FULLSCREEN = false;
static unsigned int FPS = 120;

static int WINDOW_WIDTH = 1200;
static int WINDOW_HEIGHT = 800;

const static int MAX_ACTIONS = 10;
const static int MAX_LAYERS = 20;
static unsigned int DOUBLE_CLICK_SPEED = 300;

static long int SETTINGS = 0;
static int LMB_IS_HELD = BIT(0);
static int CTRL_IS_HELD = BIT(1);
static int ALT_IS_HELD = BIT(2);
static int SPACE_IS_HELD = BIT(3);

void getDesktopResolution(int& horizontal, int& vertical) {

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	horizontal = GetSystemMetrics(SM_CXSCREEN);
	vertical = GetSystemMetrics(SM_CYSCREEN);
}

bool isMouseHeld() {
	return SETTINGS & LMB_IS_HELD;
}

void setMouseIsHeld() {
	SETTINGS |= LMB_IS_HELD;
}

void setMouseNotHeld() {
	SETTINGS &= ~LMB_IS_HELD;
}

bool isCtrlHeld() {
	return SETTINGS & CTRL_IS_HELD;
}

void setCtrlIsHeld() {
	SETTINGS |= CTRL_IS_HELD;
}

void setCtrlNotHeld() {
	SETTINGS &= ~CTRL_IS_HELD;
}

bool isAltHeld() {
	return SETTINGS & ALT_IS_HELD;
}

void setAltIsHeld() {
	SETTINGS |= ALT_IS_HELD;
}

void setAltNotHeld() {
	SETTINGS &= ~ALT_IS_HELD;
}

bool isSpaceHeld() {
	return SETTINGS & SPACE_IS_HELD;
}

void setSpaceIsHeld() {
	SETTINGS |= SPACE_IS_HELD;
}

void setSpaceNotHeld() {
	SETTINGS &= ~SPACE_IS_HELD;
}