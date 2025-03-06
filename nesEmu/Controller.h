#pragma once
#include <iostream>
#include "SDL_keyboard.h"
class Controller
{
public:
	// a b select start up down left right
	uint8_t controller = 0;
	// how much we shifted
	uint8_t shiftVal = 0;
	// is polling
	bool isPolling = true;

	uint8_t read();
	void write(uint8_t value);

	void updateKeys();
};

