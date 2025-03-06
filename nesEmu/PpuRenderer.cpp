#include "PpuRenderer.h"


PpuRenderer::PpuRenderer(ppu* PPU, SDL_Renderer* renderer) {
	r = renderer;
    p = PPU;
    texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 1024, 1024);
    
    SDL_QueryTexture(texture, &f.format, nullptr, NULL, NULL);
    RED = SDL_MapRGB(&f, 255, 0, 0);
    GREEN = SDL_MapRGB(&f, 0, 255, 0);
    BLUE = SDL_MapRGB(&f, 0, 0, 255); 
    reset();
    scanlines = 0;

}

void drawBlock(SDL_Renderer* r, uint8_t data, SDL_Rect square) {
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    switch (unsigned(data & 0b00000011)) {
    case 0:
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        break;
    case 1:
        SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
        break;
    case 2:
        SDL_SetRenderDrawColor(r, 0, 255, 0, 255);
        break;
    case 3:
        SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
        break;
    }

    SDL_RenderFillRect(r, &square);

}

void drawRGB(SDL_Renderer* r, SDL_Rect square, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);

    SDL_RenderFillRect(r, &square);
}

void drawRow(SDL_Renderer* r, uint16_t data1, uint16_t data2, SDL_Rect* box, int size, bool xFlip) {
    int originX = box->x;
    uint8_t m = 0b10000000;
    if (xFlip) {
        m = 1;
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (xFlip) {
            drawBlock(r, (((data2 & m) >> (i)) << 1) + (((data1 & m) >> (i))), *box);
            m = m << 1;
        }
        else {
            drawBlock(r, (((data2 & m) >> (7 - i)) << 1) + (((data1 & m) >> (7 - i))), *box);
            m = m >> 1;
        }
        box->x += size;
    }

    box->y += size;
    box->x = originX;
}

void drawSprite(SDL_Renderer* r, int sprID, uint8_t chMEM[8192], SDL_Rect* square, int size, bool xFlip, bool yflip) {
    for (int j = 0; j < 8; j++) {
drawRow(r, chMEM[(sprID * 16) + j], chMEM[(sprID * 16) + j + 8], square, size, xFlip);
    }
}


void PpuRenderer::drawRow(uint16_t data1, uint16_t data2, bool xFlip) {
    int originX = square.x;
    uint8_t m = 0b10000000;
    if (xFlip) {
        m = 1;
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (xFlip) {
            drawCycle((((data2 & m) >> (i)) << 1) + (((data1 & m) >> (i))), 0);
            m = m << 1;
        }
        else {
            drawCycle((((data2 & m) >> (7 - i)) << 1) + (((data1 & m) >> (7 - i))), 0);
            m = m >> 1;
        }
        square.x += pixelSize;
    }

    square.y += pixelSize;
    square.x = originX;
}

void PpuRenderer::drawSprite(int sprID, bool xFlip, bool yflip) {
    for (int j = 0; j < 8; j++) {
        if (!yflip) {
            drawRow(p->patternTable[(sprID * 16) + j], p->patternTable[(sprID * 16) + j + 8], xFlip);
        }
        else {
            drawRow(p->patternTable[(sprID * 16) + (7 - j)], p->patternTable[(sprID * 16) + (7 - j) + 8], xFlip);
        }
    }
}

void PpuRenderer::drawCycle(uint8_t data, uint8_t paletteNum) {
    SDL_Color c = paletteMap[p->vram[0x3F00 + data + (paletteNum * 4)]];
    pixelColor = (c.r << 16) + (c.g << 8) + c.b;
    ///std::cout << std::hex << "color is " << pixelColor << "\n";

    if (data > 0) {
        pixels[square.y * (pitch / sizeof(unsigned int)) + square.x] = pixelColor;
    }
    else {
        SDL_Color c = paletteMap[p->vram[0x3F00]];
        pixelColor = (c.r << 16) + (c.g << 8) + c.b;
        pixels[square.y * (pitch / sizeof(unsigned int)) + square.x] = pixelColor;
    }
}


void PpuRenderer::drawOAM() {
    for (int i = 0; i < 64; i++) {
        square.y = p->oam[(4 * i)];
        square.x = p->oam[4 * i + 3];
        if (((p->oam[4 * i + 2] >> 6) & 1) > 0) {
            drawSprite(p->oam[4 * i + 1], true, p->oam[4 * i + 2] >> 7);
        }
        else {

            drawSprite(p->oam[4 * i + 1], false, p->oam[4 * i + 2] >> 7);
        }
    }
}

