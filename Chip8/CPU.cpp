#include "CPU.h"

void CPU::Init()
{
	//The Rom starts at 0x200
	m_PC = 0x200;
	m_OpCode = 0;
	m_IR = 0;
	m_StackPointer = 0;
	m_DelayTimer = 0;
	m_SoundTimer = 0;

	for (int i = 0; i < 16; i++)
		m_Keys[i] = 0;

	for (int i = 0; i < 2048; i++)
		m_PixelData[i] = 0;

	for (int i = 0; i < 16; i++)
		m_Stack[i] = 0;

	for (int i = 0; i < 4096; i++)
		m_Memory[i] = 0;

	for (int i = 0; i < 80; i++)
		m_Memory[i] = m_Chip8Font[i];

}

bool CPU::LoadRom(const char *a_FileName)
{
	Init();

	FileName = a_FileName;

	FILE *file = fopen(a_FileName, "rb");
	if (file == NULL)
	{
		printf("File Error");
		return false;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(sizeof(char) * size);
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return false;
	}

	size_t result = fread(buffer, 1, size, file);
	if (result != size)
	{
		fputs("Reading error", stderr);
		return false;
	}

	if ((4069 - 512) > size)
	{
		for (int i = 0; i < size; i++)
		{
			m_Memory[i + 512] = buffer[i];
		}
	}
	else
	{
		printf("Rom is to big");
		return false;
	}

	fclose(file);
	free(buffer);
	return true;
}

