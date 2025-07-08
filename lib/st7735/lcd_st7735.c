#include "lcd_st7735.h"
#include "font1.h"
#include "font_mono.h"
#include "font_elegant.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include <math.h>

static const char *TAG = "ST7735";

// Буфер для быстрой передачи данных
static uint8_t spi_buffer[ST7735_WIDTH * 2];

esp_err_t st7735_init(st7735_t *lcd) {
    esp_err_t ret;
    
    // GPIO
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LCD_DC_PIN) | (1ULL << LCD_RST_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
    
    lcd->dc_pin = LCD_DC_PIN;
    lcd->rst_pin = LCD_RST_PIN;
    lcd->cs_pin = LCD_CS_PIN;
    lcd->current_font = FONT_DEFAULT;
    
    // SPI
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = LCD_MOSI_PIN,
        .sclk_io_num = LCD_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = ST7735_WIDTH * ST7735_HEIGHT * 2,
    };
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40 * 1000 * 1000, // 40MHz
        .mode = 0,
        .spics_io_num = LCD_CS_PIN,
        .queue_size = 7,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };
    
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus");
        return ret;
    }
    
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &lcd->spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device");
        return ret;
    }
    
    // Сброс дисплея
    gpio_set_level(lcd->rst_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(lcd->rst_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Инициализация
    st7735_write_command(lcd, ST7735_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    
    st7735_write_command(lcd, ST7735_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    st7735_write_command(lcd, ST7735_FRMCTR1);
    st7735_write_data(lcd, 0x01);
    st7735_write_data(lcd, 0x2C);
    st7735_write_data(lcd, 0x2D);
    
    st7735_write_command(lcd, ST7735_FRMCTR2);
    st7735_write_data(lcd, 0x01);
    st7735_write_data(lcd, 0x2C);
    st7735_write_data(lcd, 0x2D);
    
    st7735_write_command(lcd, ST7735_FRMCTR3);
    st7735_write_data(lcd, 0x01);
    st7735_write_data(lcd, 0x2C);
    st7735_write_data(lcd, 0x2D);
    st7735_write_data(lcd, 0x01);
    st7735_write_data(lcd, 0x2C);
    st7735_write_data(lcd, 0x2D);

    st7735_write_command(lcd, ST7735_INVCTR);
    st7735_write_data(lcd, 0x07);
    
    st7735_write_command(lcd, ST7735_PWCTR1);
    st7735_write_data(lcd, 0xA2);
    st7735_write_data(lcd, 0x02);
    st7735_write_data(lcd, 0x84);
    
    st7735_write_command(lcd, ST7735_PWCTR2);
    st7735_write_data(lcd, 0xC5);
    
    st7735_write_command(lcd, ST7735_PWCTR3);
    st7735_write_data(lcd, 0x0A);
    st7735_write_data(lcd, 0x00);
    
    st7735_write_command(lcd, ST7735_PWCTR4);
    st7735_write_data(lcd, 0x8A);
    st7735_write_data(lcd, 0x2A);
    
    st7735_write_command(lcd, ST7735_PWCTR5);
    st7735_write_data(lcd, 0x8A);
    st7735_write_data(lcd, 0xEE);
    
    st7735_write_command(lcd, ST7735_VMCTR1);
    st7735_write_data(lcd, 0x0E);
    
    st7735_write_command(lcd, ST7735_MADCTL);
    st7735_write_data(lcd, 0xA0);
    
    st7735_write_command(lcd, ST7735_COLMOD);
    st7735_write_data(lcd, 0x05);
    
    st7735_write_command(lcd, ST7735_GMCTRP1);
    st7735_write_data(lcd, 0x02);
    st7735_write_data(lcd, 0x1c);
    st7735_write_data(lcd, 0x07);
    st7735_write_data(lcd, 0x12);
    st7735_write_data(lcd, 0x37);
    st7735_write_data(lcd, 0x32);
    st7735_write_data(lcd, 0x29);
    st7735_write_data(lcd, 0x2d);
    st7735_write_data(lcd, 0x29);
    st7735_write_data(lcd, 0x25);
    st7735_write_data(lcd, 0x2B);
    st7735_write_data(lcd, 0x39);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, 0x01);
    st7735_write_data(lcd, 0x03);
    st7735_write_data(lcd, 0x10);
    
    st7735_write_command(lcd, ST7735_GMCTRN1);
    st7735_write_data(lcd, 0x03);
    st7735_write_data(lcd, 0x1d);
    st7735_write_data(lcd, 0x07);
    st7735_write_data(lcd, 0x06);
    st7735_write_data(lcd, 0x2E);
    st7735_write_data(lcd, 0x2C);
    st7735_write_data(lcd, 0x29);
    st7735_write_data(lcd, 0x2D);
    st7735_write_data(lcd, 0x2E);
    st7735_write_data(lcd, 0x2E);
    st7735_write_data(lcd, 0x37);
    st7735_write_data(lcd, 0x3F);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, 0x02);
    st7735_write_data(lcd, 0x10);
    
    st7735_write_command(lcd, ST7735_NORON);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    st7735_write_command(lcd, ST7735_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    ESP_LOGI(TAG, "ST7735 initialized successfully");
    return ESP_OK;
}

void st7735_write_command(st7735_t *lcd, uint8_t cmd) {
    gpio_set_level(lcd->dc_pin, 0);
    
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    spi_device_polling_transmit(lcd->spi, &t);
}

void st7735_write_data(st7735_t *lcd, uint8_t data) {
    gpio_set_level(lcd->dc_pin, 1);
    
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
    };
    spi_device_polling_transmit(lcd->spi, &t);
}

void st7735_write_data_16(st7735_t *lcd, uint16_t data) {
    gpio_set_level(lcd->dc_pin, 1);
    
    uint8_t buffer[2] = {data >> 8, data & 0xFF};
    spi_transaction_t t = {
        .length = 16,
        .tx_buffer = buffer,
    };
    spi_device_polling_transmit(lcd->spi, &t);
}

void st7735_write_data_bulk(st7735_t *lcd, uint8_t *data, size_t len) {
    gpio_set_level(lcd->dc_pin, 1);
    
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_polling_transmit(lcd->spi, &t);
}

void st7735_set_addr_window(st7735_t *lcd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    st7735_write_command(lcd, ST7735_CASET);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, x0);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, x1);
    
    st7735_write_command(lcd, ST7735_RASET);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, y0);
    st7735_write_data(lcd, 0x00);
    st7735_write_data(lcd, y1);
    
    st7735_write_command(lcd, ST7735_RAMWR);
}

