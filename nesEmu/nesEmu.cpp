// nesEmu.cpp : This file contains the 'main' function. Program execution begins and ends there.
// use gtk

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "SDL_ttf.h"
#include "Controller.h"
#include "_6502.h";
#include "PpuRenderer.h"
#include "SDL.h"
#include "ppu.h"
#include "dissassemble.h"



void log(_6502& cpu) {
    std::cout << "flags: " << std::hex << int(cpu.SR);
    std::cout << " ac: " << std::hex << int(cpu.ac);
    std::cout << " X: " << std::hex << int(cpu.x);
    std::cout << " Y: " << std::hex << int(cpu.y);
    std::cout << " PC: " << std::hex << int(cpu.pc);
    std::cout << " mem zero: " << std::hex << int(cpu.memory[0]);
    std::cout << " mem two: " << std::hex << int(cpu.memory[2]);
}


void loadPalette(const char src[], SDL_Renderer* render, PpuRenderer* pr) {
    std::ifstream rom(src, std::fstream::in | std::ios::binary);
    uint8_t ch;
    unsigned int i = 0;
    int k = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    int count = 0;
   
    SDL_Rect square = { 0, 0, 10, 10 };

    if (rom.is_open()) {
        while (rom >> std::noskipws >> ch) {

            i++;
            switch (i) {
            case 1:
                r = ch;
                break;
            case 2:
                g = ch;
                break;
            case 3:
                b = ch;
                if (i > 0) {
                    std::cout << "R:" << unsigned(r);

                    std::cout << " G:" << unsigned(r);

                    std::cout << " B:" << unsigned(b);

                    std::cout << " at:" << unsigned(k / 3) << "\n";
                    SDL_Color c = { r, g, b, 0xFF };
                    drawRGB(render, square, c);
                    if (((k + 1) / 3) % 8 == 0) {
                        square.x = 0;
                        square.y += 10;
                    }
                    else {
                        square.x += 10;
                    }
                    pr->paletteMap[k / 3] = c;
                    i = 0;
                }
                break;
            }
            k++;
        }

    }

    rom.close();
}

// map INES
void loadINES(const char text[], SDL_Renderer* r, _6502& cpu, ppu& p) {
    std::ifstream rom("./stress.nes", std::fstream::in | std::ios::binary);
    uint8_t ch;
    unsigned int prgROMSize = 999999;
    unsigned int chrROMSize = 999999;
    unsigned int i = 0;

    unsigned int offsetL = 0x8000;
    unsigned int offsetR = 0xC000;
    unsigned int count = 0;
    bool isLow = true;
    uint8_t chMEM[8192] = { 0 };
    uint8_t prgRom[0x10000] = { 0xFF };
    
    if (rom.is_open()) {
        while (rom >> std::noskipws >> ch) {
            if (i == 4) {
                prgROMSize = unsigned(ch);
            }
            if (i == 5) {
                chrROMSize = unsigned(ch);

            }
            if (i == 6) {
                if ((ch & 1) == 0) {
                    p.vMirror = false;
                }
                else {
                    p.vMirror = true;
                }
            }
            // parse prg rom
            if (prgROMSize == 1) {
                if (i > 0xF && i <= 15 + 16384 * prgROMSize) {
                    cpu.memory[offsetL + i - 0x10] = ch;
                    cpu.memory[offsetR + i - 0x10] = ch;
                }
            }
            else {
                if (i > 0xF && i <= 15 + 16384 * prgROMSize) {
                    cpu.memory[offsetL + i - 0x10] = ch;
                }
            }
            // parse ch rom
            if (i > (15 + 16384 * prgROMSize) && i < (15 + 16384 * prgROMSize) + 8192 * chrROMSize) {
                chMEM[count] = ch;
                p.patternTable[count] = ch;

                count++;
            }
            i++;
        }
    }

    std::cout << "DIFF" << ((15 + 16384 * prgROMSize) - (15 + 16384 * prgROMSize) + 8192 * chrROMSize);
    rom.close();
    // load the program too the cpu
    //std::copy(std::begin(cpu.memory), std::end(cpu.memory), std::begin(prgRom));

    unsigned int sprCount = 0;
    unsigned int size = 5;
    unsigned int padding = 2;
    SDL_Rect square = { 0, 0, size, size };
    unsigned int numPerRow = 30;
    unsigned int x = 0;
    unsigned int y = 0;
    for (unsigned int i = 0x00; i < 8192 * chrROMSize; i = i + 16) {
        drawSprite(r, (i/16), chMEM, &square, size);
        sprCount++;
        square.y = y;
        x += size * 8 + padding;
        square.x = x;
        if (sprCount%numPerRow == 0) {
            y += size * 8 + padding;
            x = 0;
            square.x = 0;
            square.y = y;
        }
    }
    SDL_RenderPresent(r);
    //std::cout << "SPRITE COUNT IS" << spriteCount;
}

