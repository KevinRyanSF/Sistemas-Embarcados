#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// Pinos
#define LED1 GPIO_NUM_35
#define LED2 GPIO_NUM_36
#define LED3 GPIO_NUM_37
#define LED4 GPIO_NUM_38

#define BUTTON_A GPIO_NUM_6
#define BUTTON_B GPIO_NUM_7


//Variáveis
#define DEBOUNCE_US 200000

static uint8_t contador = 0;
static uint8_t incremento = 1;
static int64_t last_press_a = 0;
static int64_t last_press_b = 0;

//Funções
void atualizar_leds(uint8_t valor){
  gpio_set_level(LED1, (valor >> 0) & 0x01);
  gpio_set_level(LED2, (valor >> 1) & 0x01);
  gpio_set_level(LED3, (valor >> 2) & 0x01);
  gpio_set_level(LED4, (valor >> 3) & 0x01);
}

//Interrupções
//Botão A:
static void IRAM_ATTR button_a_isr(void *arg){
  int64_t agora = esp_timer_get_time();
  if (agora - last_press_a > DEBOUNCE_US){
    last_press_a = agora;
    contador = (contador + incremento) & 0x0F;
    atualizar_leds(contador);
  }
}

//Botão B:
static void IRAM_ATTR button_b_isr(void *arg){
  int64_t agora = esp_timer_get_time();
  if (agora - last_press_b > DEBOUNCE_US){
    last_press_b = agora;
    incremento = (incremento == 1) ? 2 : 1;
  }
}

//Setup
void gpio_configuracao(){
  //Leds:
  gpio_reset_pin(LED1);
  gpio_reset_pin(LED2);
  gpio_reset_pin(LED3);
  gpio_reset_pin(LED4);

  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED4, GPIO_MODE_OUTPUT);

  //Botões:
  gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_POSEDGE,
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = (1ULL << BUTTON_A) | (1ULL << BUTTON_B),
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE
  };
  gpio_config(&io_conf);

  //Interrupções:
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON_A, button_a_isr, NULL);
  gpio_isr_handler_add(BUTTON_B, button_b_isr, NULL);

  atualizar_leds(contador);
}





void app_main() {
  gpio_configuracao();
  while (true) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
