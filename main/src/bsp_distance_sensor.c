#include "bsp_distance_sensor.h"
#include "bsp_gpio.h"


void bsp_read_distance() {
    // Pulse out
    gpio_set_level(SENSOR_A_TRIG_PIN, 1);
    usleep(10);
    gpio_set_level(SENSOR_A_TRIG_PIN, 0);

    // Wait for pulse in
    while (gpio_get_level(SENSOR_A_ECHO_PIN) == 0);

    // Start timer
    
}