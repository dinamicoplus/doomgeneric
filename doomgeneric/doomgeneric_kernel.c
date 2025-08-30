//doomgeneric for a bare Linux VirtualTerminal
// Copyright (C) 2025 Techflash
// Based on doomgeneric_sdl.c

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
#include "i_system.h"


#include <stdio.h>

#include <stdbool.h>
#include <drivers/video_vga13.h>
#include <drivers/pit.h>
#include <kernel/system.h>

uint8_t color_wheel = 0;
uint32_t start_ticks;

void DG_Init() {
	vga_set_mode13();
    vga13_build_palette();
	vga13_clear(0);
    start_ticks = pit_ticks;
}

void DG_DrawFrame(void) {

    // vga13_clear(color_wheel);
    // color_wheel = (color_wheel + 1) & 0xFF;
    const uint32_t *src = (const uint32_t *)DG_ScreenBuffer;
    volatile uint8_t *dst = VGA13_FB;
    for (int y = 0; y < VGA13_H; ++y) {
        const uint32_t *s0 = src + (y*4) * DOOMGENERIC_RESX;
        const uint32_t *s1 = s0  + DOOMGENERIC_RESX;
        volatile uint8_t *d  = dst + y * VGA13_W;
        for (int x = 0; x < VGA13_W; ++x) {
            uint32_t c00 = s0[x*2], c01 = s0[x*2+1];
            uint32_t c10 = s1[x*2], c11 = s1[x*2+1];
            // Promedio rápido por canal (sin overflow visible)
            uint32_t r = ((c00>>16)&0xFF) + ((c01>>16)&0xFF) + ((c10>>16)&0xFF) + ((c11>>16)&0xFF);
            uint32_t g = ((c00>> 8)&0xFF) + ((c01>> 8)&0xFF) + ((c10>> 8)&0xFF) + ((c11>> 8)&0xFF);
            uint32_t b = ((c00    )&0xFF) + ((c01    )&0xFF) + ((c10    )&0xFF) + ((c11    )&0xFF);
            uint32_t avg = (0xFFu<<24) | (((r+2)>>2)<<16) | (((g+2)>>2)<<8) | ((b+2)>>2);
            d[x] = rgb32_to_index(avg); // o la versión con dithering
        }
    }
}


void DG_SleepMs(uint32_t ms) {
    // pit_ticks is incremented in the PIT interrupt handler 100 times per second
    // so we divide ms by 10 to get the number of ticks to wait
    uint32_t target_ticks = pit_ticks + (ms / 10);
    while (pit_ticks < target_ticks) {
        halt_cpu();
    }

}

uint32_t DG_GetTicksMs() {
    // pit_ticks is incremented in the PIT interrupt handler 100 times per second
    // so we multiply by 10 to get milliseconds
	return (pit_ticks - start_ticks) * 10;
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
