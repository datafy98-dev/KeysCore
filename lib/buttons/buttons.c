#include "buttons.h"
#include "esp_timer.h"

void button_init(button_t* btn, gpio_num_t pin, bool active_low, button_callback_t callback) {
    btn->pin = pin;
    btn->active_low = active_low;
    btn->callback = callback;
    btn->debounce_ms = 50;
    btn->long_press_ms = 1000;
    btn->last_state = false;
    btn->press_time = 0;
    btn->long_press_triggered = false;
    
    // Настройка GPIO
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = active_low ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pull_down_en = active_low ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&config);
}

void button_set_debounce(button_t* btn, uint32_t debounce_ms) {
    btn->debounce_ms = debounce_ms;
}

void button_set_long_press(button_t* btn, uint32_t long_press_ms) {
    btn->long_press_ms = long_press_ms;
}

void button_process(button_t* btn) {
    bool current_state = gpio_get_level(btn->pin);
    if (btn->active_low) {
        current_state = !current_state;
    }
    
    uint32_t now = esp_timer_get_time() / 1000;
    
    // Обработка нажатия
    if (current_state && !btn->last_state) {
        btn->press_time = now;
        btn->long_press_triggered = false;
        if (btn->callback) {
            btn->callback(BUTTON_PRESSED);
        }
    }
    // Обработка отпускания
    else if (!current_state && btn->last_state) {
        if (now - btn->press_time >= btn->debounce_ms) {
            if (btn->callback) {
                btn->callback(BUTTON_RELEASED);
            }
        }
    }
    // Обработка длинного нажатия
    else if (current_state && btn->last_state) {
        if (!btn->long_press_triggered && 
            (now - btn->press_time >= btn->long_press_ms)) {
            btn->long_press_triggered = true;
            if (btn->callback) {
                btn->callback(BUTTON_LONG_PRESS);
            }
        }
    }
    
    btn->last_state = current_state;
}
