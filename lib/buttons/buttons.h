#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum {
    BUTTON_PRESSED,
    BUTTON_RELEASED,
    BUTTON_LONG_PRESS
} button_event_t;

typedef void (*button_callback_t)(button_event_t event);

typedef struct {
    gpio_num_t pin;
    bool active_low;
    uint32_t debounce_ms;
    uint32_t long_press_ms;
    button_callback_t callback;
    
    // Внутренние переменные
    bool last_state;
    uint32_t press_time;
    bool long_press_triggered;
} button_t;

// Инициализация кнопки
void button_init(button_t* btn, gpio_num_t pin, bool active_low, button_callback_t callback);

// Установка времени антидребезга (по умолчанию 50мс)
void button_set_debounce(button_t* btn, uint32_t debounce_ms);

// Установка времени длинного нажатия (по умолчанию 1000мс)
void button_set_long_press(button_t* btn, uint32_t long_press_ms);

// Обработка кнопки (вызывать в цикле или таймере)
void button_process(button_t* btn);

#endif
