#include "Controller.h"

uint8_t Controller::read() {
	uint8_t value = controller & (0b1 << shiftVal);
	//std::cout << "READING";
	shiftVal++;

	if (value > 0) {
		return 0b00000001;
	}
	else {
		return 0;
	}

}

void Controller::write(uint8_t value) {
	//std::cout << "WRITE: " << unsigned(value) << std::endl;
	if ((value & 1)== 0) {
		isPolling = false;
		updateKeys();
	}
	else {
		isPolling = true;
	}
	shiftVal = 0;

}

void Controller::updateKeys() {
	//std::cout << "CHECKING";
	const Uint8* keys = SDL_GetKeyboardState(nullptr);
	controller = 0;

	if (isPolling) {
		shiftVal = 0;
	}

	if (keys[SDL_SCANCODE_RIGHT]) {
		controller = controller | 0x8;
	}

	if (keys[SDL_SCANCODE_LEFT]) {
		controller = controller | 0x4;
	}

	if (keys[SDL_SCANCODE_DOWN]) {
		controller = controller | 0x1;
	}

	if (keys[SDL_SCANCODE_UP]) {
		controller = controller | 0x2;
	}

	if (keys[SDL_SCANCODE_D]) {
		controller = controller | 0x80;
	}

	if (keys[SDL_SCANCODE_W]) {
		controller = controller | 0x10;
	}

	if (keys[SDL_SCANCODE_A]) {
		controller = controller | 0x40;
	}

	if (keys[SDL_SCANCODE_S]) {
		controller = controller | 0x20;
	}

}