void drawvRam(SDL_Renderer* r, ppu* p) {
    const int sx = 500;
    SDL_Rect square = { sx, 0, 6, 6 };
    for (int i = 0x2000; i < 0x2400; i ++) {
        drawBlock(r, p->vram[i], square);
        square.x += 6;
        if(i%32 == 0){
            square.y += 6;
            square.x = sx;
        }
    }
    square.x = 32 * 6 + sx;
    square.y = 0;
    for (int i = 0x2400; i < 0x27FF; i++) {
        drawBlock(r, p->vram[i], square);
        square.x += 6;
        if (i % 32 == 0) {
            square.y += 6;
            square.x = 30*6 + sx;
        }
    }
    square.y = 30 * 6;
    for (int i = 0x2800; i < 0x2BFF; i++) {
        drawBlock(r, p->vram[i], square);
        square.x += 6;
        if (i % 32 == 0) {
            square.y += 6;
            square.x = sx;
        }
    }
    square.x = 32 * 6 + sx;
    square.y = 30 * 6;
    for (int i = 0x2C00; i < 0x2FFF; i++) {
        drawBlock(r, p->vram[i], square);
        square.x += 6;
        if (i % 32 == 0) {
            square.y += 6;
            square.x = 30 * 6 + sx;
        }
    }
    square.w = 2;
    square.h = 6 * 60;
    square.y = 0;
    square.x = (p->scrollX  * 8) + sx;
    //drawBlock(r, 0b11, square);
    square.x += 32;
    square.x = (square.x % 64) + sx;
    //rawBlock(r, 0b11, square);
}

void print(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, std::string text, int x, int y) {

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color);

    // turn the surface to a texture
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);


    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    TTF_SizeText(font, text.c_str(), &messageRect.w, &messageRect.h);
    TTF_SizeText(font, text.c_str(), &messageRect.w, &messageRect.h);
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

std::string toHex(uint16_t num, uint8_t size = 2) {
    std::stringstream sstream;
    sstream << std::setfill('0') << std::setw(size) << std::uppercase << std::hex << unsigned(num & (0xFFFF >> (4 - size)));
    return sstream.str();
}


void drawBackground(ppu p, SDL_Renderer* r, TTF_Font* sans, SDL_Color c) {
    // draw the nametable at 0x2000
    SDL_Rect square = { 0,0, 2, 2 };
    int y = 0;
    for (int i = 1; i <= 0x3C0; i++) {

        //drawSprite(r, p.vram[0x2000 + i], p.patternTable, &square, 8)
        y = square.y;
        if (p.vram[0x2000 + i - 1] != 0) {
         drawSprite(r, p.vram[0x2000 + i - 1], p.patternTable, &square, 2);
        }
        square.y = y;
        square.x += 16;
        if (i % 32 == 0) {
            square.x = 0;
            square.y += 16;
        }
    }
    print(r, sans, c, "PPU STATUS: Addr:" + toHex(p.vramAddr, 4) + " Data: " + toHex(p.data), 0, 570);
}


void draw(Dissassembler d, uint8_t memory[], SDL_Renderer* renderer, TTF_Font* font, SDL_Color textC, SDL_Color currC, _6502 cpu) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 0);
    SDL_RenderClear(renderer);
    std::vector<std::string> text = d.dissassembleAll(&cpu);
    int y = 0;
    int x = 0;
    unsigned int vecSize = text.size();

    for (unsigned int i = 0; i < vecSize; i++) {

        if (i == d.currInstr) {
            print(renderer, font, currC, text[i], x, y);
        }
        else {

            print(renderer, font, textC, text[i], x, y);
        }
        y += 30;
        if (y >= 350) {
            y = 0;
            x += 250;
        }
    }
    print(renderer, font, currC, "PC: " + toHex(cpu.pc, 4) + " X:" + toHex(cpu.x) + ", Y: " + toHex(cpu.y) + ", AC :" + toHex(cpu.ac) + ", FLAGS:" + toHex(cpu.SR) + ", PPU: " + toHex(cpu.ppuEnable), 0, 370);
}