void PpuRenderer::spriteEval() {
    int c = 0;
    spr0 = 0xFF;
    for (int i = 0; i < 64; i++) {
        if (scanlines > p->oam[4 * i] && scanlines  <= p->oam[4 * i] + 8)
        {
            if (c < 64) {
                if (i == 0) {
                    // >> d is divided by 4
                    spr0 = c >> 2;
                }
                std::copy_n(p->oam + (i * 4), 4, sprsOnLine + c);
            }
            c += 4;
        }
    }

    // means over 8 are on scanine so set overflow
    if (c > 64) {
        *p->status = *p->status | 0b00100000;
    }
}

void PpuRenderer::cycle() {
    if ((cycles%341) == 0) {
        cycles++;
        if (((frames + 1) % 2) == 0) {
            return;
        }
    }



    if (scanlines <= 240) {



        if (cycles % (341) <= 256) {
            // cycles is how many frames!
            // magic math to get the srite ID
            // go x/8 across and y/8 down
            sprID = p->vram[0x2000 | (p->v & 0x0FFF)];


           
            if (x == 0) {

                lowShift = p->patternTable[((sprID) * 16) + (p->v >> 12) + 4096 * ((p->ctrl >> 4) & 1)] << 8;
                highShift = p->patternTable[((sprID) * 16) + (p->v >> 12) + 8 + 4096 * ((p->ctrl >> 4) & 1)] << 8;
                palletteShift |= p->vram[0x23C0 | (p->v & 0x0C00) | ((p->v >> 4) & 0x38) | ((p->v >> 2) & 0x07)];
            }

            if (x % 8 == 0) {
                lowShift = (lowShift & 0xFF00);
                highShift = (highShift & 0xFF00);
                palletteShift = (palletteShift & 0xFF00);


                uint16_t tempV = p->v;
                p->incX();
                sprID = p->vram[0x2000 | ((p->v) & 0x0FFF)];



                lowShift |= p->patternTable[((sprID) * 16) + ((p->v) >> 12) + 4096 * ((p->ctrl >> 4) & 1)];
                highShift |= p->patternTable[((sprID) * 16) + ((p->v) >> 12) + 8 + 4096 * ((p->ctrl >> 4) & 1)];
                palletteShift |= p->vram[0x23C0 | (p->v & 0x0C00) | ((p->v >> 4) & 0x38) | ((p->v >> 2) & 0x07)];
                p->v = tempV;
            }


            sprID = p->vram[0x2000 | (p->v & 0x0FFF)];

            // amount we will shift: 0 = bottom right, 2, bottom left, 4 = top right, 6 = top left
            // bit 0 unused
            // bit 1 x enable
            // bit 2 y enable
            uint8_t palQuad = ((((p->v ) & 0b10)) | ((p->v & 64) >> 4));

            //std::cout << unsigned(palQuad);
            //std::cout << unsigned(palQuad) << " ";
            if (p->scrollX + (x % 8) >= 8) {
                palQuad = ((((p->v + 1) & 0b10)) | ((p->v & 64) >> 4));
                palleteNum = ((palletteShift) & (0b11 << palQuad)) >> (palQuad);


            }
            else {
                palleteNum = ((palletteShift >> 8) & (0b11 << palQuad)) >> (palQuad);
            }
            



            lowData = (lowShift & (0x8000 >> p->scrollX)) >> (15 - p->scrollX);
            lowShift = lowShift << 1;

            highData = (highShift & (0x8000 >> p->scrollX)) >> (15 - p->scrollX);
            highShift = highShift << 1;
            

            // sprite rendering
            for (int i = 0; i < 8; i++) {
                if ((x >= sprsOnLine[(4 * i + 3)] && x < sprsOnLine[(4 * i + 3)] + 8)) {

                    sprID = sprsOnLine[i * 4 + 1];


                    int bg = lowData | (highData << 1);


                    int sprLowData = p->patternTable[((sprID) * 16) + (scanlines - sprsOnLine[i * 4] - 1) + 4096 * ((p->ctrl >> 3) & 1)];
                    int sprHighData = p->patternTable[((sprID) * 16) + (scanlines - sprsOnLine[i * 4] - 1) + 4096 * ((p->ctrl >> 3) & 1) + 8];
                    int sprPixelNum = x - sprsOnLine[(4 * i + 3)];;
                    if ((bg == 0 && ((sprsOnLine[i * 4 + 2] & 0b100000) >> 5) == 1) || ((sprsOnLine[i * 4 + 2] & 0b100000) >> 5) == 0) {
                        



                        pixelNum =  sprPixelNum;

                        // horizontal mirroring
                        if (sprsOnLine[(4 * i + 2)] >> 6 == 0) {
                            mask = 0b10000000 >> pixelNum;

                            pixelNum = 7 - pixelNum;
                        }
                        else {
                            mask = 1 << pixelNum;
                        }
                        if (((sprLowData & mask) >> pixelNum) != 0 || ((sprHighData & mask) >> pixelNum) != 0 ) {
                            lowData = (sprLowData & mask) >> pixelNum;
                            highData = (sprHighData & mask) >> pixelNum;
                            palleteNum = (sprsOnLine[i * 4 + 2] & 0b11) + 4;
                        }
                        

                    }

  

                    if (i == spr0 && (((sprLowData & mask) >> sprPixelNum)) + ((((sprHighData & mask) >> sprPixelNum)) << 1) > 0) { // bg > 0
                        zeroNextLine = 1;
                    }


                    break;
                }
            }


            // this speghetti code is just get the high bit from high data and the low from low

            //drawCycle((((lowData & mask) >> pixelNum)) + ((((highData & mask) >> pixelNum)) << 1), palleteNum);
            //if (palleteNum != 0) {
            //    std::cout << (uint16_t)palleteNum << " " << (uint16_t)(lowData | (highData << 1)) << std::endl;
            //}
            
            drawCycle(lowData | (highData << 1), palleteNum);


            x++;

            if (x % 8 == 0) {
                palletteShift = palletteShift << 8;
                p->incX();


            }

            square.x += pixelSize;


        }

    }
    else {
        x++;

    }

    if (zeroNextLine) {

        *p->status = *p->status | 0b01000000;
    }

    cycles++;

    if (x == 256) {
 
        x = 0;
        square.x = 0;

        p->incY();

        y++;

        square.y += pixelSize;

    }


    if ((cycles % 341) == 257) {
       p->v = (p->v & 0b111101111100000) | (p->t & ~0b111101111100000);
       //std::cout << "THE X IS: " << (p->t & 0b11111) << std::endl;
    }

    if (scanlines == 261 && cycles % 341 >= 280 && cycles % 341 <= 304) {
        p->v =(p->v & 0b000010000011111) | (p->t & ~0b000010000011111);
        //std::cout << (p->t & 0b11111) << std::endl;
    }


    if (cycles % 341 == 0) {

        scanlines++;
        if (zeroNextLine > 0) {
            zeroNextLine -= 1;
            if (zeroNextLine == 0) {
                *p->status = *p->status | 0b01000000;
            }
        }

        //std::cout << unsigned(scanlines) << " : " << unsigned(cycles) << "\n\n\n";
        std::fill(std::begin(sprsOnLine), std::end(sprsOnLine), palleteNum);
        if (((p->mask >> 4) & 1) == 1) {
            spriteEval();
        }

        if (scanlines == 261) {

            *p->status = (*p->status & 0b00111111);
            //p->v = p->t;
        }
        else if (scanlines == 241) {
            *(p->status) = *(p->status) | 0x80;

        }
        //cycles = 0;
        x = 0;
    }
}

