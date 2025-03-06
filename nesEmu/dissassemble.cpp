#include "dissassemble.h"



Dissassembler::Dissassembler() {
	InstrMap[0x00] = Instr{ "BRK", Implied };
	InstrMap[0x01] = Instr{ "ORA", XIndr };
	InstrMap[0x03] = Instr{ "SLO", XIndr };
	InstrMap[0x04] = Instr{ "NOPs", Zpg };
	InstrMap[0x05] = Instr{ "ORA", Zpg };
	InstrMap[0x06] = Instr{ "ASL", Zpg };
	InstrMap[0x07] = Instr{ "SLO", Zpg };
	InstrMap[0x08] = Instr{ "PHP", Implied };
	InstrMap[0x09] = Instr{ "ORA", Immediate };
	InstrMap[0x0A] = Instr{ "ASL", Acume };
	InstrMap[0x0B] = Instr{ "ANC", Immediate };
	InstrMap[0x0C] = Instr{ "NOPs", Abs };
	InstrMap[0x0D] = Instr{ "ORA", Abs };
	InstrMap[0x0E] = Instr{ "ASL", Abs };
	InstrMap[0x0F] = Instr{ "SLO", Abs };
	InstrMap[0x10] = Instr{ "BPL", Rel };
	InstrMap[0x11] = Instr{ "ORA", IndrY };
	InstrMap[0x13] = Instr{ "SLO", IndrY };
	InstrMap[0x14] = Instr{ "NOPs", ZpgX };
	InstrMap[0x15] = Instr{ "ORA", ZpgX };
	InstrMap[0x16] = Instr{ "ASL", ZpgX };
	InstrMap[0x17] = Instr{ "SLO", ZpgX };
	InstrMap[0x18] = Instr{ "CLC", Implied };
	InstrMap[0x19] = Instr{ "ORA", AbsY };
	InstrMap[0x1A] = Instr{ "NOPs", Implied };
	InstrMap[0x1B] = Instr{ "SLO", AbsY };
	InstrMap[0x1C] = Instr{ "NOPs", AbsX };
	InstrMap[0x1D] = Instr{ "ORA", AbsX };
	InstrMap[0x1E] = Instr{ "ASL", AbsX };
	InstrMap[0x1F] = Instr{ "SLO", AbsX };
	InstrMap[0x20] = Instr{ "JSR", Abs };
	InstrMap[0x21] = Instr{ "AND", XIndr };
	InstrMap[0x23] = Instr{ "RLA", XIndr };
	InstrMap[0x24] = Instr{ "BIT", Zpg };
	InstrMap[0x25] = Instr{ "AND", Zpg };
	InstrMap[0x26] = Instr{ "ROL", Zpg };
	InstrMap[0x27] = Instr{ "RLA", Zpg };
	InstrMap[0x28] = Instr{ "PLP", Implied };
	InstrMap[0x29] = Instr{ "AND", Immediate };
	InstrMap[0x2A] = Instr{ "ROL", Acume };
	InstrMap[0x2B] = Instr{ "ANC", Immediate };
	InstrMap[0x2C] = Instr{ "BIT", Abs };
	InstrMap[0x2D] = Instr{ "AND", Abs };
	InstrMap[0x2E] = Instr{ "ROL", Abs };
	InstrMap[0x2F] = Instr{ "RLA", Abs };
	InstrMap[0x30] = Instr{ "BMI", Rel };
	InstrMap[0x31] = Instr{ "AND", IndrY };
	InstrMap[0x33] = Instr{ "RLA", IndrY };
	InstrMap[0x34] = Instr{ "NOPs", ZpgX };
	InstrMap[0x35] = Instr{ "AND", ZpgX };
	InstrMap[0x36] = Instr{ "ROL", ZpgX };
	InstrMap[0x37] = Instr{ "RLA", ZpgX };
	InstrMap[0x38] = Instr{ "SEC", Implied };
	InstrMap[0x39] = Instr{ "AND", AbsY };
	InstrMap[0x3A] = Instr{ "NOPs", Implied };
	InstrMap[0x3B] = Instr{ "RLA", AbsY };
	InstrMap[0x3C] = Instr{ "NOPs", AbsX };
	InstrMap[0x3D] = Instr{ "AND", AbsX };
	InstrMap[0x3E] = Instr{ "ROL", AbsX };
	InstrMap[0x3F] = Instr{ "RLA", AbsX };
	InstrMap[0x40] = Instr{ "RTI", Implied };
	InstrMap[0x41] = Instr{ "EOR", XIndr };
	InstrMap[0x43] = Instr{ "SRE", XIndr };
	InstrMap[0x44] = Instr{ "NOPs", Zpg };
	InstrMap[0x45] = Instr{ "EOR", Zpg };
	InstrMap[0x46] = Instr{ "LSR", Zpg };
	InstrMap[0x47] = Instr{ "SRE", Zpg };
	InstrMap[0x48] = Instr{ "PHA", Implied };
	InstrMap[0x49] = Instr{ "EOR", Immediate };
	InstrMap[0x4A] = Instr{ "LSR", Acume };
	InstrMap[0x4B] = Instr{ "ALR", Immediate };
	InstrMap[0x4C] = Instr{ "JMP", Abs };
	InstrMap[0x4D] = Instr{ "EOR", Abs };
	InstrMap[0x4E] = Instr{ "LSR", Abs };
	InstrMap[0x4F] = Instr{ "SRE", Abs };
	InstrMap[0x50] = Instr{ "BVC", Rel };
	InstrMap[0x51] = Instr{ "EOR", IndrY };
	InstrMap[0x53] = Instr{ "SRE", IndrY };
	InstrMap[0x54] = Instr{ "NOPs", ZpgX };
	InstrMap[0x55] = Instr{ "EOR", ZpgX };
	InstrMap[0x56] = Instr{ "LSR", ZpgX };
	InstrMap[0x57] = Instr{ "SRE", ZpgX };
	InstrMap[0x58] = Instr{ "CLI", Implied };
	InstrMap[0x59] = Instr{ "EOR", AbsY };
	InstrMap[0x5A] = Instr{ "NOPs", Implied };
	InstrMap[0x5B] = Instr{ "SRE", AbsY };
	InstrMap[0x5C] = Instr{ "NOPs", AbsX };
	InstrMap[0x5D] = Instr{ "EOR", AbsX };
	InstrMap[0x5E] = Instr{ "LSR", AbsX };
	InstrMap[0x5F] = Instr{ "SRE", AbsX };
	InstrMap[0x60] = Instr{ "RTS", Implied };
	InstrMap[0x61] = Instr{ "ADC", XIndr };
	InstrMap[0x63] = Instr{ "RRA", XIndr };
	InstrMap[0x64] = Instr{ "NOPs", Zpg };
	InstrMap[0x65] = Instr{ "ADC", Zpg };
	InstrMap[0x66] = Instr{ "ROR", Zpg };
	InstrMap[0x67] = Instr{ "RRA", Zpg };
	InstrMap[0x68] = Instr{ "PLA", Implied };
	InstrMap[0x69] = Instr{ "ADC", Immediate };
	InstrMap[0x6A] = Instr{ "ROR", Acume };
	InstrMap[0x6B] = Instr{ "ARR", Immediate };
	InstrMap[0x6C] = Instr{ "JMP", Indr };
	InstrMap[0x6D] = Instr{ "ADC", Abs };
	InstrMap[0x6E] = Instr{ "ROR", Abs };
	InstrMap[0x6F] = Instr{ "RRA", Abs };
	InstrMap[0x70] = Instr{ "BVS", Rel };
	InstrMap[0x71] = Instr{ "ADC", IndrY };
	InstrMap[0x73] = Instr{ "RRA", IndrY };
	InstrMap[0x74] = Instr{ "NOPs", ZpgX };
	InstrMap[0x75] = Instr{ "ADC", ZpgX };
	InstrMap[0x76] = Instr{ "ROR", ZpgX };
	InstrMap[0x77] = Instr{ "RRA", ZpgX };
	InstrMap[0x78] = Instr{ "SEI", Implied };
	InstrMap[0x79] = Instr{ "ADC", AbsY };
	InstrMap[0x7A] = Instr{ "NOPs", Implied };
	InstrMap[0x7B] = Instr{ "RRA", AbsY };
	InstrMap[0x7C] = Instr{ "NOPs", AbsX };
	InstrMap[0x7D] = Instr{ "ADC", AbsX };
	InstrMap[0x7E] = Instr{ "ROR", AbsX };
	InstrMap[0x7F] = Instr{ "RRA", AbsX };
	InstrMap[0x80] = Instr{ "NOPs", Immediate };
	InstrMap[0x81] = Instr{ "STA", XIndr };
	InstrMap[0x82] = Instr{ "NOPs", Immediate };
	InstrMap[0x83] = Instr{ "SAX", XIndr };
	InstrMap[0x84] = Instr{ "STY", Zpg };
	InstrMap[0x85] = Instr{ "STA", Zpg };
	InstrMap[0x86] = Instr{ "STX", Zpg };
	InstrMap[0x87] = Instr{ "SAX", Zpg };
	InstrMap[0x88] = Instr{ "DEY", Implied };
	InstrMap[0x89] = Instr{ "NOPs", Immediate };
	InstrMap[0x8A] = Instr{ "TXA", Implied };
	InstrMap[0x8B] = Instr{ "ANE", Immediate };
	InstrMap[0x8C] = Instr{ "STY", Abs };
	InstrMap[0x8D] = Instr{ "STA", Abs };
	InstrMap[0x8E] = Instr{ "STX", Abs };
	InstrMap[0x8F] = Instr{ "SAX", Abs };
	InstrMap[0x90] = Instr{ "BCC", Rel };
	InstrMap[0x91] = Instr{ "STA", IndrY };
	InstrMap[0x93] = Instr{ "SHA", IndrY };
	InstrMap[0x94] = Instr{ "STY", ZpgX };
	InstrMap[0x95] = Instr{ "STA", ZpgX };
	InstrMap[0x96] = Instr{ "STX", ZpgY };
	InstrMap[0x97] = Instr{ "SAX", ZpgY };
	InstrMap[0x98] = Instr{ "TYA", Implied };
	InstrMap[0x99] = Instr{ "STA", AbsY };
	InstrMap[0x9A] = Instr{ "TXS", Implied };
	InstrMap[0x9C] = Instr{ "SHY", AbsX };
	InstrMap[0x9D] = Instr{ "STA", AbsX };
	InstrMap[0x9E] = Instr{ "SHX", AbsY };
	InstrMap[0x9F] = Instr{ "SHA", AbsY };
	InstrMap[0xA0] = Instr{ "LDY", Immediate };
	InstrMap[0xA1] = Instr{ "LDA", XIndr };
	InstrMap[0xA2] = Instr{ "LDX", Immediate };
	InstrMap[0xA3] = Instr{ "LAX", XIndr };
	InstrMap[0xA4] = Instr{ "LDY", Zpg };
	InstrMap[0xA5] = Instr{ "LDA", Zpg };
	InstrMap[0xA6] = Instr{ "LDX", Zpg };
	InstrMap[0xA7] = Instr{ "LAX", Zpg };
	InstrMap[0xA8] = Instr{ "TAY", Implied };
	InstrMap[0xA9] = Instr{ "LDA", Immediate };
	InstrMap[0xAA] = Instr{ "TAX", Implied };
	InstrMap[0xAB] = Instr{ "LXA", Immediate };
	InstrMap[0xAC] = Instr{ "LDY", Abs };
	InstrMap[0xAD] = Instr{ "LDA", Abs };
	InstrMap[0xAE] = Instr{ "LDX", Abs };
	InstrMap[0xAF] = Instr{ "LAX", Abs };
	InstrMap[0xB0] = Instr{ "BCS", Rel };
	InstrMap[0xB1] = Instr{ "LDA", IndrY };
	InstrMap[0xB3] = Instr{ "LAX", IndrY };
	InstrMap[0xB4] = Instr{ "LDY", ZpgX };
	InstrMap[0xB5] = Instr{ "LDA", ZpgX };
	InstrMap[0xB6] = Instr{ "LDX", ZpgY };
	InstrMap[0xB7] = Instr{ "LAX", ZpgY };
	InstrMap[0xB8] = Instr{ "CLV", Implied };
	InstrMap[0xB9] = Instr{ "LDA", AbsY };
	InstrMap[0xBA] = Instr{ "TSX", Implied };
	InstrMap[0xBB] = Instr{ "LAS", AbsY };
	InstrMap[0xBC] = Instr{ "LDY", AbsX };
	InstrMap[0xBD] = Instr{ "LDA", AbsX };
	InstrMap[0xBE] = Instr{ "LDX", AbsY };
	InstrMap[0xBF] = Instr{ "LAX", AbsY };
	InstrMap[0xC0] = Instr{ "CPY", Immediate };
	InstrMap[0xC1] = Instr{ "CMP", XIndr };
	InstrMap[0xC2] = Instr{ "NOPs", Immediate };
	InstrMap[0xC3] = Instr{ "DCP", XIndr };
	InstrMap[0xC4] = Instr{ "CPY", Zpg };
	InstrMap[0xC5] = Instr{ "CMP", Zpg };
	InstrMap[0xC6] = Instr{ "DEC", Zpg };
	InstrMap[0xC7] = Instr{ "DCP", Zpg };
	InstrMap[0xC8] = Instr{ "INY", Implied };
	InstrMap[0xC9] = Instr{ "CMP", Immediate };
	InstrMap[0xCA] = Instr{ "DEX", Implied };
	InstrMap[0xCB] = Instr{ "SBX", Immediate };
	InstrMap[0xCC] = Instr{ "CPY", Abs };
	InstrMap[0xCD] = Instr{ "CMP", Abs };
	InstrMap[0xCE] = Instr{ "DEC", Abs };
	InstrMap[0xCF] = Instr{ "DCP", Abs };
	InstrMap[0xD0] = Instr{ "BNE", Rel };
	InstrMap[0xD1] = Instr{ "CMP", IndrY };
	InstrMap[0xD3] = Instr{ "DCP", IndrY };
	InstrMap[0xD4] = Instr{ "NOPs", ZpgX };
	InstrMap[0xD5] = Instr{ "CMP", ZpgX };
	InstrMap[0xD6] = Instr{ "DEC", ZpgX };
	InstrMap[0xD7] = Instr{ "DCP", ZpgX };
	InstrMap[0xD8] = Instr{ "CLD", Implied };
	InstrMap[0xD9] = Instr{ "CMP", AbsY };
	InstrMap[0xDA] = Instr{ "NOPs", Implied };
	InstrMap[0xDB] = Instr{ "DCP", AbsY };
	InstrMap[0xDC] = Instr{ "NOPs", AbsX };
	InstrMap[0xDD] = Instr{ "CMP", AbsX };
	InstrMap[0xDE] = Instr{ "DEC", AbsX };
	InstrMap[0xDF] = Instr{ "DCP", AbsX };
	InstrMap[0xE0] = Instr{ "CPX", Immediate };
	InstrMap[0xE1] = Instr{ "SBC", XIndr };
	InstrMap[0xE2] = Instr{ "NOPs", Immediate };
	InstrMap[0xE3] = Instr{ "ISC", XIndr };
	InstrMap[0xE4] = Instr{ "CPX", Zpg };
	InstrMap[0xE5] = Instr{ "SBC", Zpg };
	InstrMap[0xE6] = Instr{ "INC", Zpg };
	InstrMap[0xE7] = Instr{ "ISC", Zpg };
	InstrMap[0xE8] = Instr{ "INX", Implied };
	InstrMap[0xE9] = Instr{ "SBC", Immediate };
	InstrMap[0xEA] = Instr{ "NOP", Implied };
	InstrMap[0xEB] = Instr{ "USBC", Immediate };
	InstrMap[0xEC] = Instr{ "CPX", Abs };
	InstrMap[0xED] = Instr{ "SBC", Abs };
	InstrMap[0xEE] = Instr{ "INC", Abs };
	InstrMap[0xEF] = Instr{ "ISC", Abs };
	InstrMap[0xF0] = Instr{ "BEQ", Rel };
	InstrMap[0xF1] = Instr{ "SBC", IndrY };
	InstrMap[0xF3] = Instr{ "ISC", IndrY };
	InstrMap[0xF4] = Instr{ "NOPs", ZpgX };
	InstrMap[0xF5] = Instr{ "SBC", ZpgX };
	InstrMap[0xF6] = Instr{ "INC", ZpgX };
	InstrMap[0xF7] = Instr{ "ISC", ZpgX };
	InstrMap[0xF8] = Instr{ "SED", Implied };
	InstrMap[0xF9] = Instr{ "SBC", AbsY };
	InstrMap[0xFA] = Instr{ "NOPs", Implied };
	InstrMap[0xFB] = Instr{ "ISC", AbsY };
	InstrMap[0xFC] = Instr{ "NOPs", AbsX };
	InstrMap[0xFD] = Instr{ "SBC", AbsX };
	InstrMap[0xFE] = Instr{ "INC", AbsX };
	InstrMap[0xFF] = Instr{ "ISC", AbsX };
 
	regMap[0x2000] = "PPU CTRL";
	regMap[0x2001] = "PPU MASK";
	regMap[0x2002] = "PPU STATUS";
	regMap[0x2003] = "OAM ADDR";
	regMap[0x2004] = "OAM DATA";
	regMap[0x2005] = "PPU SCROOL";
	regMap[0x2006] = "PPU ADDR";
	regMap[0x2007] = "PPU Data";
	regMap[0x4014] = "OAM DMA";
}


