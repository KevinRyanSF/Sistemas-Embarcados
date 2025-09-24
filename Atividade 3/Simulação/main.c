#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_1 35
#define LED_2 36
#define LED_3 37
#define LED_4 38

// Funções
void contador4Led(void);
void set_led(int led_num, int state);
void resetLeds(void);
void sequencia4Led(void);

void app_main() {
  gpio_reset_pin(LED_1);
  gpio_reset_pin(LED_2);
  gpio_reset_pin(LED_3);
  gpio_reset_pin(LED_4);

  gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_3, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_4, GPIO_MODE_OUTPUT);

  while (true) {
    contador4Led();

    vTaskDelay(500 / portTICK_PERIOD_MS);
    resetLeds();
    sequencia4Led();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void contador4Led(void) {
    for (int i = 0; i < 16; i++) {
        gpio_set_level(LED_1, (i >> 0) & 0x01);  
        gpio_set_level(LED_2, (i >> 1) & 0x01);  
        gpio_set_level(LED_3, (i >> 2) & 0x01); 
        gpio_set_level(LED_4, (i >> 3) & 0x01); 

        vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
}

void set_led(int led_num, int state) {
    switch (led_num) {
        case 1: gpio_set_level(LED_1, state); break;
        case 2: gpio_set_level(LED_2, state); break;
        case 3: gpio_set_level(LED_3, state); break;
        case 4: gpio_set_level(LED_4, state); break;
    }
}


void resetLeds(void){
  gpio_set_level(LED_1, 0);
  gpio_set_level(LED_2, 0);
  gpio_set_level(LED_3, 0);
  gpio_set_level(LED_4, 0);
}

void sequencia4Led(void){
  for (int i = 1; i <= 4; i++) {
      set_led(i, 1);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      set_led(i, 0);
    }
  for (int i = 4; i >= 1; i--) {
    set_led(i, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    set_led(i, 0);
  }
}

