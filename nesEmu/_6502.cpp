#include "_6502.h"

void _6502::memWrite(uint16_t addr, uint8_t v) {
	if (addr >= 0x800 && addr <= 0x1FFF) {
		memory[(addr & 0x7FF)] = v;
		memory[0x800 + (addr & 0x7FF)] = v;
		memory[0x1000 + (addr & 0x7FF)] = v;
		memory[0x1800 + (addr & 0x7FF)] = v;
	}
	else {
		memory[addr] = v;
	}
	
	//evalSpecialRegs(addr, false);

}

void _6502::step() {
	uint8_t p1 = 0;
	uint8_t p2 = 0;
	if (pc < memoryLen - 1) {
		p1 = memory[pc + 1];
	}
	else {
		p1 = 0;
	}
		
	if (pc < memoryLen - 2) {
		p2 = memory[pc + 2];
	}
	else
	{
		p2 = 0;
	}

	eval(memory[pc], p1, p2);

}

// https://www.masswerk.at/6502/6502_instruction_set.html
void _6502::eval(unsigned int instr, uint8_t p1, uint8_t p2) {
	// reset ppuEanble with each instruction
	ppuEnable = 0;
	uint8_t lowNibble = instr & 0xFF;
	uint8_t highNibble = instr >> 8;

	// add to cycles
	if (cycleMap[instr] == 0) {
		//std::cout << "IS ZERO " << unsigned(instr) << " : " << unsigned(p_instr) << std::endl;
	}
	cycleCount += cycleMap[instr];

	// change to two bits
	uint8_t cc = instr & 0b00000011;
	if ( ((instr & 0xF) == 0x8 || (instr & 0xF) == 0xA || instr == 0x00 || instr == 0x20 || instr == 0x40 || instr == 0x60) && instr != 0x4A && instr != 0x0A && instr != 0x6A && instr != 0x2A) {
		otherEval(instr, p1, p2);
	}else if (cc == 0b01) {
		groupOneEval(instr, p1, p2);
	}
	else if (cc == 0b10) {
		groupTwoEval(instr, p1, p2);
	}
	else if ((instr & 0b00011111) == 0b10000) {
		branchEval(instr, p1);
	}
	else if(cc == 0) {
		groupThreeEval(instr, p1, p2);
	}
	else if (cc == 0b11) {
		illegalEval(instr, p1, p2);
	}
	p_instr = instr;
}

uint8_t _6502::branchShiftData(uint8_t insr) {
	uint8_t shift = 0b00000000;
	switch (insr >> 6)
	{
	case 0b00:
		// negitive
		shift = 7;
		break;
	case 0b01:
		// overflow
		shift = 6;
		break;
	case 0b10:
		// carry
		shift = 0;
		break;
	case 0b11:
		// zero
		shift = 1;
		break;
	default:
		break;
	}
	return shift;
}

_6502::memoryData _6502::getGroupOneMemData(uint8_t addressing, uint8_t param1, uint8_t param2) {
	// 000: (zero page, x) get the data stored at ($const + x). Example: x=B, ($80, X) ($80 + x) = 8B read from 8B in memory that is the pointer then get the value from the pointer
	// 001: zero page (get data at 8 bit zero page)
	// 010: #immediate (a constant)
	// 011: absolute (get data at 16 bit address)
	// 100: (zero page), Y get the data at ($const) then move by Y. Example Y=B ($80), Y. Get the value of the pointer at 80 lets say it is F. We get the data at F + Y or F + B so we get the data at 1A
	// 101: zero page, X (zero page + X) note this wraps around
	// 110: absolute, Y (absoulte + Y)
	memoryData result;
	// just a placeholder
	uint16_t addr = 0;
	switch (addressing)
	{
	case 0b000:
		// (zero page, x)
		addr = uint16_t((memory[uint8_t(param1 + x + 1)] << 8) + memory[uint8_t(param1 + x)]);
		result.memData = memory[addr];

		result.addrData = addr;
		result.size = 2;
		break;
	case 0b001:
		// zero page
		result.addrData = param1;
		result.memData = memory[param1];
		result.size = 2;
		break;
	case 0b010:
		// #immediate or just a constant
		result.addrData = param1;
		result.memData = param1;
		result.size = 2;
		break;
	case 0b011:
		// absoulte (give an address)
		result.addrData = uint16_t((param2 << 8) + param1);
		result.memData = memory[result.addrData];
		result.size = 3;
		break;
	case 0b100:
		// (zero page), Y
		addr = (memory[uint8_t(param1 + 1)] << 8) + memory[param1];
		result.memData = (uint8_t)memory[uint16_t(addr + uint8_t(y))];
		result.addrData = addr + y;
		result.size = 2;
		break;
	case 0b101:
		// zeropage, X
		result.memData = memory[uint8_t(param1 + x)];
		result.addrData = uint8_t(param1 + x);
		result.size = 2;
		break;
	case 0b110:
		//absoulte, Y
		addr = uint16_t((param2 << 8) + param1 + y);
		result.addrData = addr;
		result.memData = memory[addr];
		result.size = 3;
		break;
	case 0b111:
		// absolute, X

		addr = uint16_t(((param2 << 8) + param1));
		result.addrData = addr + x;
		//result.addrData = result.addrData%0xFFFF;

		result.memData = memory[result.addrData];
		result.size = 3;
		break;
	default:
		break;
	}
	p->getVal(result.addrData, &result.memData);
	return result;
}

