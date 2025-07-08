#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lcd_st7735.h"
#include "bk.h"
#include "buttons.h"

static const char *TAG = "MAIN";
static st7735_t *g_lcd = NULL;
static int g_numb = 1; // Правильное объявление переменной

// Предварительное объявление функции
void draw_main_menu(const char *menu_names[], size_t menu_size);

// Обработчик кнопки UP (увеличивает номер пункта меню)
void buttonUP_handler(button_event_t event) {
    if (g_lcd == NULL) return;

    switch (event) {
        case BUTTON_PRESSED:
            g_numb -= 1;
            draw_main_menu((const char *[]){"something1", "something2", "something3"}, 3);
            break;
        case BUTTON_RELEASED:
            break;
        case BUTTON_LONG_PRESS:
            break;
    }
}

// Обработчик кнопки DOWN (уменьшает номер пункта меню)
void buttonDOWN_handler(button_event_t event) {
    if (g_lcd == NULL) return;

    switch (event) {
        case BUTTON_PRESSED:
            g_numb += 1;
            draw_main_menu((const char *[]){"something1", "something2", "something3"}, 3);
            break;
        case BUTTON_RELEASED:
            break;
        case BUTTON_LONG_PRESS:
            break;
    }
}

// Обработчик кнопки SELECT (подтверждение выбора)
void buttonSELECT_handler(button_event_t event) {
    if (g_lcd == NULL) return;

    switch (event) {
        case BUTTON_PRESSED:
            st7735_fill_screen(g_lcd, COLOR_RED);
            st7735_draw_string(g_lcd, 40, 60, ((const char *[]){"something1", "something2", "something3"})[g_numb - 1],
                               COLOR_BLACK, COLOR_RED);
            vTaskDelay(pdMS_TO_TICKS(1000));
            draw_main_menu((const char *[]){"something1", "something2", "something3"}, 3);
            break;
        case BUTTON_RELEASED:
            break;
        case BUTTON_LONG_PRESS:
            break;
    }
}

// Функция отрисовки меню

void draw_main_menu(const char *menu_names[], size_t menu_size) {
    if (g_lcd == NULL) return;

    st7735_fill_screen(g_lcd, COLOR_BLACK);
    st7735_draw_mono_image(g_lcd, 0, 0, bk_image, BK_IMAGE_WIDTH, BK_IMAGE_HEIGHT, COLOR_WHITE, COLOR_BLACK);

    st7735_set_font(g_lcd, FONT_ELEGANT);
    st7735_draw_string(g_lcd, 5, 5, "ULTRA MAIN MENU", COLOR_RED, COLOR_BLACK);

    st7735_set_font(g_lcd, FONT_MONO);

    for(int i = 0; i < menu_size; i++) {
        if ((i + 1) != g_numb) {
            st7735_draw_string(g_lcd, 5, 20 + (i * 15), menu_names[i], COLOR_WHITE, COLOR_BLACK);
        } else {
            st7735_draw_string(g_lcd, 10, 20 + (i * 15), menu_names[i], COLOR_YELLOW, COLOR_BLACK);
        }
    }
}

// Основная функция
void app_main(void) {
    ESP_LOGI(TAG, "Starting ST7735");

    st7735_t lcd;
    button_t my_button_up, my_button_down, my_button_select;
    const char *menu_names[] = {"something1", "something2", "something3"};
    size_t menu_size = sizeof(menu_names) / sizeof(menu_names[0]);

    esp_err_t ret = st7735_init(&lcd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LCD: %s", esp_err_to_name(ret));
        return;
    }

    g_lcd = &lcd;

    st7735_fill_screen(&lcd, COLOR_BLACK);
    draw_main_menu(menu_names, menu_size);

    // Инициализация кнопок
    button_init(&my_button_up, GPIO_NUM_1, true, buttonUP_handler);     // UP
    button_init(&my_button_down, GPIO_NUM_42, true, buttonDOWN_handler); // DOWN
    button_init(&my_button_select, GPIO_NUM_2, true, buttonSELECT_handler); // SELECT

    button_set_debounce(&my_button_up, 30);
    button_set_debounce(&my_button_down, 30);
    button_set_debounce(&my_button_select, 30);

    button_set_long_press(&my_button_up, 1000);
    button_set_long_press(&my_button_down, 1000);
    button_set_long_press(&my_button_select, 1000);

    while (1) {
        button_process(&my_button_up);
        button_process(&my_button_down);
        button_process(&my_button_select);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}