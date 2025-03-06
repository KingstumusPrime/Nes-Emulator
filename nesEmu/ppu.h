#pragma once
#include <iostream>
#include <map>
class ppu
{
private:
	// the last value written
	uint8_t ioDataBus = 0;
public:
	// a reference to memory used by cpu && ppu
	uint8_t(&memory)[0x10000];

	// V P H B S I NN
	// V: NMI enavle
	// P: ppu master/slave
	// H: sprite height
	// B: background tile select
	// S: sprite tile select
	// I: increment mode
	// N: nametable select
	uint8_t ctrl = 0;
	
	// BGR s b M m G
	// BGR: color emphasis
	// s: sprite enable
	// b: background enable
	// M: sprite left column enable
	// m: background left column enable
	// G: greyscale
	uint8_t mask = 0;

	// V S O
	// V: v blank
	// S: sprite 0 hit
	// sprite overflow
	// read resets write pair for $2005/$2006 (whatever that means)
	uint8_t* status = &memory[0x2002];
	// oam read/write addr
	uint16_t oam_addr = 0;
	
	// oam data
	uint8_t oam_data = 0;

	// ppu scroll
	uint8_t scrollX = 0;
	uint8_t scrollY = 0;
	// ppu read/write address
	// two writes: least significant byte, most significant byte
	uint16_t vramAddr = 0;
	uint8_t vramHigh = 0;

	// data read/write
	uint8_t data = 0;

	// OAM DMA high addr
	// the data is copied like flood fill and this specifies the first 8 bits to start the fill
	// the oam is then filled with XX00 to xxFF
	uint8_t dma_addr = 0;

	// register is used to tell wether or not you are on first or second write
	bool secondWrite = false;

	// pattern table (raw sprite data)
	//uint8_t patternTableL[512];
	//uint8_t patternTableR[512];
	// the current pattern table selected
	//uint8_t (*patternTable)[512] = &patternTableL;
	uint8_t patternTable[8192] = { 0 };
	// nametable (the background)
	uint8_t vram[0x10000] = { 0 };


	uint8_t oam[256] = { 0 };

	// the mirroring mode set by the mapper
	bool vMirror = true;

	// T and V as described in loopys scroll docs
	uint16_t v = 0;
	uint16_t t = 0;


	// MEM MAP
	//  0x0000 - 0x1FFF  CHR ROM(Bitmap pattern data)
	//  0x2000 - 0x23FF  Nametable 1  Attribute @ 0x23C0
	//	0x2400 - 0x27FF  Nametable 2  Attribute @ 0x27C0
	//	0x2800 - 0x2BFF  Nametable 3  Attribute @ 0x2BC0
	//	0x2C00 - 0x2FFF  Nametable 4  Attribute @ 0x2FC0
	//	0x3000 - 0x3F00  Mirror of nametables
	//	0x3F00 - 0x3F1F  Palette colors

	// constructor just copies memory into internal memory
	ppu(uint8_t(&mem)[0x10000]) : memory(mem) {
	}

	// function is called when cpu writes to the ppu registers
	void evaluatePPURegisters(uint16_t addr, bool read);
	
	void getVal(uint16_t addr, uint16_t* value);
	
	// increment x and y as desccribed in "The Skinney On NES Scrolling"6
	void incX();
	void incY();
};

