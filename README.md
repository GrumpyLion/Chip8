This is an working Chip8 Emulator. 

![alt tag](https://scontent-frt3-1.xx.fbcdn.net/v/t1.0-0/p370x247/17309431_209298139550993_4879150629076074612_n.jpg?oh=b69b0e34a1a87b92839ca37d1e96158a&oe=595FEA1D)

I used help from this Tutorial
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

You will need to find the Roms yourself.

Run Chip8.exe with the Rom name per command line.
The Rom needs to be next to the exe.

e.g
"Chip8.exe Tetris"  or if it has an extension "Chip8.exe Tetris.c8"

If you're building it yourself set Visual Studio to Release Mode and x64 or you will be missing the GLFW Includes.

Using Libraries:
- GLFW 3

Tested on Windows 10 with the following Games:
- Space Invaders
- Brix
- Tanks
- Kaleid 
- Pong
- Pong2
- Tetris
