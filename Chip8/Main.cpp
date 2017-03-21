#include <GLFW\glfw3.h>
#include <cstdio>
#include <string>
#include <sstream>
#include "CPU.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int modifier = 20;

int displayWidth = SCREEN_WIDTH * modifier;
int displayHeight = SCREEN_HEIGHT * modifier;

__int8 screenData[SCREEN_HEIGHT][SCREEN_WIDTH][3];

GLFWwindow* window;
CPU cpu;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		exit(0);

	if (key == GLFW_KEY_1)
	{
		if(action == GLFW_PRESS)
			cpu.m_Keys[0x1] = 1;
		else
			cpu.m_Keys[0x1] = 0;
	}

	if (key == GLFW_KEY_2)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x2] = 1;
		else
			cpu.m_Keys[0x2] = 0;
	}

	if (key == GLFW_KEY_3)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x3] = 1;
		else
			cpu.m_Keys[0x3] = 0;
	}

	if (key == GLFW_KEY_4)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0xC] = 1;
		else
			cpu.m_Keys[0xC] = 0;
	}

	if (key == GLFW_KEY_Q)
	{
		if (action != GLFW_RELEASE)
		{
			cpu.m_Keys[0x4] = 1;
		}
		else
		{
			cpu.m_Keys[0x4] = 0;
		}
	}

	if (key == GLFW_KEY_W)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x5] = 1;
		else
			cpu.m_Keys[0x5] = 0;
	}

	if (key == GLFW_KEY_E)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x6] = 1;
		else
			cpu.m_Keys[0x6] = 0;
	}

	if (key == GLFW_KEY_R)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0xD] = 1;
		else
			cpu.m_Keys[0xD] = 0;
	}

	if (key == GLFW_KEY_A)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x7] = 1;
		else
			cpu.m_Keys[0x7] = 0;
	}

	if (key == GLFW_KEY_S)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x8] = 1;
		else
			cpu.m_Keys[0x8] = 0;
	}

	if (key == GLFW_KEY_D)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x9] = 1;
		else
			cpu.m_Keys[0x9] = 0;
	}

	if (key == GLFW_KEY_F)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0xE] = 1;
		else
			cpu.m_Keys[0xE] = 0;
	}

	if (key == GLFW_KEY_Z)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0xA] = 1;
		else
			cpu.m_Keys[0xA] = 0;
	}

	if (key == GLFW_KEY_X)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0x0] = 1;
		else
			cpu.m_Keys[0x0] = 0;
	}

	if (key == GLFW_KEY_C)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0xB] = 1;
		else
			cpu.m_Keys[0xB] = 0;
	}

	if (key == GLFW_KEY_V)
	{
		if (action != GLFW_RELEASE)
			cpu.m_Keys[0xF] = 1;
		else
			cpu.m_Keys[0xF] = 0;
	}
}

void SetupTexture()
{
	// Clear screen
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
		for (int x = 0; x < SCREEN_WIDTH; ++x)
			screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;

	// Create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

	// Set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Enable textures
	glEnable(GL_TEXTURE_2D);
}

void UpdateTexture(const CPU& c8)
{
	// Update pixels
	for (int y = 0; y < 32; ++y)
		for (int x = 0; x < 64; ++x)
			if (c8.m_PixelData[(y * 64) + x] == 0)
				screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;	// Disabled
			else
				screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255;  // Enabled

																						// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);		glVertex2d(0.0, displayHeight);
	glTexCoord2d(1.0, 0.0); 	glVertex2d(displayWidth, displayHeight);
	glTexCoord2d(1.0, 1.0); 	glVertex2d(displayWidth, 0.0);
	glTexCoord2d(0.0, 1.0); 	glVertex2d(0.0, 0.0);
	glEnd();
}

bool InitWindow()
{
	if (!glfwInit())
		return false;

	std::string title = static_cast<std::ostringstream*>(&(std::ostringstream() << "CHIP-8 Emulator Playing " << cpu.FileName))->str();

	window = glfwCreateWindow(displayWidth, displayHeight, title.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, keyCallback);

	glfwMakeContextCurrent(window);

	return true;
}

int main(int argc, char*argv [])
{
	if (argc < 2)
	{
		printf("Usage Error Type Chip8.exe 'Romname'\n");
		exit(0);
	}

	cpu = CPU();
	
	if (!cpu.LoadRom(argv[1]))
		return -1;

	if (!InitWindow())
		return -1;

	SetupTexture();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, displayWidth, 0.0, displayHeight, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glViewport(0, 0, displayWidth, displayHeight);

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		//Inaccurate
		for(int i = 0; i < 5; i++)
		{
			cpu.Emulate();
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		UpdateTexture(cpu);		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}