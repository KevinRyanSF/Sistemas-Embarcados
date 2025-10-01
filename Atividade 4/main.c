#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LED1_GPIO 2
#define LED2_GPIO 4
#define LED3_GPIO 5
#define LED4_GPIO 18
#define BUZZER_GPIO 19

#define LED_FREQ     1000   // 1kHz para LEDs
#define BUZZER_FREQ  1000   // inicial 1kHz
#define LEDC_RES     LEDC_TIMER_13_BIT
#define DELAY_MS     2000   // tempo base

// Função para configurar um canal LEDC
void config_ledc_channel(int gpio, ledc_channel_t channel, ledc_timer_t timer) {
    ledc_channel_config_t ch = {
        .gpio_num   = gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = channel,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = timer,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch);
}

void app_main(void) {
    // Configura TIMER para LEDs
    ledc_timer_config_t led_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_RES,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = LED_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&led_timer);

    // Configura TIMER para buzzer
    ledc_timer_config_t buzzer_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_RES,
        .timer_num        = LEDC_TIMER_1,
        .freq_hz          = BUZZER_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&buzzer_timer);

    // Configura canais para LEDs
    config_ledc_channel(LED1_GPIO, LEDC_CHANNEL_0, LEDC_TIMER_0);
    config_ledc_channel(LED2_GPIO, LEDC_CHANNEL_1, LEDC_TIMER_0);
    config_ledc_channel(LED3_GPIO, LEDC_CHANNEL_2, LEDC_TIMER_0);
    config_ledc_channel(LED4_GPIO, LEDC_CHANNEL_3, LEDC_TIMER_0);

    // Configura canal para Buzzer
    config_ledc_channel(BUZZER_GPIO, LEDC_CHANNEL_4, LEDC_TIMER_1);

    // Habilita função de fade
    ledc_fade_func_install(0);

    while (1) {
        // --- FASE 1: Fading sincronizado ---
        printf("Fase 1: Fading sincronizado\n");
        for (int ch = 0; ch < 4; ch++) {
            ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, ch,
                (1 << LEDC_RES) - 1, 2000, LEDC_FADE_NO_WAIT);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
        for (int ch = 0; ch < 4; ch++) {
            ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, ch,
                0, 2000, LEDC_FADE_NO_WAIT);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));

        // --- FASE 2: Fading sequencial ---
        printf("Fase 2: Fading sequencial\n");
        for (int ch = 0; ch < 4; ch++) {
            ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, ch,
                (1 << LEDC_RES) - 1, 1000, LEDC_FADE_WAIT_DONE);
            ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, ch,
                0, 1000, LEDC_FADE_WAIT_DONE);
        }

        // --- FASE 3: Buzzer ---
        printf("Fase 3: Teste do buzzer\n");
        for (int freq = 500; freq <= 2000; freq += 200) {
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, freq);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, (1 << (LEDC_RES-1)));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);
            vTaskDelay(pdMS_TO_TICKS(300));
        }
        for (int freq = 2000; freq >= 500; freq -= 200) {
            ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_1, freq);
            vTaskDelay(pdMS_TO_TICKS(300));
        }
        // Desliga o buzzer
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_4);
    }
}
