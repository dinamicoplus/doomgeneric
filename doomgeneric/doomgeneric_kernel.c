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
#include <string.h>
#include "i_video.h"

#define R(x) (((x)>> 16)&0xFF)
#define G(x) (((x)>>  8)&0xFF)
#define B(x) (((x)>>  0)&0xFF)

uint8_t color_wheel = 0;
uint32_t start_ticks;

void DG_Init() {
	vga_set_mode13();
    //vga_init_text_mode();
	vga13_clear(0);
    start_ticks = pit_ticks;
}

typedef struct
{
	byte r;
	byte g;
	byte b;
} col_t;

int getPaletteIndex (int r, int g, int b)
{
    int best, best_diff, diff;
    int i;
    col_t color;

    best = 0;
    best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
    	color.r = colors[i].r;
    	color.g = colors[i].g;
    	color.b = colors[i].b;

        diff = (r - color.r) * (r - color.r)
             + (g - color.g) * (g - color.g)
             + (b - color.b) * (b - color.b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

void DG_DrawFrame(void) {
    //while(1);
    // color_wheel = (color_wheel + 1) & 0xFF;
    const uint32_t *src = (const uint32_t *)DG_ScreenBuffer;
    volatile uint8_t *dst = VGA13_FB;
    for (int y = 0; y < VGA13_H; ++y) {
        const uint32_t *s = src + (y*4) * DOOMGENERIC_RESX;
        volatile uint8_t *d  = dst + y * VGA13_W;
        for (int x = 0; x < VGA13_W; ++x) {
            //d[x] = rgb32_to_enhanced_index(s[DOOMGENERIC_RESX/VGA13_W*x]); // o la versión con dithering
            d[x] = getPaletteIndex(R(s[DOOMGENERIC_RESX/VGA13_W*x]), G(s[DOOMGENERIC_RESX/VGA13_W*x]), B(s[DOOMGENERIC_RESX/VGA13_W*x]));
        }
    }
    handleKeyInput();

    // Update palette
    if(palette_changed) {
        for(int i = 0; i < 256; i++) {
            uint8_t r = colors[i].r >> 2;
            uint8_t g = colors[i].g >> 2;
            uint8_t b = colors[i].b >> 2;
            vga13_set_palette(i, r, g, b);
        }
        palette_changed = false;
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
