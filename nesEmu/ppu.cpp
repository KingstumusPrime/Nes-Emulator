#include "ppu.h"

// Given address intercept value with a ppu register
// This function does not assume the value is read it just changes the value in case
void ppu::getVal(uint16_t addr, uint16_t* value) {
	//std::cout << "HI";
	switch (addr)
	{
	case 0x2000:
		*value = ctrl;
		break;
	case 0x2007:
		*value = ioDataBus;
		if (vramAddr >= 0x2000 && vramAddr < 0x3F00) {
			bool NameTableZero = (vramAddr & ((int)vMirror * 0x0400)) == 0;
			ioDataBus = vram[vramAddr];
				
		}
		else if (addr >= 0x3F00 && addr <= 0x3FFF) {
			*value = vram[addr] & 0b111111;
			ioDataBus =0;
		}
		else {
			ioDataBus = patternTable[vramAddr];
		}
		break; 
	default:

		break;
	}
}
void ppu::evaluatePPURegisters(uint16_t addr, bool read) {

	if (read) {
		if (addr == 0x2002) {

			secondWrite = false;
			*status = (*status & 0b01111111);
		}
		else if (addr == 0x2007) {
			// write to vram then increment based on ppucontroll
			if ((ctrl & 0b00000100) > 0) {
				vramAddr += 32;
				v += 32;
			}
			else {
				vramAddr += 1;
				v += 1;
			}
			vramAddr = vramAddr % 0x4000;
			v = v % 0x4000;
		}

		else {
			//memory[addr] = ioDataBus;
		}
	}
	else {
		ioDataBus = memory[data];
		switch (addr)
		{
		case 0x2000:
			ctrl = memory[addr];
			t = (t & 0b111001111111111) | ((ctrl & 0b11) << 10);
			//vramAddr = (vramAddr & 0b111001111111111) | ((ctrl & 0b11) << 10);
			break;
		case 0x2001:
			mask = memory[addr];
			break;
		case 0x2002:
			*status = memory[addr];
			break;
		case 0x2003:
			oam_addr = memory[addr];
			break;
		case 0x2004:
			// store at oam
			oam[oam_addr] = oam_data;
			oam_addr++;
			break;
		case 0x2005:
			if (secondWrite) {
				//scrollY = memory[addr];
				// t: FGH..AB CDE..... <- d: ABCDEFGH
				// clear bits
				t = (t & 0b000110000011111);
				// ABCDE
				t = (t | ((memory[addr] & 0xF8) << 2));

				// FGH
				t = (t | ((memory[addr] & 0x7) << 12));
				
			}
			else {
				scrollX = memory[addr] & 0b111;
				t = (t & (~0b11111)) | ((memory[addr] & 0b11111000) >> 3);
			//	std::cout << "SETTING SCROLL TO: " << ((memory[addr] & 0b11111000) >> 3) << "SCROLL IS " << (t&0b11111) << std::endl;

			}
			secondWrite = !secondWrite;
			break;
		case 0x2006:
			// first write high bit
			// second low bit
			if (secondWrite) {
				vramAddr = (vramHigh << 8) | memory[addr];
				vramAddr = vramAddr % 0x4000;
				t = (t & 0xFF00) | memory[addr];

				v = t%0x4000;
				
			}
			else
			{
				vramHigh = memory[addr];
				t = (t & 0xFF) | ((memory[addr] & 0b00111111) << 8);
				// last bit is cleared
				t = t & 0x7FFF; // old me said 3FFF but this makes no sense
				
			}
			secondWrite = !secondWrite;
			break;
		case 0x2007:
			//vramAddr = v &0xC000);
			data = memory[addr];

			if (vramAddr >= 0x3000) {
				vram[vramAddr] = memory[addr];
				vram[vramAddr - 0x1000] = memory[addr];

				if (vramAddr == 0x3F00 || vramAddr == 0x3F10) {
					vram[0x3F00] = data;
					vram[0x3F10] = data;
				}
			}
			else if (vMirror) {
				
				// -----------------
				// | 2000   |  2400 |
				// |   A    |   B   |
				// |        |       |
				// |----------------|
				// | 2800   |  2C00 |
				// |   A    |   B   |
				// |        |       |
				// ------------------
				

				if ((vramAddr >= 0x2000 && vramAddr < 0x2800)) {

					vram[vramAddr] = data;
					vram[vramAddr + 0x800] = data;
				}
				else if (vramAddr >= 0x2800 && vramAddr < 0x3000) {
					vram[vramAddr] = data;
					vram[vramAddr - 0x800] = data;
				}
				else {
					vram[vramAddr] = data;
				}
			}
			else {
				// -----------------
				// | 2000   |  2400 |
				// |   A    |   A   |
				// |        |       |
				// |----------------|
				// | 2800   |  2C00 |
				// |   B    |   B   |
				// |        |       |
				// ------------------

				if ((vramAddr >= 0x2000 && vramAddr < 0x2400) || (vramAddr >= 0x2800 && vramAddr < 0x2C00)) {
					vram[vramAddr] = data;
					vram[vramAddr + 0x400] = data;
				}
				else if ((vramAddr >= 0x2400 && vramAddr < 0x2800) || (vramAddr >= 0x2C00 && vramAddr < 0x3000)) {
					vram[vramAddr] = data;
					vram[vramAddr - 0x400] = data;
				}
				else {
					vram[vramAddr] = data;
				}
			}

			// write to vram then increment based on ppucontroll
			if ((ctrl & 0b00000100) > 0) {
				vramAddr += 32;
				v += 32;
			}
			else {
				vramAddr += 1;
				v += 1;
			}

			vramAddr = vramAddr % 0x4000;
			v = v % 0x4000;
			break;
		case 0x4014:
			// flood fill
			std::copy(memory + (memory[addr] << 8),
				memory + (memory[addr] << 8) + 0x100,
				oam);
			

			break;
		default:
			break;
		}

	}
}

void ppu::incX() {
	if ((v & 0x001F) == 31) {
		v = v & (~0x001F); // clear coarse x
		v = v ^ 0x0400;
	}
	else {
		v += 1;
	}
}

void ppu::incY() {
	if ((v & 0x7000) != 0x7000) {
		v += 0x1000;
	}
	else {
		v = v & (~0x7000);
		int y = (v & 0x03E0) >> 5;
		if (y == 29) {
			y = 0;
			v = v ^ 0x0800;
		}
		else if (y == 31) {
			y = 0;
		}
		else {
			y += 1;
		}
		v = (v & ~0x03E0) | (y << 5);
	}
}