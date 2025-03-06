
/*
// load rom

std::ifstream rom("./sled.nes", std::fstream::in | std::ios::binary);
int ref = 0;
uint8_t ch;

// charecter memory
uint8_t chMem[8096];
int point = 0;
int i = 0;
SDL_Rect rect;
rect.x = 0;
rect.y = 0;
rect.w = 5;
rect.h = 5;

uint8_t PRGROMSIZE = 0;
uint8_t CHROMSIZE = 0;
if (rom.is_open()) {
    while (rom >> std::noskipws >> ch) {

        // get prg rom size
        if (i == 4) {
            PRGROMSIZE = ch;
        }
        if (i == 5) {
            CHROMSIZE = ch;
            std::cout << "PRG" << unsigned(PRGROMSIZE) << "CHR" << unsigned(CHROMSIZE);
        }
        if (i == 7 && (ch & 0x0C) == 0x088) {
            std::cout << "INES";
        }
        //std::cout << " " << std::hex << unsigned(ch);
        if (i > 8096) {
            drawBlock(renderer, ch & 0b11000000, rect);
            rect.x += 5;
            if (rect.x > 600) {
                rect.x = 0;
                rect.y += 5;
            }
            drawBlock(renderer, ch & 0b00110000, rect);
            rect.x += 5;
            if (rect.x > 600) {
                rect.x = 0;
                rect.y += 5;
            }
            drawBlock(renderer, ch & 0b00001100, rect);
            rect.x += 5;
            if (rect.x > 600) {
                rect.x = 0;
                rect.y += 5;
            }
            drawBlock(renderer, ch & 0b00000011, rect);
            rect.x += 5;
            if (rect.x > 600) {
                rect.x = 0;
                rect.y += 5;
            }
        }

        i++;
    }

}




void drawBlock(SDL_Renderer * r, uint8_t data, SDL_Rect square) {
    SDL_SetRenderDrawColor(r, 0, 255, 255, 255);
    switch (unsigned(data & 0b00000011)) {
    case 0:
        SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
        break;
    case 1:
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        break;
    case 2:
        SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
        break;
    case 3:
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        break;
    }

    SDL_RenderFillRect(r, &square);
    SDL_RenderPresent(r);
}
*/