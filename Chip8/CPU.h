#pragma once

#include <stdio.h>
#include <stdlib.h>

class CPU
{

private:
	//This is the current Op Code
	unsigned short m_OpCode;
	unsigned char m_Memory[4096];

	//The Chip8 has 15 8-Bit Register. The 16th one is used for flags
	unsigned char m_Registers[16];

	//Index Register
	unsigned short m_IR;
	//Program Counter both go from 0x000 to 0xFFF
	unsigned short m_PC;

	unsigned char m_DelayTimer;
	unsigned char m_SoundTimer;

	unsigned short m_Stack[16];
	unsigned short m_StackPointer;

	unsigned char m_Chip8Font[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	};

	//Resets Memory and Pointers
	void Init();

public:
	const char* FileName;
	unsigned char m_PixelData[64 * 32];
	unsigned char m_Keys[16];

	bool Debug = false;

	//Loads the Rom into Memory
	bool LoadRom(const char *a_FileName);

	//Fetches an OpCode and compiles it
	void Emulate();
};