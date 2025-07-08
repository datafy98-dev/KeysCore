#ifndef LCD_ST7735_H
#define LCD_ST7735_H

#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

// Разрешение
#define ST7735_WIDTH  162
#define ST7735_HEIGHT 132

// Пины
#define LCD_MOSI_PIN    11
#define LCD_SCLK_PIN    12
#define LCD_CS_PIN      10
#define LCD_DC_PIN      13
#define LCD_RST_PIN     14

// Команды
#define ST7735_NOP      0x00
#define ST7735_SWRESET  0x01
#define ST7735_RDDID    0x04
#define ST7735_RDDST    0x09
#define ST7735_SLPIN    0x10
#define ST7735_SLPOUT   0x11
#define ST7735_PTLON    0x12
#define ST7735_NORON    0x13
#define ST7735_INVOFF   0x20
#define ST7735_INVON    0x21
#define ST7735_DISPOFF  0x28
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_RAMRD    0x2E
#define ST7735_PTLAR    0x30
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36
#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_DISSET5  0xB6
#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5
#define ST7735_RDID1    0xDA
#define ST7735_RDID2    0xDB
#define ST7735_RDID3    0xDC
#define ST7735_RDID4    0xDD
#define ST7735_PWCTR6   0xFC
#define ST7735_GMCTRP1  0xE0
#define ST7735_GMCTRN1  0xE1

// Цвета (RGB565)
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_ORANGE    0xFD20
#define COLOR_PURPLE    0x8010
#define COLOR_LIME      0x07FF
#define COLOR_PINK      0xF81F
#define COLOR_GRAY      0x8410
#define COLOR_BROWN     0xA145

// Типы шрифтов
typedef enum {
    FONT_DEFAULT = 0,
    FONT_MONO = 1,
    FONT_ELEGANT = 2
} font_type_t;

typedef struct {
    spi_device_handle_t spi;
    int dc_pin;
    int rst_pin;
    int cs_pin;
    font_type_t current_font;
} st7735_t;

// Основные функции
esp_err_t st7735_init(st7735_t *lcd);
void st7735_write_command(st7735_t *lcd, uint8_t cmd);
void st7735_write_data(st7735_t *lcd, uint8_t data);
void st7735_write_data_16(st7735_t *lcd, uint16_t data);
void st7735_set_addr_window(st7735_t *lcd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// Оптимизированные функции рисования
void st7735_fill_screen(st7735_t *lcd, uint16_t color);
void st7735_fill_screen_fast(st7735_t *lcd, uint16_t color);
void st7735_draw_pixel(st7735_t *lcd, uint8_t x, uint8_t y, uint16_t color);
void st7735_draw_line(st7735_t *lcd, int x0, int y0, int x1, int y1, uint16_t color);
void st7735_draw_rect(st7735_t *lcd, int x, int y, int w, int h, uint16_t color);
void st7735_fill_rect(st7735_t *lcd, int x, int y, int w, int h, uint16_t color);
void st7735_draw_circle(st7735_t *lcd, int x0, int y0, int r, uint16_t color);
void st7735_fill_circle(st7735_t *lcd, int x0, int y0, int r, uint16_t color);
void st7735_draw_mono_image(st7735_t *lcd, uint8_t x, uint8_t y, const uint8_t *image_data, uint8_t width, uint8_t height, uint16_t color, uint16_t bg_color);

// Функции работы с текстом
void st7735_set_font(st7735_t *lcd, font_type_t font);
void st7735_draw_char(st7735_t *lcd, uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg_color);
void st7735_draw_char_with_font(st7735_t *lcd, uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg_color, font_type_t font);
void st7735_draw_string(st7735_t *lcd, uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg_color);
void st7735_draw_string_with_font(st7735_t *lcd, uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg_color, font_type_t font);

// Утилиты
uint16_t st7735_color565(uint8_t r, uint8_t g, uint8_t b);
void st7735_invert_display(st7735_t *lcd, bool invert);
void st7735_set_rotation(st7735_t *lcd, uint8_t rotation);

#endif
