# Nes Emulator

A fully functional NES emulator with a cycle acurate PPU (Picture Proccessing Unit) and debugging tools.

## Description

I started this project because after the creation of my [Chip8]() Emulator I wanted to tackle somthing a bit more difficult. Since then I have worked on this project on and off and am constantally trying to perfect it. Currently the CPU is the weakest area but I plan on re-writing most of it once I get some more time. In addition I would also like to get around too adding audio support.

### Features

* Written using SDL2 for graphics. Graphics code is seperate allowing for easy migration to other systems
* 6502 CPU Emulator written from complete scratch (with only instruction documentation)
* Illegal Opcode support
* Cycle acurate PPU allows for acurate depictions of rendering
* Debugging tools such as a art viewer and 6502 dissassembler
* Passes NEStest without any issue
* Passes the Nestress CPU tests

## Gallery


## Getting Started

### Dependencies

* SDL2
* Visual Studio
![Illegal](https://github.com/user-attachments/assets/e1dfb692-f79c-4e40-bee4-701aa7cc2318)
![NESTEST](https://github.com/user-attachments/assets/6ea4809e-bd0b-44c7-a0f6-6b5d4ea3a093)
![NESEMU](https://github.com/user-attachments/assets/330a269b-9748-4ccc-9440-49cf88f0540e)

### Executing program

* To execute this program you should just be able to open the solution in Visaul Studio and run it. To change the rom being opened just go to line 87 and change stress.nes to the name of your rom.

> NOTE: this emulator only supports [NROM](https://nesdir.github.io/mapper0.html) mapper zero games. Meaning that many NES game would require extra code to implement.

## TODO
- [X] Finish 6502 emulator
- [X] Catridge support 
- [X] Add PPU
- [X] Controller support
- [X] Fix rendering bug on Mario Bros
- [ ] Add APU
- [ ] Fix bug where sprite directions get messed up when enemies die (Mario)

