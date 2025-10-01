#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

// --- Pinos ---
#define LED_ONE     15
#define LED_TWO     16
#define LED_THREE   17
#define LED_FOUR    18
#define BUZZER_PIN  14

// --- PWM Config ---
#define LEDC_FREQUENCY       500       // Hz
#define LEDC_RESOLUTION      LEDC_TIMER_8_BIT
#define LEDC_MODE            LEDC_LOW_SPEED_MODE

#define LEDC_TIMER           LEDC_TIMER_0
#define LEDC_TIMER_BUZZER    LEDC_TIMER_1

#define LEDC_CHANNEL_ONE     LEDC_CHANNEL_0
#define LEDC_CHANNEL_TWO     LEDC_CHANNEL_1
#define LEDC_CHANNEL_THREE   LEDC_CHANNEL_2
#define LEDC_CHANNEL_FOUR    LEDC_CHANNEL_3
#define LEDC_CHANNEL_BUZZER  LEDC_CHANNEL_4

// --- Prototipos de funções ---
void config_pwm(int pin, int timer, int channel, int duty);
void fase_um();
void fase_dois();
void fase_tres();
void execute_fade(int led_channel);

void app_main() {
    while (1) {
        fase_um();
        fase_dois();
        fase_tres();
    }
}

// --- Funções de fase ---
void fase_um() {
    /* Fase 1: Fading sincronizado de todos os LEDs */
    config_pwm(LED_ONE, LEDC_TIMER, LEDC_CHANNEL_ONE, 0);
    config_pwm(LED_TWO, LEDC_TIMER, LEDC_CHANNEL_TWO, 0);
    config_pwm(LED_THREE, LEDC_TIMER, LEDC_CHANNEL_THREE, 0);
    config_pwm(LED_FOUR, LEDC_TIMER, LEDC_CHANNEL_FOUR, 0);

    // Aumenta de 0 → 255
    for (int duty = 0; duty <= 255; duty++) {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_ONE, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_ONE);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_TWO, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_TWO);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_THREE, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_THREE);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_FOUR, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_FOUR);

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Diminui de 255 → 0
    for (int duty = 255; duty >= 0; duty--) {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_ONE, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_ONE);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_TWO, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_TWO);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_THREE, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_THREE);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_FOUR, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_FOUR);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void fase_dois() {
    /* Fase 2: Fading sequencial dos LEDs */
    config_pwm(LED_ONE, LEDC_TIMER, LEDC_CHANNEL_ONE, 0);
    config_pwm(LED_TWO, LEDC_TIMER, LEDC_CHANNEL_TWO, 0);
    config_pwm(LED_THREE, LEDC_TIMER, LEDC_CHANNEL_THREE, 0);
    config_pwm(LED_FOUR, LEDC_TIMER, LEDC_CHANNEL_FOUR, 0);

    // Sequência de fade
    execute_fade(LEDC_CHANNEL_ONE);
    execute_fade(LEDC_CHANNEL_TWO);
    execute_fade(LEDC_CHANNEL_THREE);
    execute_fade(LEDC_CHANNEL_FOUR);
    execute_fade(LEDC_CHANNEL_THREE);
    execute_fade(LEDC_CHANNEL_TWO);
    execute_fade(LEDC_CHANNEL_ONE);
}

void fase_tres() {
    /* Fase 3: Teste sonoro com o buzzer */
    config_pwm(BUZZER_PIN, LEDC_TIMER_BUZZER, LEDC_CHANNEL_BUZZER, 128);

    // Frequência sobe de 500 → 2000 Hz
    for (int freq = 500; freq <= 2000; freq += 2) {
        ledc_set_freq(LEDC_MODE, LEDC_TIMER_BUZZER, freq);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Frequência desce de 2000 → 500 Hz
    for (int freq = 2000; freq >= 500; freq -= 2) {
        ledc_set_freq(LEDC_MODE, LEDC_TIMER_BUZZER, freq);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    ledc_set_freq(LEDC_MODE, LEDC_TIMER_BUZZER, 0);
}

// --- Funções auxiliares ---
void execute_fade(int led_channel) {
    // Aumenta de 0 → 255
    for (int duty = 0; duty <= 255; duty++) {
        ledc_set_duty(LEDC_MODE, led_channel, duty);
        ledc_update_duty(LEDC_MODE, led_channel);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    // Diminui de 255 → 0
    for (int duty = 255; duty >= 0; duty--) {
        ledc_set_duty(LEDC_MODE, led_channel, duty);
        ledc_update_duty(LEDC_MODE, led_channel);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void config_pwm(int custom_pin, int custom_timer, int custom_channel, int duty) {
    // Configuração do Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode      = LEDC_MODE,
        .timer_num       = custom_timer,
        .duty_resolution = LEDC_RESOLUTION,
        .freq_hz         = LEDC_FREQUENCY,
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Configuração do Canal
    ledc_channel_config_t ledc_channel = {
        .gpio_num   = custom_pin,
        .speed_mode = LEDC_MODE,
        .channel    = custom_channel,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = custom_timer,
        .duty       = duty,
        .hpoint     = 0
    };
    ledc_channel_config(&ledc_channel);
}