_6502::memoryData _6502::getGroupTwoMemData(uint8_t addressing, uint8_t param1, uint8_t param2) {
	// 000: immediate
	// 001: zero page
	// 010: acumulator
	// 011: absolute
	// 101: zero page, X
	// 111: absolute, X

	// just a placeholder
	memoryData result;
	result.memData = 0;
	result.addrData = 0;
	uint16_t addr = 0;
	switch (addressing)
	{
	case 0b000:
		// immediate
		result.memData = param1;
		result.addrData = param1;
		result.size = 2;
		break;
	case 0b001:
		// zero page
		result.addrData = param1;
		result.memData = memory[param1];
		result.size = 2;
		break;
	case 0b010:
		// acumulator
		result.memData = ac;
		result.addrData = ac;
		result.size = 1;
		break;
	case 0b011:
		// absoulte (give an address)
		addr = uint16_t((param2 << 8) + param1);
		result.addrData = addr;
		result.memData = memory[addr];
		result.size = 3;
		break;
	case 0b101:
		// zeropage, X
		addr = uint8_t(param1 + x);
		result.memData = memory[addr];
		result.addrData = addr;
		result.size = 2;
		break;
	case 0b111:
		// absolute, X
		addr = uint16_t(((param2 << 8) + param1));
		result.addrData = addr + x;
		//result.addrData = result.addrData % 0xFFFF;
		result.memData = memory[result.addrData];
		result.size = 3;
		break;
	default:
		break;
	}
	p->getVal(result.addrData, &result.memData);
	return result;
}

void _6502::groupOneEval(uint8_t instr, uint8_t param1, uint8_t param2) {
	// three bit opcode
	uint8_t opcode = instr >> 5;
	// adressing mode (three bits)
	uint8_t addrMode = (instr >> 2) & 0b000111;
	// get the data from the addrmode
	memoryData d = getGroupOneMemData(addrMode, param1, param2);
	uint16_t addrData = d.addrData;
	uint16_t memData = d.memData;
	instrSize = d.size;

	// absolute, X (absolute + X)
	switch (opcode)
	{
	case 0b000:
		// ORA
		ac = ac | memData;
		setFlags(ac, false, true, true, false);
		break;
	case 0b001:
		// AND
		ac = ac & memData;
		setFlags(ac, false, true, true, false);
		break;
	case 0b010:
		// EOR (xor)
		ac = ac ^ memData;
		setFlags(ac, false, true, true, false);
		break;
	case 0b011:
		// ADC need to add the carry flag
		addWCarry(memData);
		evalSpecialRegs(addrData, true);
		break;
	case 0b100:
		// STA
		// store the acumulator
		evalControllerRegs(addrData, false, &memData);
		memWrite(addrData, ac);
		//memory[addrData] = ac;
		evalSpecialRegs(addrData, false);
		break;
	case 0b101:
		// LDA
		evalControllerRegs(addrData, true, &memData);
		ac = memData;
		setFlags(ac, false, true, true, false);
		evalSpecialRegs(addrData, true);
		break;

	case 0b110:
		// CMP is just subaratc without using the result
		compare(ac, memData);

		break;
	case 0b111:
		// SBC
		addWCarry(~memData);
		break;

	default:
		break;
	}
	pc += instrSize;
}

