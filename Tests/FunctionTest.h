#pragma once

int otherFunction();

int testFunction() {
	return otherFunction();
};

int otherFunction() {
	return 2;
}