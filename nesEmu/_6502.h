#pragma once
#include <iterator>
#include <algorithm>
#include <map>
#include <iostream>
#include "Controller.h"
#include "ppu.h"
// NOTES
// instructions range from 8 to 24 bits
// 8 bit opcode, 8 bit param 1, 8 bit param 2
// you just have to know wich ones use wich params and treat aprotiratally

class _6502
{
// all CPU instructions done no interrupt or decimal mode yet
public:
	uint16_t memoryLen = 0xFFFF;
	// porgam counter
	uint16_t pc = 0;
	// acumulator (your universal 8 bit register)
	uint8_t ac = 0xDE;
	// x, y registers (no logic just count up and down)
	uint8_t x = 0;
	uint8_t y = 0xF2;
	// status (P) register
	// NO-BDIZC
	// N: negitive
	// O: overflow (does not fit into a signed byte)
	// -: ignored
	// B: break
	// D: Decimal
	// I: Interrupt
	// Z: Zero
	// C : carry
	uint8_t SR = 0x07;
	// stack pointer the stack goes down not up
	uint8_t sp = 0xFF;
	// stack can only pus a register and P register
	// stack is stored at  ($0100-$01FF)
	uint16_t stackAddr = 0x0100;
	// page zero (first 256 entries) is an optimised memory space
	uint8_t (&memory)[0x10000];
	// tells the cpu how much to increment the stackpointer after each instruction
	int8_t instrSize = 0;
	
	// the adress the cpu is trying to write too
	// if the cpu does not write to a ppu addr it is 0
	uint16_t ppuEnable = 0;
	// did we read or write
	bool ppuRead = false;

	struct memoryData {
		uint16_t memData;
		uint16_t addrData;
		uint8_t size;
	};

	uint8_t p_instr = 0;

	// NMI address
	uint16_t NMI_addr = 0;

	std::map<uint8_t, uint8_t> cycleMap;

	// a reference to the controller
	Controller* controller = nullptr;
	ppu* p = nullptr;

	_6502(uint8_t (&mem)[0x10000], Controller* c) : memory(mem) {
		loadCycleMap();
		controller = c;
	}

	// stores how many cycles since ppu was last used
	// starts at 7 because of the interrupt
	int cycleCount = 7;

	void loadCycleMap();
	void step();
	void eval(unsigned int instr, uint8_t p1, uint8_t p2);
	void groupOneEval(uint8_t instr, uint8_t param1, uint8_t param2);
	void groupTwoEval(uint8_t instr, uint8_t param1, uint8_t param2);
	void groupThreeEval(uint8_t instr, uint8_t param1, uint8_t param2);
	void memWrite(uint16_t addr, uint8_t v);
	void illegalEval(uint8_t instr, uint8_t param1, uint8_t param2);
	// no param two cuz branches are the same
	void branchEval(int8_t instr, int8_t param1);
	// single byte and subroutines
	void otherEval(uint8_t instr, uint8_t param1, uint8_t param2);
	void setFlags(uint16_t num, bool carry, bool negitive, bool zero, bool overflow);
	// returns the bit mask that is anded with the branch
	uint8_t branchShiftData(uint8_t insr);
	memoryData getGroupOneMemData(uint8_t addressing,uint8_t param1, uint8_t param2);
	// also works for three three has no acumulator
	memoryData getGroupTwoMemData(uint8_t addressing, uint8_t param1, uint8_t param2);

	// sets ppu enable given an address
	void evalSpecialRegs(uint16_t addrData, bool read);
	void evalControllerRegs(uint16_t addrData, bool read, uint16_t* memD);


	// helpers
	void compare(uint8_t a, uint8_t b);
	void addWCarry(uint8_t data);
	void ROR(uint8_t instr, uint16_t addr, uint8_t data);
	void ROL(uint8_t instr, uint16_t addr, uint8_t data);
	void LAX(uint8_t data);
	void SAX(uint16_t data);
	void NMI();
};

