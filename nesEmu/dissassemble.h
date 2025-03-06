#pragma once
#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>
#include <vector>
#include "_6502.h"

class Dissassembler {
public:
	struct addrData {
		uint8_t size;
		std::string prefix, post;
	};

	int currInstr =0;

	int pc = 0;
	addrData Acume = { 0, "A", "" };
	addrData Abs = { 2, "$", "" };
	addrData AbsX = { 2, "$", ",X" };
	addrData AbsY = { 2, "$",  ",X" };
	addrData Immediate = { 1, "#$", "" };
	addrData Implied = { 0, "", "" };
	addrData Indr = { 2, "($", ")" };
	addrData XIndr = { 1, "($", "X,)" };
	addrData IndrY = { 2, "($", "),Y" };
	addrData Rel = { 1, "$", "" };
	addrData Zpg = { 1, "$",  "" };
	addrData ZpgX = { 1, "$", ",X" };
	addrData ZpgY = { 1, "$", ",Y" };

	struct Instr {
		std::string name;
		addrData addr;
	};

	std::map < uint16_t, std::string > regMap;
	std::map<uint8_t, Instr> InstrMap;

	Dissassembler();
	std::string dissassemble(uint8_t instr, uint8_t p1, uint8_t p2);

	std::vector<std::string> dissassembleAll(_6502* cpu);
};