void _6502::groupTwoEval(uint8_t instr, uint8_t param1, uint8_t param2) {
	// three bit opcode
	uint8_t opcode = instr >> 5;
	// adressing mode (three bits)
	uint8_t addrMode = (instr >> 2) & 0b000111;
	// get the data from the addrmode
	memoryData d = getGroupTwoMemData(addrMode, param1, param2);
	uint16_t memData = d.memData;
	uint16_t addrData = d.addrData;
	instrSize = d.size;

	switch (opcode)
	{
	case 0b000:
		// ASL
		if (instr == 0x0A) {
			if ((ac & 0b10000000) > 0) {
				SR = SR | 0b00000001;
			}
			else {
				SR = SR & 0b11111110;
			}
			ac = memData << 1;
		}
		else {
			if ((memData & 0b10000000) > 0) {
				SR = SR | 0b00000001;
			}
			else {
				SR = SR & 0b11111110;
			}
			memWrite(addrData, memData << 1);
			evalSpecialRegs(addrData, false);
			//memory[addrData] = memData << 1;
		}
		setFlags(memData << 1, false, true, true, false);
		break;
	case 0b001:
		// ROL (rotate left)
		ROL(instr, addrData, memData);
		break;
	case 0b010:
		// set the carry bit to the one we will lose

		// LSR (right shift)
		if (instr == 0x4A) {
			if ((ac & 0b00000001) > 0) {
				SR = SR | 0b00000001;
			}
			else {
				SR = SR & 0b11111110;
			}

			ac = ((memData >> 1) & 0b01111111);
		}
		else {
			if ((memData & 0b00000001) > 0) {
				SR = SR | 0b00000001;
			}
			else {
				SR = SR & 0b11111110;
			}

			memWrite(addrData, ((memData >> 1) & 0b01111111));
			evalSpecialRegs(addrData, false);
			//memory[addrData] = ((memData >> 1) & 0b01111111);
		}
		setFlags(((memData >> 1) & 0b01111111), false, true, true, false);
		break;
	case 0b011:
		// ROR (rotate right)
		ROR(instr, addrData, memData);
		break;
	// STX
	case 0b100:


		// STX (store x)
		// zero page, X becomes zeropage, Y
		if (addrMode == 0b101) {
			// zeropage, Y
			addrData = uint8_t(param1 + y);
		}
		evalControllerRegs(addrData, false, &memData);

		memWrite(addrData, x);
		//memory[addrData] = x;

		evalSpecialRegs(addrData, false);
		break;
	case 0b101:
		// LDX
		// zeropage, X becomes zeropage, Y
		if (addrMode == 0b101) {
			// zeropage, Y
			memData = memory[uint8_t(param1 + y)];
		}		
		// abs X becomes abs Y
		else if (addrMode == 0b111) {
			//absoulte, Y
			memData = memory[uint16_t((param2 << 8) + param1 + y)];
		}

		evalControllerRegs(addrData, true, &memData);
		x = memData & 0xFF;
		setFlags(x, false, true, true, false);
		evalSpecialRegs(addrData, true);
		break;
	case 0b110:
		// DEC (decrement by one)
		evalControllerRegs(addrData, false, &memData);
		memWrite(addrData, memory[addrData] - 1);
		//memory[addrData] -= 1;
		setFlags(memory[addrData], false, true, true, false);
		evalSpecialRegs(addrData, false);
		break;
	case 0b111:
		// INC (increment by one)
		evalControllerRegs(addrData, false, &memData);
		
		memWrite(addrData, memory[addrData] + 1);
		//memory[addrData] += 1;
		setFlags(memory[addrData], false, true, true, false);
		evalSpecialRegs(addrData, false);
		break;
	default:
		break;
	}
	pc += instrSize;
}

void _6502::groupThreeEval(uint8_t instr, uint8_t param1, uint8_t param2) {
	// three bit opcode
	uint8_t opcode = instr >> 5;
	// adressing mode (three bits)
	uint8_t addrMode = (instr >> 2) & 0b000111;
	// get the data from the addrmode (works for two&three)
	memoryData d = getGroupTwoMemData(addrMode, param1, param2);
	int16_t addrData = d.addrData;
	uint16_t memData = d.memData;
	instrSize = d.size;

	if (instr == 0x1A || instr == 0x3A || instr == 0x5A || instr == 0x7A || instr == 0xDA
		|| instr == 0xFA || instr == 0x80 || instr == 0x82 || instr == 0x89 || instr == 0xC2 || instr == 0xE2 || instr == 0x04 || instr == 0x44
		|| instr == 0x64 || instr == 0x14 || instr == 0x34 || instr == 0x54 || instr == 0x74 || instr == 0xD4 || instr == 0xF4 || instr == 0x0C
		|| instr == 0x1C || instr == 0x3C || instr == 0x5C || instr == 0x7C || instr == 0xDC || instr == 0xFC)
	{
		pc += instrSize;
		return;
	}
	switch (opcode)
	{
	case 0b001:

		//BIT
		// first check if it anded is zero
		if ( (ac & memData) == 0) {
			SR = SR | 0b00000010;
		}
		else {
			SR = SR & 0b11111101;
		}
		// bits six and seven of data are set to overflow and negative
		SR = (SR & 0b00111111) | (memData & 0b11000000);
		break;
	case 0b010:
		// JMP
		pc = addrData;
		instrSize = 0;
		break;
	case 0b011:
		// JMP (abs)
		// JMP to pointer
		// bugged so it only adds to the low byte of addr data
		// https://forums.nesdev.org/viewtopic.php?t=6621&start=15
		if (instr != 0x6C) {
			break;
		}
		pc = (memory[uint16_t( (addrData & 0xFF00) + uint8_t((addrData & 0x00FF) + 1))] << 8) + memory[addrData];

		instrSize = 0;
		break;
	case 0b100:
		// STY
		//memory[addrData] = y;

		evalControllerRegs(addrData, false, &memData);
		memWrite(addrData, y);
		evalSpecialRegs(addrData, false);
		break;
	case 0b101:
		// LDY
		evalControllerRegs(addrData, true, &memData);
		y = memData & 0xFF;
		setFlags(y, false, true, true, false);
		evalSpecialRegs(addrData, true);
		break;
	case 0b110:
		// CPY (compare y)
		if (instr == 0xD4) {
			break;
		}
		compare(y, memData);
		break;
	case 0b111:
		// CPX (compare x)
		compare(x, memData);
		break;
	default:
		break;
	}
	pc += instrSize;
}