std::string Dissassembler::dissassemble(uint8_t instr, uint8_t p1, uint8_t p2) {
	std::string arg = "";
	addrData data = InstrMap[instr].addr;

	std::stringstream sstream;
	sstream << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << unsigned(p1);
	std::string p1Hex = sstream.str();

	// clear the stram
	sstream.str("");
	sstream << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << unsigned(p2);
	std::string p2Hex = sstream.str();

	switch (data.size)
	{
	case 1:
		arg = data.prefix + p1Hex + data.post;
		pc += 1;
		break;
	case 2:
		arg = data.prefix + p2Hex + p1Hex + data.post;
		pc += 2;
		break;
	default:
		break;
	}
	pc += 1;

	if (regMap.count((p2 << 8) + p1) > 0) {
		arg = regMap[(p2 << 8) + p1];
	}
	return  InstrMap[instr].name + " " + arg;
}

std::vector<std::string> Dissassembler::dissassembleAll(_6502* cpu) {
	std::vector<std::string> result;
	pc = cpu->pc;
	while (pc < 0xFFFF && pc < cpu->pc + 0xFF) {
		if (cpu->memory[pc] == 0) {
			break;
		}
		if (pc == 244) {
			result.push_back(dissassemble(cpu->memory[pc], 0, 0));
		}
		else if (pc == 243) {
			result.push_back(dissassemble(cpu->memory[pc], cpu->memory[pc + 1], 0));
		}
		else {
			result.push_back(dissassemble(cpu->memory[pc], cpu->memory[pc + 1], cpu->memory[pc + 2]));
		}

	}
	return result;
}