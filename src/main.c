#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <zvb_hardware.h>

#define SCREEN_COL80_WIDTH  80
#define SCREEN_COL80_HEIGHT 40
#define SCREEN_COL40_WIDTH  40
#define SCREEN_COL40_HEIGHT 20

#define SCREEN_FONT_SIZE    3072

int fflush_stdout(void); // zeal8bitos.asm

const uint8_t CHARS[4][12] = {
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
};

#define PATTERN_LENGTH 4
const uint8_t PATTERNS[][PATTERN_LENGTH] = {
    { 2, 2, 2, 2 },
    { 3, 2, 3, 2 },
    { 3, 2, 1, 4 },
};

#define PATTERNS_LENGTH (sizeof(PATTERNS) / sizeof(PATTERNS[0]))

const __sfr __banked __at(0xF2) mmu_page2_ro;
__sfr __at(0xF2) mmu_page2;
uint8_t mmu_page_current;

uint8_t __at(0x8000) TEXT[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];
uint8_t __at(0x9000) COLOR[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];
uint8_t __at(0xB000) FONT[SCREEN_FONT_SIZE];

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

int main(void)
{
    text_map_vram();
    // backup existing font
    memcpy(font_backup, &FONT[12], 4 * 12);
    // set new font chars
    memcpy(&FONT[12], CHARS, sizeof(CHARS));
    text_demap_vram();

    for(uint8_t p = 0; p < PATTERNS_LENGTH; p++) {
        const uint8_t *test = PATTERNS[p];
        for(uint8_t y = 0; y < 8; y++) {
            for(uint8_t x = 0; x < SCREEN_COL80_WIDTH / PATTERN_LENGTH; x++) {
                for(uint8_t c = 0; c < PATTERN_LENGTH; c++) {
                    putchar(test[c]);
                }
            }
        }
        putchar('\n');
        putchar('\n');
        fflush_stdout();
    }

    // restore font backup
    // text_map_vram();
    // memcpy(FONT, font_backup, 4 * 12);
    // text_demap_vram();

    // printf("patterns: %d\n", PATTERNS_LENGTH);
    return 0;
}
