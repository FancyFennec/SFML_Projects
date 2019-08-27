#pragma once

int BIT(int i) { return 1 << i; }

static int SCREEN_WIDTH = 1200;
static int SCREEN_HEIGHT = 800;

static long int SETTINGS = 0;
static int LMB_PRESSED = BIT(1);
static int LMB_RELEASED = BIT(2);
static int MOUSE_IS_HELD = BIT(3);
static int CTRL_IS_HELD = BIT(4);
static int ALT_IS_HELD = BIT(5);
static int SPACE_IS_HELD = BIT(6);

enum REVERTABLE_ACTION {
	CREATE_LAYER,
	DELETE_LAYER,
	UPDATE_LAYER
};

bool wasLMBPressed() {
	return SETTINGS & LMB_PRESSED;
}

void setLMBPressed() {
	SETTINGS |= LMB_PRESSED;
}

void setLMBNotPressed() {
	SETTINGS &= ~LMB_PRESSED;
}

bool wasLMBReleased() {
	return SETTINGS & LMB_RELEASED;
}

void setLMBReleased() {
	SETTINGS |= LMB_RELEASED;
}

void setLMBNotReleased() {
	SETTINGS &= ~LMB_RELEASED;
}

bool isMouseHeld() {
	return SETTINGS & MOUSE_IS_HELD;
}

void setMouseIsHeld() {
	SETTINGS |= MOUSE_IS_HELD;
}

void setMouseNotHeld() {
	SETTINGS &= ~MOUSE_IS_HELD;
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