void st7735_fill_screen_fast(st7735_t *lcd, uint16_t color) {
    st7735_set_addr_window(lcd, 0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    for (int i = 0; i < ST7735_WIDTH * 2; i += 2) {
        spi_buffer[i] = color >> 8;
        spi_buffer[i + 1] = color & 0xFF;
    }
    
    gpio_set_level(lcd->dc_pin, 1);
    
    for (int y = 0; y < ST7735_HEIGHT; y++) {
        st7735_write_data_bulk(lcd, spi_buffer, ST7735_WIDTH * 2);
    }
}

void st7735_fill_screen(st7735_t *lcd, uint16_t color) {
    st7735_fill_screen_fast(lcd, color);
}

void st7735_draw_pixel(st7735_t *lcd, uint8_t x, uint8_t y, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    
    st7735_set_addr_window(lcd, x, y, x, y);
    st7735_write_data_16(lcd, color);
}

void st7735_draw_line(st7735_t *lcd, int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        st7735_draw_pixel(lcd, x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void st7735_draw_rect(st7735_t *lcd, int x, int y, int w, int h, uint16_t color) {
    st7735_draw_line(lcd, x, y, x + w - 1, y, color);
    st7735_draw_line(lcd, x + w - 1, y, x + w - 1, y + h - 1, color);
    st7735_draw_line(lcd, x + w - 1, y + h - 1, x, y + h - 1, color);
    st7735_draw_line(lcd, x, y + h - 1, x, y, color);
}

void st7735_fill_rect(st7735_t *lcd, int x, int y, int w, int h, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    if (x + w > ST7735_WIDTH) w = ST7735_WIDTH - x;
    if (y + h > ST7735_HEIGHT) h = ST7735_HEIGHT - y;
    
    st7735_set_addr_window(lcd, x, y, x + w - 1, y + h - 1);
    
    for (int i = 0; i < w * 2; i += 2) {
        spi_buffer[i] = color >> 8;
        spi_buffer[i + 1] = color & 0xFF;
    }
    
    gpio_set_level(lcd->dc_pin, 1);
    
    for (int row = 0; row < h; row++) {
        st7735_write_data_bulk(lcd, spi_buffer, w * 2);
    }
}

void st7735_draw_circle(st7735_t *lcd, int x0, int y0, int r, uint16_t color) {
    int x = r;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        st7735_draw_pixel(lcd, x0 + x, y0 + y, color);
        st7735_draw_pixel(lcd, x0 + y, y0 + x, color);
        st7735_draw_pixel(lcd, x0 - y, y0 + x, color);
        st7735_draw_pixel(lcd, x0 - x, y0 + y, color);
        st7735_draw_pixel(lcd, x0 - x, y0 - y, color);
        st7735_draw_pixel(lcd, x0 - y, y0 - x, color);
        st7735_draw_pixel(lcd, x0 + y, y0 - x, color);
        st7735_draw_pixel(lcd, x0 + x, y0 - y, color);
        
        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void st7735_fill_circle(st7735_t *lcd, int x0, int y0, int r, uint16_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                st7735_draw_pixel(lcd, x0 + x, y0 + y, color);
            }
        }
    }
}

void st7735_set_font(st7735_t *lcd, font_type_t font) {
    lcd->current_font = font;
}

const uint8_t* get_font_data(font_type_t font, char c) {
    if (c < 32 || c > 126) return NULL;
    
    switch (font) {
        case FONT_MONO:
            return font_mono_8x8[c - 32];
        case FONT_ELEGANT:
            return font_elegant_8x8[c - 32];
        case FONT_DEFAULT:
        default:
            return font_8x8[c - 32];
    }
}

void st7735_draw_char_with_font(st7735_t *lcd, uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg_color, font_type_t font) {
    if (c < 32 || c > 126) return;
    if (x + 8 > ST7735_WIDTH || y + 8 > ST7735_HEIGHT) return;
    
    const uint8_t *char_data = get_font_data(font, c);
    if (!char_data) return;
    
    st7735_set_addr_window(lcd, x, y, x + 7, y + 7);
    
    uint16_t pixel_data[64]; // 8x8 пикселей
    int idx = 0;
    
    for (int row = 0; row < 8; row++) {
        uint8_t line = char_data[row];
        for (int col = 0; col < 8; col++) {
            if (line & (0x01 << col)) {
                pixel_data[idx++] = color;
            } else {
                pixel_data[idx++] = bg_color;
            }
        }
    }
    
    // Отправляем данные одним блоком
    gpio_set_level(lcd->dc_pin, 1);
    spi_transaction_t t = {
        .length = 64 * 16, // 64 пикселя по 16 бит
        .tx_buffer = pixel_data,
    };
    spi_device_polling_transmit(lcd->spi, &t);
}

void st7735_draw_char(st7735_t *lcd, uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg_color) {
    st7735_draw_char_with_font(lcd, x, y, c, color, bg_color, lcd->current_font);
}

void st7735_draw_string_with_font(st7735_t *lcd, uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg_color, font_type_t font) {
    uint8_t cur_x = x;
    uint8_t cur_y = y;
    
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += 8;
        } else if (*str == '\r') {
            cur_x = x;
        } else if (*str == '\t') {
            cur_x = ((cur_x / 32) + 1) * 32;
        } else {
            if (cur_x + 8 > ST7735_WIDTH) {
                cur_x = x;
                cur_y += 8;
            }
            if (cur_y + 8 > ST7735_HEIGHT) break;
            
            st7735_draw_char_with_font(lcd, cur_x, cur_y, *str, color, bg_color, font);
            cur_x += 8;
        }
        str++;
    }
}

