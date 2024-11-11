#include "bsp_gpio.h"

void bsp_gpio_init() {
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SENSOR_A_TRIG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SENSOR_A_ECHO_PIN, GPIO_MODE_INPUT);
}