void _6502::branchEval(int8_t instr, int8_t param1) {

	uint8_t shift = branchShiftData(instr);
	uint8_t JumpOnTrue = (instr & 0b00100000);
	instrSize = 2;
	pc += instrSize;
	// if SR & mask == 0 false if SR & mask != 0 true
	if ( ((SR >> shift) & 0b00000001) != 0 && JumpOnTrue != 0) {
		pc += int(param1);
	}
	else if(((SR >> shift) & 0b00000001) == 0 && JumpOnTrue == 0)
	{

		pc += int(param1);
	}

}

void _6502::otherEval(uint8_t instr, uint8_t param1, uint8_t param2) {
	// default value is one just because most instructions are one byte
	instrSize = 1;
	switch (instr)
	{
	case 0x00:
		// BRK
		instrSize = 0;

		pc += 2;
		memory[stackAddr + sp] = pc >> 8;
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;
		memory[stackAddr + sp] = pc & 0xFF;
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;
		
		// BRK
		SR = SR | 0x10;
		memory[stackAddr + sp ] = SR;
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;

		// interrupt
		SR = SR | 0x4;
		//SR = (SR & (~0x20));


		pc = (memory[0xFFFE] | (memory[0xFFFF] << 8));
		std::cout << "SP IS: " << (uint16_t)sp << " DATA " << std::hex << (uint16_t)memory[stackAddr + sp + 1] << " : " << (uint16_t)memory[stackAddr + sp + 2] << " : " << (uint16_t)memory[stackAddr + sp + 3] << " SR " << (uint16_t)SR;
		break;
	case 0x20:
		// JSR abs
		pc += 2;
		memory[stackAddr + sp] = (((pc) & 0xFF00) >> 8);
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;
		memory[stackAddr + sp] = ((pc) & 0xFF);
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;
		pc = (param2 << 8) + param1;

		// instruction has no size because we jumped

		instrSize = 0;
		break;
	case 0x40:
		// RTI
		sp += 1;
		SR = ((memory[stackAddr + sp] & 0b11011111) | 0b00100000);
		evalSpecialRegs(stackAddr + sp, true);
		sp += 1;
		pc = (memory[stackAddr + uint8_t(sp + 1)] << 8) + memory[stackAddr + sp];

		evalSpecialRegs(stackAddr + sp, true);
		sp += 1;
		instrSize = 0;

		break;
	case 0x60:
		// RTS
		sp += 1;
		pc = (memory[stackAddr + uint8_t(sp + 1)] << 8) + memory[stackAddr + sp];
		evalSpecialRegs(stackAddr + sp, true);
		evalSpecialRegs(stackAddr + sp + 1, true);
		sp += 1;
		instrSize = 1;
		break;
	// single byte instructions
	case 0x08:
		// PHP
		memory[stackAddr + sp] = (SR | 0b00010000);
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;
		break;
	case 0x28:
		// PLP
		// break and bit 5 ignored
		sp += 1;
		SR = (memory[stackAddr + sp] & 0b11001111) | (SR & 0b00110000);

		evalSpecialRegs(stackAddr + sp, true);
		break;
	case 0x48:
		// PHA
		memory[stackAddr + sp] = ac;
		evalSpecialRegs(stackAddr + sp, false);
		sp -= 1;
		break;
	case 0x68:
		// PLA
		sp += 1;
		ac = memory[stackAddr + sp];
		evalSpecialRegs(stackAddr + sp, true);
		setFlags(ac, false, true, true, false);
		break;
	case 0x88:
		// DEY
		y -= 1;
		setFlags(y, false, true, true, false);
		break;
	case 0xA8:
		// TAY
		y = ac;
		setFlags(y, false, true, true, false);
		break;
	case 0xC8:
		// INY
		y += 1;
		setFlags(y, false, true, true, false);
		break;
	case 0xE8:
		// INX
		x += 1;
	
		setFlags(x, false, true, true, false);
		break;
	case 0x18:
		// CLC (clear carry flag)
		SR = SR & 0b11111110;
		break;
	case 0x38:
		// SEC (set carry flag)
		SR = SR | 0b00000001;
		break;
	case 0x58:
		// CLI (clear interrupt disable)
		SR = SR & 0b11111011;
		break;
	case 0x78:
		// SEI (set interrupt disable)
		SR = SR | 0b00000100;
		break;
	case 0x98:
		// TYA (transfer Y to AC)
		ac = y;
		setFlags(ac, false, true, true, false);
		break;
	case 0xB8:
		// CLV (clear overflow)
		SR = SR & 0b10111111;
		break;
	case 0xD8:
		// CLD
		SR = SR & 0b11110111;
		break;
	case 0xF8:
		// SED
		SR = SR | 0b00001000;
		break;
	case 0x8A:
		// TXA (transfer X to AC)
		ac = x;
		setFlags(ac, false, true, true, false);
		break;
	case 0x9A:
		// TXS (trasfer x to sp)
		sp = x;
		break;
	case 0xAA:
		// TAX (ac to x)
		x = ac;
		setFlags(x, false, true, true, false);
		break;
	case 0xBA:
		// TSX
		x = sp;
		setFlags(x, false, true, true, false);
		break;
	case 0xCA:
		// DEX
		x -= 1;
		setFlags(x, false, true, true, false);
		break;
	case 0xEA:
		// NOP (no operation)
		break;
	default:
		break;
	}
	if (instr == 0x38) {
		//std::cout <<  "SIZE " << unsigned(instrSize);
	}
	pc += instrSize;
}

