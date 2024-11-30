#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"
#include "driver/gpio.h"

#define BLINK_GPIO 5
#define GPIO_4 4 

#define GPIO_OUTPUT_PIN_IN ((1ULL<<GPIO_NUM_0))
#define GPIO_OUTPUT_PIN_OUT ((1ULL<<GPIO_NUM_2))

typedef enum{
    FLAG_DEACTIVATED,
    FLAG_ACTIVATED,
}flag_status;

typedef enum{
    LED_OFF,
    LED_ON,
}led_status;

flag_status button_flag;

void gpio_config_fun(){
    // Configuración del GPIO 0 como entrada
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_IN;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE; // Habilitar pull-up
    gpio_config(&io_conf);

    // Configuración del GPIO 2 como salida
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_OUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void change_led_state(){
    led_status status_led = LED_OFF;
    while(1){
        if(button_flag == FLAG_ACTIVATED){
            button_flag = FLAG_DEACTIVATED;
            if(status_led == LED_ON){
                gpio_set_level(GPIO_NUM_2, 0);
                status_led = LED_OFF;
            }
            else if(status_led == LED_OFF){
                gpio_set_level(GPIO_NUM_2, 1);
                status_led = LED_ON;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay para evitar el uso excesivo de CPU
    }
}

void read_button(void *pvParameter){
    while(1){
        if(gpio_get_level(GPIO_NUM_0) == 0){
            button_flag = FLAG_ACTIVATED;
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay para evitar el uso excesivo de CPU
    }
}

void app_main(void){
    gpio_config_fun();
    xTaskCreatePinnedToCore(&read_button, "read_button", 2048, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(&change_led_state, "change_led_state", 2048, NULL, 5, NULL, 0);
}