void st7735_draw_string(st7735_t *lcd, uint8_t x, uint8_t y, const char *str, uint16_t color, uint16_t bg_color) {
    st7735_draw_string_with_font(lcd, x, y, str, color, bg_color, lcd->current_font);
}

void st7735_draw_mono_image(st7735_t *lcd, uint8_t x, uint8_t y, const uint8_t *image_data, uint8_t width, uint8_t height, uint16_t color, uint16_t bg_color) {
    if (x + width > ST7735_WIDTH || y + height > ST7735_HEIGHT) return;
    
    st7735_set_addr_window(lcd, x, y, x + width - 1, y + height - 1);
    gpio_set_level(lcd->dc_pin, 1);
    
    uint16_t pixel_buffer[32]; // буфер
    int pixel_count = 0;
    
    for (int byte_idx = 0; byte_idx < (width * height) / 8; byte_idx++) {
        uint8_t byte_data = image_data[byte_idx];
        
        for (int bit = 0; bit < 8; bit++) {
            pixel_buffer[pixel_count++] = (byte_data & (0x80 >> bit)) ? color : bg_color;
            
            if (pixel_count == 32) {
                spi_transaction_t t = {
                    .length = 32 * 16, // 32 пикселя по 16 бит
                    .tx_buffer = pixel_buffer,
                };
                spi_device_polling_transmit(lcd->spi, &t);
                pixel_count = 0;
            }
        }
    }
    
    // Отправляем оставшиеся пиксели
    if (pixel_count > 0) {
        spi_transaction_t t = {
            .length = pixel_count * 16,
            .tx_buffer = pixel_buffer,
        };
        spi_device_polling_transmit(lcd->spi, &t);
    }
}


// RGB в RGB565
uint16_t st7735_color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Инверсия дисплея
void st7735_invert_display(st7735_t *lcd, bool invert) {
    st7735_write_command(lcd, invert ? ST7735_INVON : ST7735_INVOFF);
}

// Поворот дисплея
void st7735_set_rotation(st7735_t *lcd, uint8_t rotation) {
    st7735_write_command(lcd, ST7735_MADCTL);
    
    switch (rotation % 4) {
        case 0:
            st7735_write_data(lcd, 0xA0);
            break;
        case 1:
            st7735_write_data(lcd, 0x60);
            break;
        case 2:
            st7735_write_data(lcd, 0xC0);
            break;
        case 3:
            st7735_write_data(lcd, 0x00);
            break;
    }
}