void _6502::illegalEval(uint8_t instr, uint8_t param1, uint8_t param2) {
	// three bit opcode
	uint8_t opcode = instr >> 5;
	// adressing mode (three bits)
	uint8_t addrMode = (instr >> 2) & 0b000111;
	// get the data from the addrmode
	memoryData d = getGroupOneMemData(addrMode, param1, param2);
	uint16_t addrData = d.addrData;
	uint16_t memData = d.memData;
	instrSize = d.size;

	switch (opcode)
	{
	// LAX
	case 0b101:
		// zpg, X becomes zpg, Y same with abs
		if (instr == 0xB7) {
			memData = memory[uint8_t(param1 + y)];
		}
		else if (instr == 0xBF) {
			memData = memory[uint16_t((param2 << 8) + param1 + y)];
		}
		LAX(memData);
		break;
	// SAX
	case 0b100:
		// zpg, X becomes zpg, Y
		if (instr == 0x97) {
			addrData = uint8_t(param1 + y);
		}
		SAX(addrData);
		break;
	// SBC or ISC (increment then sbc)
	case 0b111:
		// SBC
		if (instr != 0xEB) {
			memWrite(addrData, memory[addrData] + 1);
			//memory[addrData] += 1;
			addWCarry(~memory[addrData]);
		}
		else {
			addWCarry(~memData);
		}

		break;
	// DCP decrement memory then CMP with ac
	case 0b110:
		memWrite(addrData, memory[addrData] - 1);
		//memory[addrData] -= 1;
		compare(ac, memory[addrData]);
		break;
	// SLO (shift left OR)
	case 0b000:
		// ASL
		if ((memory[addrData] & 0b10000000) > 0) {
			SR = SR | 0b00000001;
		}
		else {
			SR = SR & 0b11111110;
		}

		memWrite(addrData, memData << 1);
		evalSpecialRegs(addrData, false);
		//memory[addrData] = memData << 1;
		setFlags(memData << 1, false, true, true, false);
		// ORA
		ac = ac | memory[addrData];
		setFlags(ac, false, true, true, false);
		break;
	// RLA (ROL + AND)
	case 0b001:
		ROL(instr, addrData, memData);
		ac = ac & memory[addrData];
		setFlags(ac, false, true, true, false);
		break;
	// SRE (LSR + EOR)
	case 0b010:
		// LSR
		if ((memory[addrData] & 0b00000001) > 0) {
			SR = SR | 0b00000001;
		}
		else {
			SR = SR & 0b11111110;
		}
		// LSR (right shift)
		memWrite(addrData, ((memData >> 1) & 0b01111111));
		//memory[addrData] = ((memData >> 1) & 0b01111111);
		setFlags(((memData >> 1) & 0b01111111), false, true, true, false);

		// EOR
		ac = ac ^ memory[addrData];
		setFlags(ac, false, true, true, false);
		break;
	case 0b011:
		ROR(instr, addrData, memData);
		addWCarry(memory[addrData]);
		break;
	default:
		break;
	}
	pc += instrSize;
}

