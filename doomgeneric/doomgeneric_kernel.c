//doomgeneric for a bare Linux VirtualTerminal
// Copyright (C) 2025 Techflash
// Based on doomgeneric_sdl.c

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
#include "i_system.h"


#include <stdio.h>

#include <stdbool.h>


void DG_Init() {
	
}

void DG_DrawFrame() {

}

void DG_SleepMs(uint32_t ms) {

}

uint32_t DG_GetTicksMs() {
	return 0;
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
	return 0;
}

void DG_SetWindowTitle(const char * title) {
	printf("Window Title: %s\n", title);
}

int main(int argc, char **argv) {
	doomgeneric_Create(argc, argv);

	while (1)
	{
		doomgeneric_Tick();
	}


	return 0;
}