void writeToLog(std::ofstream* log, _6502* cpu) {
    (*log) << toHex(cpu->pc, 4) + " " + toHex(cpu->ac & 0xFF) + " " + toHex(cpu->x & 0xFF) + " " + toHex(cpu->y & 0xFF) + " " + toHex(cpu->SR) + " " + toHex(cpu->sp) + " " + std::to_string(cpu->cycleCount) << std::endl;
}

int main(int argc, char* argv[])
{
    // basic SDL inititation
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    // window used to show the roms
    SDL_Window* gWindow = SDL_CreateWindow("graphics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, 0);

    // window used for the cpu
    SDL_Window* window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // the window where the game is displayed
    SDL_Window* gameWindow = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* gameRenderer = SDL_CreateRenderer(gameWindow, -1, 0);


    // clear the cpu intructions dont clear graphics window because it is overwritten
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 0);
    SDL_RenderClear(gameRenderer);
    SDL_RenderPresent(gameRenderer);


    uint8_t memory[0x10000] = {0xFF};

    // create a controller
    Controller c;

    // INIT cpu
    _6502 cpu(memory, &c);
    // FONT
    TTF_Font* Sans = TTF_OpenFont("./font.ttf", 24);

    // COLORS
    SDL_Color White = { 255,255,255 };
    SDL_Color Green = { 0,255,0 };


    // create a new dissessembler
    Dissassembler d;
    //d.pc = &cpu.pc;

    // make a ppu
    ppu p(cpu.memory);

    // give the cpu the ppu
    cpu.p = &p;


    // load the rom and display it
    loadINES("", gRenderer, cpu, p);
    SDL_RenderPresent(gRenderer);
    std::cout << "MEM IS" << unsigned(cpu.memory[0xFF78]);
    cpu.pc = (cpu.memory[0xFFFD] << 8) + cpu.memory[0xFFFC];
    std::cout << "PC IS " << cpu.pc << std::endl;
    cpu.NMI_addr = (cpu.memory[0xFFFB] << 8) + cpu.memory[0xFFFA];
    //cpu.pc = 0xC000;
    // dissessembles the cpus memory given d then draw it
    //draw(d, cpu.memory, renderer, Sans, White, Green, cpu);
    //SDL_RenderPresent(renderer);


    bool exit = false;
    SDL_Event eventData;

    std::ofstream log;

    p.vram[0x2000] = 8;
    p.vram[0x2020] = 2;
    p.vram[0x2050] = 10;

    p.vram[0x2001] = 3;

    PpuRenderer pRenderer(&p, gameRenderer);

    // load the palette
    loadPalette("./pals/Real.pal", renderer, &pRenderer);

    bool frameFin = false;

    log.open("log.txt");
    time_t start = time(0);
    int frames = 0;

    SDL_Rect r = { 0, 0, 512, 241 };

    bool assembler = false;
    uint32_t brk = 0x10000;


    while (!exit)
    {
        frameFin = false;
        if ((brk & 0x80000000) == 0) {
            while (!frameFin)
            {
                //SDL_RenderClear(gameRenderer);
                //writeToLog(&log, &cpu);
                cpu.step();

                if (cpu.ppuEnable > 0) {
                    p.evaluatePPURegisters(cpu.ppuEnable, cpu.ppuRead);
                }
                for (int i = 0; i < cpu.cycleCount * 3; i++) {
                    pRenderer.cycle();
                    if (pRenderer.scanlines == 241 && pRenderer.cycles % 341 == 0 && (p.ctrl >> 7) == 1) {


                        SDL_UnlockTexture(pRenderer.texture);
                        SDL_RenderCopy(gameRenderer, pRenderer.texture, &r, NULL);
                        SDL_RenderPresent(gameRenderer);
                        //*p.status = *p.status | 0x80;
                        cpu.NMI();
                        //p.ctrl = (p.ctrl & 0x7F);
                    }
                    if (pRenderer.scanlines == 262) {
                        pRenderer.reset();
                        frameFin = true;
                        if (frames % 10 == 0) {
                            //pRenderer.sx += 1;
                        }
                        frames += 1;
                    }
                }
                cpu.cycleCount = 0;
                if (cpu.pc == 0x892A && cpu.x == 0x16 && cpu.y == 0x5 && cpu.ac == 0x05 && false) {
                    brk = brk | 0x80000000;
                    std::cout << "AC IS :" << (uint16_t)cpu.ac << "VRAM" << (uint16_t)cpu.memory[0x2007] << " Y " << (uint16_t)cpu.y << "VRAM" << (uint16_t)cpu.memory[0x3] << std::endl;
                    break;
                }
                if (brk & 0x80000000) {
                    std::cout << "HI!\n";
                }
            }

        }

       // SDL_RenderClear(renderer);
       // drawvRam(gameRenderer, &p);

        //SDL_RenderPresent(gameRenderer);
        
        //print(renderer, Sans, White, "frames " + std::to_string(static_cast<double>(frames) / difftime(time(0), start)), 0, 0);
        if (!assembler) {
            print(renderer, Sans, White, "MEMORY " + toHex(cpu.memory[0x6000]) + " NMI EN: " + toHex(p.ctrl >> 7), 0, 0);
            SDL_RenderClear(renderer);
        }

        
        SDL_RenderPresent(renderer);
        if (c.isPolling) {
            c.shiftVal = 0;
        }
        while (SDL_PollEvent(&eventData))
        {

            switch (eventData.type)
            {
            case SDL_QUIT:
                exit = true;
                break;

            case SDL_KEYDOWN:
                switch (eventData.key.keysym.sym)
                {

                case SDLK_SPACE:
                    SDL_RenderClear(renderer);
                    // step the cpu
                    writeToLog(&log, &cpu);
                    cpu.step();

                    if (cpu.ppuEnable > 0) {
                        p.evaluatePPURegisters(cpu.ppuEnable, cpu.ppuRead);
                    }

                    d.currInstr = 0;
                    // draw the cpu memory
                    std::cout << "AC IS :" << (uint16_t)cpu.ac << "VRAM: " << (uint16_t)cpu.memory[0x2007] << " Y " << (uint16_t)cpu.y << "VRAM: " << (uint16_t)cpu.memory[0x3] << " SR: " << (uint16_t)cpu.SR << std::endl;
                    draw(d, cpu.memory, renderer, Sans, White, Green, cpu);
                    assembler = true;
                    break;
                case SDLK_1:
                    for (int i = 0; i < 32; i++) {

                        writeToLog(&log, &cpu);
                        cpu.step();

                        if (cpu.ppuEnable > 0) {
                            p.evaluatePPURegisters(cpu.ppuEnable, cpu.ppuRead);
                        }

                    }
                    assembler = !assembler;
                    d.currInstr = 0;
                    std::cout << "APLLETE: " << (uint16_t)p.vram[0x23F8] << std::endl;
                    // draw the cpu memory
                    draw(d, cpu.memory, renderer, Sans, White, Green, cpu);
                    break;
                case SDLK_9:
                    for (int i = 0; i < 1024; i++) {

                        writeToLog(&log,&cpu);
                        cpu.step();

                        if (cpu.ppuEnable > 0) {
                            p.evaluatePPURegisters(cpu.ppuEnable, cpu.ppuRead);
                        }

                    }
                    d.currInstr = 0;
                    // draw the cpu memory
                    draw(d, cpu.memory, renderer, Sans, White, Green, cpu);
                    break;
                case SDLK_8:
                    if (brk & 0x80000000) {
                        brk = (brk & ~0x80000000);
                    }
                    else {
                        // breaekpoint here
                        while (cpu.pc != 0xC084)
                        {

                            writeToLog(&log, &cpu);
                            cpu.step();

                            if (cpu.ppuEnable > 0) {
                                p.evaluatePPURegisters(cpu.ppuEnable, cpu.ppuRead);
                            }
                        }

                        d.currInstr = 0;
                        // draw the cpu memory
                        draw(d, cpu.memory, renderer, Sans, White, Green, cpu);
                    }
                    break;
                case SDLK_0:
                    std::cout << "CONTROLLER IS " << unsigned(c.controller) << "SP IS " << (uint16_t)cpu.sp << std::endl;
                    break;

                default:
                    break;
                }
                break;
            case SDL_WINDOWEVENT:
                if (eventData.window.event == SDL_WINDOWEVENT_CLOSE) {
                    if (SDL_GetWindowID(gWindow) == eventData.window.windowID) {
                        exit = true;
                    }
                }
                break;
            }
        }
    }
    log.close();
    return 0;
}

