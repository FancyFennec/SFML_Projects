#pragma once

int SETTINGS = 0;
int MOUSE_IS_HELD = 1 << 1;
int CTRL_IS_HELD = 1 << 2;
int ALT_IS_HELD = 1 << 3;

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