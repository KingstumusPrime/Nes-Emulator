#pragma once
#include "SDL.h"
#include "ppu.h"
class PpuRenderer
{
private:
	// place holder on how many cycles before the VBLANK
	int frameCycles = 0;

	const int pixelSize = 1;


	Uint32* pixels;
	int pitch = 0;

	// default size per a pixel
	SDL_Rect square = { 0, 0, pixelSize, pixelSize };

	// internal mask for renderering
	uint8_t mask = 0b10000000;


	// placeholder positions used to detect when to go to a new line
	int y = 0;
	
	uint8_t sprID = 0;

	uint8_t lowData = 0;
	uint8_t highData = 0;

	int8_t pixelNum = 0;
	
	uint16_t highShift = 0;
	uint16_t lowShift = 0;
	Uint32 pixelColor = 0;
	uint8_t xscroll = 0;

	uint8_t palQuad;
	uint16_t bufferH = 0;
	uint16_t bufferL = 0;

	bool spriteData = false;

	uint8_t sprLowData = 0;
	uint8_t sprHighData = 0;

	// colors
	Uint32 RED =0;
	Uint32 GREEN =0;
	Uint32 BLUE =0;
	SDL_PixelFormat f;

	// the index of sprite overflow greater greater than seven if sprite zero is not on line
	uint8_t spr0 = 0xFF;
	uint8_t palleteNum = 0;
	uint16_t palletteShift = 0;

	uint16_t nameTableAddr = 0;
	
	// set to 2 decremented once on the first scanline then the scanline after is 0
	uint8_t zeroNextLine = 0;

	// zero this frame is pretty much the same
	uint8_t zeroThisFrame = 0;
public:
	int cycles = 0;
	int scanlines = 0;

	int frames = 0;

	int x = 0;

	SDL_Renderer* r= nullptr;
	ppu* p = nullptr;

	SDL_Texture* texture = nullptr;


	PpuRenderer(ppu* PPU, SDL_Renderer* renderer);
	void drawOAM();
	void cycle();
	void oldCycle();
	void drawCycle(uint8_t data, uint8_t palleteNum);
	void reset();

	void drawSprite(int sprID, bool xFlip, bool yflip);
	void drawRow(uint16_t data1, uint16_t data2, bool xFlip);
	void spriteEval();

	uint8_t sprsOnLine[64];

	int sx = 0;

	// maps a int to a SDL color
	// filled by the helped load Pallette
	std::map<uint8_t, SDL_Color> paletteMap;
};

void drawBlock(SDL_Renderer* r, uint8_t data, SDL_Rect square);

void drawRGB(SDL_Renderer* r, SDL_Rect square, SDL_Color c);
void drawRow(SDL_Renderer* r, uint16_t data1, uint16_t data2, SDL_Rect* box, int size, bool xFlip=false);

void drawSprite(SDL_Renderer* r, int sprID, uint8_t chMEM[8192], SDL_Rect* square, int size, bool xFlip=false, bool yFlip=false );