void PpuRenderer::oldCycle() {
    if (cycles == 0) {
        *p->status = *p->status & 0b10111111;
    }
    if (x == 0) {
        cycles++;
        if (((frames + 1) % 2) == 0) {
            return;
        }
    }
    if (scanlines <= 240) {

        if (cycles % (341) <= 256) {
            // cycles is how many frames!
            // magic math to get the srite ID
            // go x/8 across and y/8 down

            nameTableAddr = 0x2000 + (0x400 * (p->ctrl & 0b11));
            if (((x + p->scrollX) >> 3) >= 32 && false) {
                nameTableAddr += 0x400;
                sprID = p->vram[nameTableAddr + ((x + p->scrollX) >> 3) - 32 + (((y >> 3) * 32))];
                palleteNum = p->vram[0x27C0 | ((p->ctrl & 0b11) << 8) | ((y >> 5) << 3) | ((((x + p->scrollX - (32 * 8)) >> 5)))] & 0b11;
            }
            else {
                sprID = p->vram[nameTableAddr + ((x + p->scrollX) >> 3) + (((y >> 3) * 32))];

                //std::cout << " X: " << (x + p->scrollX) % 2 << "\n";
                // (0b11000000 >> ((x + p->scrollX) % 2) >> ((y%2) * 2))) >> (8 - ((x + p->scrollX) % 2) - ((y % 2) * 2));
                palleteNum = (p->vram[0x23C0 | ((p->ctrl & 0b11) << 8) | ((y >> 5) << 3) | (((x + p->scrollX) >> 5))] & 0b11);
                //std::cout << "X" << ((x + p->scrollX) >> 4) << " Y: " << (y >> 4) << "\n";

            }
            //palleteNum = 0;

            //mask = 0b10000000 >> ((x + p->scrollX) % 8);
            //pixelNum = 7 - ((x + p->scrollX) % 8);

            //lowData = p->patternTable[((sprID) * 16) + (y % 8) + 4096];
            //highData = p->patternTable[((sprID) * 16) + (y % 8) + 8 + 4096];


            for (int i = 0; i < 8; i++) {
                if ((x >= sprsOnLine[(4 * i + 3)] && x < sprsOnLine[(4 * i + 3)] + 8)) {

                    sprID = sprsOnLine[i * 4 + 1];

                    palleteNum = (sprsOnLine[i * 4 + 2] & 0b11) + 4;

                    lowData = p->patternTable[((sprID) * 16) + (scanlines - sprsOnLine[i * 4])];
                    highData = p->patternTable[((sprID) * 16) + (scanlines - sprsOnLine[i * 4]) + 8];


                    pixelNum = x - sprsOnLine[(4 * i + 3)];

                    // horizontal mirroring
                    if (sprsOnLine[(4 * i + 2)] >> 6 == 0) {
                        mask = 0b10000000 >> pixelNum;

                        pixelNum = 7 - pixelNum;
                    }
                    else {
                        mask = 1 << pixelNum;
                    }

                    if (i == spr0 && (((lowData & mask) >> pixelNum)) + ((((highData & mask) >> pixelNum)) << 1) > 0) {
                        *p->status = *p->status | 0b01000000;
                    }
                    break;
                }
            }


            // this speghetti code is just get the high bit from high data and the low from low

            drawCycle((((lowData & mask) >> pixelNum)) + ((((highData & mask) >> pixelNum)) << 1), palleteNum);
            x++;
            square.x += pixelSize;
            if (x == 256) {
                //x = 0;
                square.x = 0;
                if (scanlines != 1) {
                    y++;

                    square.y += pixelSize;

                }

            }

        }

    }
    else {
        x++;

    }
    
    if(cycles%320 == 0){
//        lowShift = p->patternTable[((sprID) * 16) + (scanlines - sprsOnLine[i * 4])];
//        highShift = p->patternTable[((sprID) * 16) + (scanlines - sprsOnLine[i * 4]) + 8];
    }

    // coppies the horizontal bits
    if (cycles % 341 == 257) {
        std::cout << "YEAH";
        p->v = (p->v & 0b111101111100000) | (p->t & (~0b111101111100000));
    }

    if (scanlines == 261 && (cycles % 341) >= 280 && (cycles % 341) <= 304) {
        p->v = (p->v & 000010000011111) | p->t & ~0000100000111;
    }
    cycles++;


    if (cycles % 341 == 0) {

        scanlines++;
        //std::cout << unsigned(scanlines) << " : " << unsigned(cycles) << "\n\n\n";
        std::fill(std::begin(sprsOnLine), std::end(sprsOnLine), palleteNum);
        spriteEval();
        if (scanlines == 241) {
            *p->status = (*p->status | 0b10000000);
        }
        else if (scanlines == 261) {

            *p->status = (*p->status & 0b00111111);
        }
        //cycles = 0;
        x = 0;
    }

}



void PpuRenderer::reset() {
    cycles = 0;
    x = 0;
    y = 0;
    square.x = 0;
    square.y = 0;
    scanlines = 0;
    //*p->status = 0;
    //SDL_SetRenderTarget(r, texture);
    if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch) != 0) {
        std::cout << "ERROR " << SDL_GetError();
    }
    spriteEval();

    // fill the shift registers
    
    lowShift = p->patternTable[((sprID) * 16) + (p->v >> 12) + 4096 * ((p->ctrl >> 4) & 1)];
    highShift = p->patternTable[((sprID) * 16) + (p->v >> 12) + 8 + 4096 * ((p->ctrl >> 4) & 1)];
    //p->vramAddr = 0x2000;
}