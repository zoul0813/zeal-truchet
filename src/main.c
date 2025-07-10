#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <zvb_hardware.h>
#include <zos_sys.h>
#include <zos_time.h>


#define SCREEN_FONT_SIZE    3072
#define SCREEN_WIDTH        80
#define SCREEN_HEIGHT       40

uint8_t PATTERN_A[] = { 0x2, };
uint8_t PATTERN_B[] = {
    2, 2,
    4, 4
};
uint8_t PATTERN_C[] = {
    2, 4,
    4, 2
};
uint8_t PATTERN_D[] = {
    3, 2,
    4, 1,
};
uint8_t PATTERN_E[] = {
    3, 4, 1, 2,
    4, 3, 2, 1,
    1, 2, 3, 4,
    2, 1, 4, 3,
};
uint8_t PATTERN_F[] = {
    3, 4,
    2, 1,
};


typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t *tiles;
} truchet_pattern_t;

truchet_pattern_t PATTERNS[] = {
    {
        .width = 1,
        .height = 1,
        .tiles = PATTERN_A,
    },
    {
        .width = 2,
        .height = 2,
        .tiles = PATTERN_B,
    },
    {
        .width = 2,
        .height = 2,
        .tiles = PATTERN_C,
    },
    {
        .width = 2,
        .height = 2,
        .tiles = PATTERN_D,
    },
    {
        .width = 4,
        .height = 4,
        .tiles = PATTERN_E,
    },
    {
        .width = 2,
        .height = 2,
        .tiles = PATTERN_F,
    },
};

int fflush_stdout(void); // zeal8bitos.asm

const uint8_t CHARS[4][12] = {
    {
        0b11111111,
        0b11111110,
        0b11111110,
        0b11111100,
        0b11111000,
        0b11111000,
        0b11110000,
        0b11110000,
        0b11100000,
        0b11000000,
        0b11000000,
        0b10000000,
    },
    {
        0b11111111,
        0b01111111,
        0b01111111,
        0b00111111,
        0b00011111,
        0b00011111,
        0b00001111,
        0b00001111,
        0b00000111,
        0b00000011,
        0b00000011,
        0b00000001,
    },
    {
        0b00000001,
        0b00000011,
        0b00000011,
        0b00000111,
        0b00001111,
        0b00001111,
        0b00011111,
        0b00011111,
        0b00111111,
        0b01111111,
        0b01111111,
        0b11111111,
    },
    {
        0b10000000,
        0b11000000,
        0b11000000,
        0b11100000,
        0b11110000,
        0b11110000,
        0b11111000,
        0b11111000,
        0b11111100,
        0b11111110,
        0b11111110,
        0b11111111,
    },
};

#define PATTERNS_LENGTH (sizeof(PATTERNS) / sizeof(PATTERNS[0]))

const __sfr __banked __at(0xF2) mmu_page2_ro;
__sfr __at(0xF2) mmu_page2;
uint8_t mmu_page_current;

uint8_t __at(0x8000) TEXT[SCREEN_HEIGHT][SCREEN_WIDTH];
uint8_t __at(0x9000) COLOR[SCREEN_HEIGHT][SCREEN_WIDTH];
uint8_t __at(0xB000) FONT[SCREEN_FONT_SIZE]; // TODO: [256][12] ???

uint8_t font_backup[4 * 12] = { 0 };

inline void text_map_vram(void)
{
    mmu_page_current = mmu_page2_ro;
    mmu_page2        = VID_MEM_PHYS_ADDR_START >> 14;
}

inline void text_demap_vram(void)
{
    mmu_page2 = mmu_page_current;
}

void draw_tile(truchet_pattern_t* pattern, uint8_t px, uint8_t py) {
    uint8_t pw = pattern->width;
    uint8_t ph = pattern->height;

    text_map_vram();
    for(uint8_t y = 0; y < ph; y++) {
        for(uint8_t x = 0; x < pw; x++) {
            uint16_t offset = (y * pw) + x;
            uint16_t index = 0;
            TEXT[py + y][px + x] = pattern->tiles[offset];
            COLOR[py + y][px + x] = 0x0F;
        }
    }
    text_demap_vram();
}

void draw_pattern(truchet_pattern_t* pattern) {
    uint8_t pw = pattern->width;
    uint8_t ph = pattern->height;
    for(uint8_t y = 0; y < SCREEN_HEIGHT; y+=ph) {
        for(uint8_t x = 0; x < SCREEN_WIDTH; x+=pw) {
            draw_tile(pattern, x, y);
        }
    }
}

int main(void)
{
    text_map_vram();
    // backup existing font
    memcpy(font_backup, &FONT[12], 4 * 12);
    // set new font chars
    memcpy(&FONT[12], CHARS, sizeof(CHARS));
    memset(TEXT, 0, 80 * 40);
    memset(COLOR, 0x0F, 80 * 40);

    zvb_peri_text_scroll_y = 0;
    zvb_peri_text_scroll_x = 0;

    uint8_t text_ctrl = zvb_peri_text_ctrl;
    zvb_peri_text_ctrl = 0;

    uint8_t cursor_time = zvb_peri_text_curs_time;
    zvb_peri_text_curs_time = 0;

    zvb_peri_text_curs_y = 0;
    zvb_peri_text_curs_x = 0;
    text_demap_vram();

    for(uint8_t p = 0; p < PATTERNS_LENGTH; p++) {
        truchet_pattern_t *pattern = &PATTERNS[p];
        draw_pattern(pattern);
        msleep(1500);
    }
    // draw_pattern(&PATTERNS[5]);

    // restore font backup
    text_map_vram();
    // memcpy(FONT, font_backup, 4 * 12);
    zvb_peri_text_ctrl = text_ctrl;
    zvb_peri_text_curs_time = cursor_time;
    zvb_peri_text_curs_y = SCREEN_HEIGHT - 1;
    zvb_peri_text_curs_x = 0;
    text_demap_vram();

    // printf("patterns: %d\n", PATTERNS_LENGTH);
    return 0;
}