// updates SR
void _6502::setFlags(uint16_t num, bool carry, bool negitive, bool zero, bool overflow) {
	if (carry) {
		if (num > 255) {
			// set the last bit of the sataus register
			SR = SR | 0b00000001;
		}
		else
		{
			SR = SR & 0b11111110;
		}
	}
	if (negitive) {
		if ((num & 0b10000000) != 0) {
			SR = SR | 0b10000000;
		}
		else {
			SR = SR & 0b01111111;
		}
	}
	if (zero) {
		if ((num & 0xFF) == 0) {
			SR = SR | 0b00000010;
		}
		else {
			SR = SR & 0b11111101;
		}
	}
	if (overflow) {
		if ((num > 127 || num < -128)) {
			SR = SR | 0b01000000;
		}
		else {
			SR = SR & 0b10111111;
		}
	}

}

// sets a flag if ppuEnable != 0 update ppu registers on th

void _6502::evalSpecialRegs(uint16_t addr, bool read) {

	if ((addr >= 0x2000 && addr <= 0x3FFF) || addr == 0x4014) {
		if (addr < 0x4014) {
			ppuEnable = 0x2000 + (addr & 0b111);
		}
		else {
			ppuEnable = addr;
		}

		ppuRead = read;
	}
	else if (addr == 0x4016 && !read) {

		
		controller->write(memory[addr]);
	}

}


void _6502::evalControllerRegs(uint16_t addr, bool read, uint16_t* memD) {
	if (addr == 0x4016) {
		if (read) {
			
			uint8_t d = controller->read();
			memWrite(addr, d);
			//memory[addr] = d;
			*memD = d;
		}

	}
}

void _6502::compare(uint8_t a, uint8_t b) {
	setFlags(uint16_t(a - b), true, true, true, false);
	if (uint8_t((a)) >= uint8_t(b)) {
		SR = SR | 0b00000001;
	}
	else {
		SR = SR & 0b11111110;
	}
}
void _6502::addWCarry(uint8_t data) {
	if ((~(ac ^ data) & (ac ^ ((ac + data + (SR & 1)) & 0xFF)) & 0x80) != 0) {
		SR = SR | 0b01000000;
	}
	else {
		SR = SR & 0b10111111;
	}

	uint8_t sum = ac + data + (SR & 0b1);
	if (int((uint16_t(ac) & 0xFF) + (data + (SR & 1))) > 0xFF)  {
		SR = SR | 0b00000001;
	}
	else {
		SR = SR & 0b11111110;
	}

	ac = sum;

	setFlags(ac, false, true, true, false);
}

void _6502::ROR(uint8_t instr, uint16_t addr, uint8_t data) {
	// set the carry bit to the one we will lose
	uint8_t oldCarry = SR & 0b00000001;

	// LSR (right shift)
	if (instr == 0x6A) {
		if ((ac & 0b00000001) > 0) {
			SR = SR | 0b00000001;
		}
		else {
			SR = SR & 0b11111110;
		}
		ac = (((data >> 1) & 0b01111111)) | (oldCarry << 7);
	}
	else {
		if ((memory[addr] & 0b00000001) > 0) {
			SR = SR | 0b00000001;
		}
		else {
			SR = SR & 0b11111110;
		}
		memWrite(addr, (((data >> 1) & 0b01111111)) | (oldCarry << 7));
		evalSpecialRegs(addr, false);
		//memory[addr] = (((data >> 1) & 0b01111111)) | (oldCarry << 7);
	}

	setFlags((((data >> 1) & 0b01111111)) | (oldCarry << 7), false, true, true, false);
}

void _6502::ROL(uint8_t instr, uint16_t addr, uint8_t data) {
	// set the carry bit to the one we will lose
	uint8_t oldCarry = SR & 0b00000001;

	// LSR (right shift)
	if (instr == 0x2A) {
		if ((ac & 0x80) > 0) {
			SR = SR | 0b00000001;
		}
		else {
			SR = SR & 0b11111110;
		}
		ac = (((data << 1) & 0b11111110)) | (oldCarry);
	}
	else {
		if ((memory[addr] & 0x80) > 0) {
			SR = SR | 0b00000001;
		}
		else {
			SR = SR & 0b11111110;
		}

		memWrite(addr, (((data << 1) & 0b11111110)) | (oldCarry));
		//memory[addr] = (((data << 1) & 0b11111110)) | (oldCarry);
	}
	setFlags((((data << 1) & 0b11111110)) | (oldCarry), false, true, true, false);
}


void _6502::LAX(uint8_t data) {
	ac = data;
	x = data;
	setFlags(data, false, true, true, false);
}

void _6502::SAX(uint16_t data) {
	memWrite(data, (ac & x));
	//memory[data] = (ac & x);
}

