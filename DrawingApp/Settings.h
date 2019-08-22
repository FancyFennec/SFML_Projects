#pragma once

int BIT(int i) { return 1 << i; }

long int SETTINGS = 0;
int LMB_PRESSED = BIT(1);
int LMB_RELEASED = BIT(2);
int MOUSE_IS_HELD = BIT(3);
int CTRL_IS_HELD = BIT(4);
int ALT_IS_HELD = BIT(5);

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