void CPU::Emulate()
{
	//We need the next 2 bytes and combine them
	m_OpCode = m_Memory[m_PC] << 8 | m_Memory[m_PC + 1];

	if(Debug)
		printf("0x%X\n", m_OpCode);

	switch (m_OpCode & 0xF000)
	{
		case 0x0000:
			switch (m_OpCode & 0x000F)
			{
				case 0x0000: //Clear Screen command
					if (Debug)
						printf("Clear Screen\n");

					for (int i = 0; i < 2048; i++)
						m_PixelData[i] = 0x0;
						m_PC += 2;
					break;

				case 0x000E: //Return from subroutine
					if (Debug)
						printf("Return from subroutine\n");

					m_StackPointer--;
					m_PC = m_Stack[m_StackPointer];
					m_PC += 2;
					break;

				default:
					printf("Unknown opcode 0x%X\n", m_OpCode);

			}
			break;

		case 0x1000: //0x1NNN Jump to adress NNN
			if (Debug)
				printf("Jump to 0x%X\n", m_OpCode & 0x0FFF);

			m_PC = m_OpCode & 0x0FFF;
			break;

		case 0x2000: // 0x2NNN Calls subroutine at NNN
			if (Debug)
				printf("Call Subroutine 0x%X\n", m_OpCode & 0x0FFF);

			m_Stack[m_StackPointer] = m_PC; //Save the current PC
			m_StackPointer++; //Increment the pointer so we don't overwrite it next time
			m_PC = m_OpCode & 0x0FFF; // Jump to NNN
			break;

		case 0x3000: // 0x3XNN Skips the next instruction if VX equals NN
			if (m_Registers[(m_OpCode & 0x0F00) >> 8] == (m_OpCode & 0x00FF))
			{
				m_PC += 4;
				if (Debug)
					printf("Skipped to next Function\n");
			}
			else
			{
				m_PC += 2;
				if (Debug)
					printf("Didn't skipped the next Function\n");
			}
			break;

		case 0x4000: // 0x4XNN Skips the next instruction if VX does'nt equal NN
			if (m_Registers[(m_OpCode & 0x0F00) >> 8] != (m_OpCode & 0x00FF))
			{
				m_PC += 4;
				if (Debug)
					printf("Skipped to next Function\n");
			}
			else
			{
				m_PC += 2;
				if (Debug)
					printf("Didn't skipped the next Function\n");
			}
			break;

		case 0x5000: // 0x5XY0 Skips the next instruction if VX equals VY
			if (m_Registers[(m_OpCode & 0x0F00) >> 8] == m_Registers[m_OpCode & 0x00F0] >> 4)
			{
				m_PC += 4;
				if (Debug)
					printf("Skipped to next Function\n");
			}
			else
			{
				m_PC += 2;
				if (Debug)
					printf("Didn't skipped the next Function\n");
			}
			break;

		case 0x6000: // 0x6XNN Sets VX to NN
			if (Debug)
				printf("Setting Register %i to %i\n", (m_OpCode & 0x0F00) >> 8, m_OpCode & 0x00FF);

			m_Registers[(m_OpCode & 0x0F00) >> 8] = m_OpCode & 0x00FF;
			m_PC += 2;
			break;

		case 0x7000: // 0x7XNN Adds NN to VX
			if (Debug)
				printf("Add %i to Register %i\n", m_OpCode & 0x00FF, (m_OpCode & 0x0F00) >> 8);

			m_Registers[(m_OpCode & 0x0F00) >> 8] += m_OpCode & 0x00FF;
			m_PC += 2;
			break;

		case 0x8000:
			switch (m_OpCode & 0x000F)
			{
				case 0x0000: // 0x8XY0 Sets VX to the value of VY
					if (Debug)
						printf("Set Register %i to Register %i\n", (m_OpCode & 0x0F00) >> 8, (m_OpCode & 0x00F0) >> 4);

					m_Registers[(m_OpCode & 0x0F00) >> 8] = m_Registers[(m_OpCode & 0x00F0) >> 4];
					m_PC += 2;
					break;
			
				case 0x0001: // 0x8XY1 Sets VX to "VX or VY"
					m_Registers[(m_OpCode & 0x0F00) >> 8] |= m_Registers[(m_OpCode & 0x00F0) >> 4];
					m_PC += 2;
					break;

				case 0x0002: // 0x8XY2 Sets VX to "VX AND VY"
					m_Registers[(m_OpCode & 0x0F00) >> 8] &= m_Registers[(m_OpCode & 0x00F0) >> 4];
					m_PC += 2;
					break;

				case 0x0003: // 0x8XY3 Sets VX to "VX XOR VY"
					m_Registers[(m_OpCode & 0x0F00) >> 8] ^= m_Registers[(m_OpCode & 0x00F0) >> 4];
					m_PC += 2;
					break;

				case 0x0004: // 0x8XY4 Adds VY to VX. VF is set to 1 when there's a carry and to 0 when there isn't
					if (m_Registers[(m_OpCode & 0x00F0) >> 4] > (0xFF - m_Registers[(m_OpCode & 0x0F00) >> 8]))
						m_Registers[0xF] = 1; // Carry
					else
						m_Registers[0xF] = 0;
					m_Registers[(m_OpCode & 0x0F00) >> 8] += m_Registers[(m_OpCode & 0x00F0) >> 4];
					m_PC += 2;
					break;

				case 0x0005: // 0x8XY5 VY is subtracted from VX 
					if (m_Registers[(m_OpCode & 0x00F0) >> 4] > m_Registers[(m_OpCode & 0x0F00) >> 8])
						m_Registers[0xF] = 0;
					else
						m_Registers[0xF] = 1;
					m_Registers[(m_OpCode & 0x0F00) >> 8] -= m_Registers[(m_OpCode & 0x00F0) >> 4];
					m_PC += 2;
					break;

				case 0x0006: // 0x8XY6 Shifts VX right by one
					m_Registers[0xF] = m_Registers[(m_OpCode & 0x0F00) >> 8] & 0x1;
					m_Registers[(m_OpCode & 0x0F00) >> 8] >>= 1;
					m_PC += 2;
					break;

				case 0x0007: // 0x8XY7 Sets VX to VY minus VX 
					if (m_Registers[(m_OpCode & 0x00F0) >> 4] > m_Registers[(m_OpCode & 0x0F00) >> 8])
						m_Registers[0xF] = 0;
					else
						m_Registers[0xF] = 1;

					m_Registers[(m_OpCode & 0x0F00) >> 8] = m_Registers[(m_OpCode & 0x00F0) >> 4] - m_Registers[(m_OpCode & 0x0F00) >> 8];
					m_PC += 2;
					break;

				case 0x000E: // 0x8XYE Shifts VX by one
					m_Registers[0xF] = m_Registers[(m_OpCode & 0x0F00) >> 8] >> 7;
					m_Registers[(m_OpCode & 0x0F00) >> 8] <<= 1;
					m_PC += 2;
					break;

				default:
					printf("Unknown opcode [0x8000] : 0x%X m_PC %i\n", m_OpCode, m_PC);
			}
			break;

		case 0x9000: // 0x9XY0 Skips the next instruction if VX doesn't equal VY
			if (m_Registers[(m_OpCode & 0x0F00) >> 8] != m_Registers[(m_OpCode & 0x00F0) >> 4])
			{
				m_PC += 4;
				if (Debug)
					printf("Skipped to next Function\n");
			}
			else
			{
				m_PC += 2;
				if (Debug)
					printf("Didn't skipped the next Function\n");
			}
			break;

		case 0xA000: // 0xANNN Sets the Index Register to the Adress NNN
			if (Debug)
				printf("Setting the Index Register to address 0x%X\n", m_OpCode & 0x0FFF);

			m_IR = m_OpCode & 0x0FFF;
			m_PC += 2;
			break;

		case 0xB000: // 0xBNNN Jumps to the address NNN plus V0
			if (Debug)
				printf("Jumping to address 0x%X\n", (m_OpCode & 0x0FFF) + m_Registers[0]);

			m_PC = (m_OpCode & 0x0FFF) + m_Registers[0];
			break;

		case 0xC000: //0xCXNN Sets VX to a random number and NN
			if (Debug)
				printf("Random number to Register 0x%X\n", (m_OpCode & 0x0F00) >> 8);

			m_Registers[(m_OpCode & 0x0F00) >> 8] = (rand() % 0xFF) & (m_OpCode & 0x00FF);
			m_PC += 2;
			break;

		case 0xD000: //0xDXYN Draws a Sprite at coordinate (VX, VY) that has a width of 8 and a height of N
		{
			unsigned short x = m_Registers[(m_OpCode & 0x0F00) >> 8];
			unsigned short y = m_Registers[(m_OpCode & 0x00F0) >> 4];
			unsigned short height = m_OpCode & 0x000F;
			unsigned short pixel = 0;

			if (Debug)
				printf("Rendering a Sprite X: %i Y: %i\n", x, y);

			m_Registers[0xF] = 0;

			for (int yLine = 0; yLine < height; yLine++)
			{
				pixel = m_Memory[m_IR + yLine];
				for (int xLine = 0; xLine < 8; xLine++)
				{
					if ((pixel & (0x80 >> xLine)) != 0)
					{
						if (m_PixelData[(x + xLine + ((y + yLine) * 64))] == 1)
						{
							m_Registers[0xF] = 1;
						}
						m_PixelData[x + xLine + ((y + yLine) * 64)] ^= 1;
					}
				}
			}
			m_PC += 2;
		}
		break;

		case 0xE000:
			switch (m_OpCode & 0x00FF)
			{
				case 0x009E: // 0xEX9E Skips the next instruction if the key stored in VX is pressed
					if (m_Keys[m_Registers[(m_OpCode & 0x0F00) >> 8]] != 0)
					{
						m_PC += 4;
						if (Debug)
							printf("Skipped to next Function\n");
					}
					else
					{
						m_PC += 2;
						if (Debug)
							printf("Didn't skipped the next Function\n");
					}
					break;

				case 0x00A1: // 0xEX9E Skips the next instruction if the key stored in VX is pressed
					if (m_Keys[m_Registers[(m_OpCode & 0x0F00) >> 8]] == 0)
					{
						m_PC += 4;
						if (Debug)
							printf("Skipped to next Function\n");
					}
					else
					{
						m_PC += 2;
						if (Debug)
							printf("Didn't skipped the next Function\n");
					}
					break;
			}
		break;

		case 0xF000:
			switch (m_OpCode & 0x00FF)
			{
				case 0x0007: // 0xFX07 Sets VX to the value of the delay timer
					if (Debug)
						printf("Setting Register 0x%X to the Delay Timer %i\n", (m_OpCode & 0x0F00) >> 8, m_DelayTimer);

					m_Registers[(m_OpCode & 0x0F00) >> 8] = m_DelayTimer;
					m_PC += 2;
					break;

				case 0x000A: // 0xFX0A Wait for  a key press and then store it in VX
				{
					if (Debug)
						printf("Waiting for keyPress\n");

					bool keyPress = false;

					for (int i = 0; i < 16; i++)
					{
						if (m_Keys[i] != 0)
						{
							m_Registers[(m_OpCode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					if (!keyPress) // Retry
						return;

					m_PC += 2;					
				}
				break;

				case 0x0015: //Sets the delay timer to VX
					if (Debug)
						printf("Setting delay timer to register\n");

					m_DelayTimer = m_Registers[(m_OpCode & 0x0F00) >> 8];
					m_PC += 2;
					break;

				case 0x0018: //Sets the Sound timer to VX
					if (Debug)
						printf("Setting sound timer to register\n");

					m_SoundTimer = m_Registers[(m_OpCode & 0x0F00) >> 8];
					m_PC += 2;
					break;

				case 0x001E: //Adds VX to I
					if (Debug)
						printf("Adding Register to Index  Register\n");

					if (m_IR + m_Registers[(m_OpCode & 0x0F00) >> 8] > 0xFFF)
						m_Registers[0xF] = 1;
					else
						m_Registers[0xF] = 0;
					m_IR += m_Registers[(m_OpCode & 0x0F00) >> 8];
					m_PC += 2;
					break;

				case 0x0029: 
					m_IR = m_Registers[(m_OpCode & 0x0F00) >> 8] * 0x5;
					m_PC += 2;
					break;

				case 0x0033:
					m_Memory[m_IR] = m_Registers[(m_OpCode & 0x0F00) >> 8] / 100;
					m_Memory[m_IR + 1] = (m_Registers[(m_OpCode & 0x0F00) >> 8] / 10) % 10;
					m_Memory[m_IR + 2] = (m_Registers[(m_OpCode & 0x0F00) >> 8] % 100) % 10;
					m_PC += 2;
					break;

				case 0x0055:
					for (int i = 0; i <= ((m_OpCode & 0x0F00) >> 8); i++)
						m_Memory[m_IR + i] = m_Registers[i];

					m_IR += ((m_OpCode & 0x0F00) >> 8) + 1;
					m_PC += 2;
					break;

				case 0x0065:
					for (int i = 0; i <= ((m_OpCode & 0x0F00) >> 8); i++)
						m_Registers[i] = m_Memory[m_IR + i];

					m_IR += ((m_OpCode & 0x0F00) >> 8) + 1;
					m_PC += 2;
					break;
				
				default:
					printf("Unknown opcode [0xF000]: 0x%X m_PC %i\n", m_OpCode, m_PC);
					break;

			}
			break;

		default:
			printf("Unknown opcode: 0x%X m_PC %i\n", m_OpCode, m_PC);
			break;
	}

	if (m_DelayTimer > 0)
		m_DelayTimer--;

	if (m_SoundTimer > 0)
	{
		if (m_SoundTimer == 1)
			printf("SOUND\n");
		m_SoundTimer--;
	}
}