void _6502::NMI() {
	memWrite(stackAddr + sp, (pc >> 8));
	//memory[stackAddr + sp] = (pc >> 8);
	sp -= 1;
	memWrite(stackAddr + sp, (pc & 0xFF));
	//memory[stackAddr + sp] = (pc & 0xFF);
	sp -= 1;
	memWrite(stackAddr + sp, SR);
	//memory[stackAddr + sp] = SR;
	sp -= 1;
	pc = NMI_addr;
}

void _6502::loadCycleMap() {
	cycleMap[0x69] = 2;
	cycleMap[0x65] = 3;
	cycleMap[0x75] = 4;
	cycleMap[0x6D] = 4;
	cycleMap[0x7D] = 4;
	cycleMap[0x79] = 4;
	cycleMap[0x61] = 6;
	cycleMap[0x71] = 5;
	cycleMap[0x29] = 2;
	cycleMap[0x25] = 3;
	cycleMap[0x35] = 4;
	cycleMap[0x2D] = 4;
	cycleMap[0x3D] = 4;
	cycleMap[0x39] = 4;
	cycleMap[0x21] = 6;
	cycleMap[0x31] = 5;
	cycleMap[0x0A] = 2;
	cycleMap[0x06] = 5;
	cycleMap[0x16] = 6;
	cycleMap[0x0E] = 6;
	cycleMap[0x1E] = 7;
	cycleMap[0x90] = 2;
	cycleMap[0xB0] = 2;
	cycleMap[0xF0] = 2;
	cycleMap[0x24] = 3;
	cycleMap[0x2C] = 4;
	cycleMap[0x30] = 2;
	cycleMap[0xD0] = 2;
	cycleMap[0x10] = 2;
	cycleMap[0x00] = 7;
	cycleMap[0x50] = 2;
	cycleMap[0x70] = 2;
	cycleMap[0x18] = 2;
	cycleMap[0xD8] = 2;
	cycleMap[0x58] = 2;
	cycleMap[0xB8] = 2;
	cycleMap[0xC9] = 2;
	cycleMap[0xC5] = 3;
	cycleMap[0xD5] = 4;
	cycleMap[0xCD] = 4;
	cycleMap[0xDD] = 4;
	cycleMap[0xD9] = 4;
	cycleMap[0xC1] = 6;
	cycleMap[0xD1] = 5;
	cycleMap[0xE0] = 2;
	cycleMap[0xE4] = 3;
	cycleMap[0xEC] = 4;
	cycleMap[0xC0] = 2;
	cycleMap[0xC4] = 3;
	cycleMap[0xCC] = 4;
	cycleMap[0xC6] = 5;
	cycleMap[0xD6] = 6;
	cycleMap[0xCE] = 6;
	cycleMap[0xDE] = 7;
	cycleMap[0xCA] = 2;
	cycleMap[0x88] = 2;
	cycleMap[0x49] = 2;
	cycleMap[0x45] = 3;
	cycleMap[0x55] = 4;
	cycleMap[0x4D] = 4;
	cycleMap[0x5D] = 4;
	cycleMap[0x59] = 4;
	cycleMap[0x41] = 6;
	cycleMap[0x51] = 5;
	cycleMap[0xE6] = 5;
	cycleMap[0xF6] = 6;
	cycleMap[0xEE] = 6;
	cycleMap[0xFE] = 7;
	cycleMap[0xE8] = 2;
	cycleMap[0xC8] = 2;
	cycleMap[0x4C] = 3;
	cycleMap[0x6C] = 5;
	cycleMap[0x20] = 6;
	cycleMap[0xA9] = 2;
	cycleMap[0xA5] = 3;
	cycleMap[0xB5] = 4;
	cycleMap[0xAD] = 4;
	cycleMap[0xBD] = 4;
	cycleMap[0xB9] = 4;
	cycleMap[0xA1] = 6;
	cycleMap[0xB1] = 5;
	cycleMap[0xA2] = 2;
	cycleMap[0xA6] = 3;
	cycleMap[0xB6] = 4;
	cycleMap[0xAE] = 4;
	cycleMap[0xBE] = 4;
	cycleMap[0xA0] = 2;
	cycleMap[0xA4] = 3;
	cycleMap[0xB4] = 4;
	cycleMap[0xAC] = 4;
	cycleMap[0xBC] = 4;
	cycleMap[0x4A] = 2;
	cycleMap[0x46] = 5;
	cycleMap[0x56] = 6;
	cycleMap[0x4E] = 6;
	cycleMap[0x5E] = 7;
	cycleMap[0xEA] = 2;
	cycleMap[0x09] = 2;
	cycleMap[0x05] = 3;
	cycleMap[0x15] = 4;
	cycleMap[0x0D] = 4;
	cycleMap[0x1D] = 4;
	cycleMap[0x19] = 4;
	cycleMap[0x01] = 6;
	cycleMap[0x11] = 5;
	cycleMap[0x48] = 3;
	cycleMap[0x08] = 3;
	cycleMap[0x68] = 4;
	cycleMap[0x28] = 4;
	cycleMap[0x2A] = 2;
	cycleMap[0x26] = 5;
	cycleMap[0x36] = 6;
	cycleMap[0x2E] = 6;
	cycleMap[0x3E] = 7;
	cycleMap[0x6A] = 2;
	cycleMap[0x66] = 5;
	cycleMap[0x76] = 6;
	cycleMap[0x6E] = 6;
	cycleMap[0x7E] = 7;
	cycleMap[0x40] = 6;
	cycleMap[0x60] = 6;
	cycleMap[0xE9] = 2;
	cycleMap[0xE5] = 3;
	cycleMap[0xF5] = 4;
	cycleMap[0xED] = 4;
	cycleMap[0xFD] = 4;
	cycleMap[0xF9] = 4;
	cycleMap[0xE1] = 6;
	cycleMap[0xF1] = 5;
	cycleMap[0x38] = 2;
	cycleMap[0xF8] = 2;
	cycleMap[0x78] = 2;
	cycleMap[0x85] = 3;
	cycleMap[0x95] = 4;
	cycleMap[0x8D] = 4;
	cycleMap[0x9D] = 5;
	cycleMap[0x99] = 5;
	cycleMap[0x81] = 6;
	cycleMap[0x91] = 6;
	cycleMap[0x86] = 3;
	cycleMap[0x96] = 4;
	cycleMap[0x8E] = 4;
	cycleMap[0x84] = 3;
	cycleMap[0x94] = 4;
	cycleMap[0x8C] = 4;
	cycleMap[0xAA] = 2;
	cycleMap[0xA8] = 2;
	cycleMap[0xBA] = 2;
	cycleMap[0x8A] = 2;
	cycleMap[0x9A] = 2;
	cycleMap[0x98] = 2;
	cycleMap[0x4B] = 2;
	cycleMap[0x0B] = 2;
	cycleMap[0x2B] = 2;
	cycleMap[0x8B] = 2;
	cycleMap[0x6B] = 2;
	cycleMap[0xC7] = 5;
	cycleMap[0xD7] = 6;
	cycleMap[0xCF] = 6;
	cycleMap[0xDF] = 7;
	cycleMap[0xDB] = 7;
	cycleMap[0xC3] = 8;
	cycleMap[0xD3] = 8;
	cycleMap[0xE7] = 5;
	cycleMap[0xF7] = 6;
	cycleMap[0xEF] = 6;
	cycleMap[0xFF] = 7;
	cycleMap[0xFB] = 7;
	cycleMap[0xE3] = 8;
	cycleMap[0xF3] = 8;
	cycleMap[0xBB] = 4;
	cycleMap[0xA7] = 3;
	cycleMap[0xB7] = 4;
	cycleMap[0xAF] = 4;
	cycleMap[0xBF] = 4;
	cycleMap[0xA3] = 6;
	cycleMap[0xB3] = 5;
	cycleMap[0xAB] = 2;
	cycleMap[0x27] = 5;
	cycleMap[0x37] = 6;
	cycleMap[0x2F] = 6;
	cycleMap[0x3F] = 7;
	cycleMap[0x3B] = 7;
	cycleMap[0x23] = 8;
	cycleMap[0x33] = 8;
	cycleMap[0x67] = 5;
	cycleMap[0x77] = 6;
	cycleMap[0x6F] = 6;
	cycleMap[0x7F] = 7;
	cycleMap[0x7B] = 7;
	cycleMap[0x63] = 8;
	cycleMap[0x73] = 8;
	cycleMap[0x87] = 3;
	cycleMap[0x97] = 4;
	cycleMap[0x8F] = 4;
	cycleMap[0x83] = 6;
	cycleMap[0xCB] = 2;
	cycleMap[0x9F] = 5;
	cycleMap[0x93] = 6;
	cycleMap[0x9E] = 5;
	cycleMap[0x9C] = 5;
	cycleMap[0x07] = 5;
	cycleMap[0x17] = 6;
	cycleMap[0x0F] = 6;
	cycleMap[0x1F] = 7;
	cycleMap[0x1B] = 7;
	cycleMap[0x03] = 8;
	cycleMap[0x13] = 8;
	cycleMap[0x47] = 5;
	cycleMap[0x57] = 6;
	cycleMap[0x4F] = 6;
	cycleMap[0x5F] = 7;
	cycleMap[0x5B] = 7;
	cycleMap[0x43] = 8;
	cycleMap[0x53] = 8;
	cycleMap[0x9B] = 5;
	cycleMap[0xEB] = 2;
	cycleMap[0x6A] = 2;
	cycleMap[0x66] = 5;
	cycleMap[0x76] = 6;
	cycleMap[0x6E] = 6;
	cycleMap[0x7E] = 7;

}