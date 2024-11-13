#include "bsp_distance_sensor.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

uint32_t DistanceSensor::internalMeasureDistance(int direction) {
    gpio_num_t trig_pin, echo_pin;
    switch (direction) {
        case front:
            trig_pin = TRIG_PIN_FRONT;
            echo_pin = ECHO_PIN_FRONT;
            break;
        case back:
            trig_pin = TRIG_PIN_BACK;
            echo_pin = ECHO_PIN_BACK;
            break;
        case left:
            trig_pin = TRIG_PIN_LEFT;
            echo_pin = ECHO_PIN_LEFT;
            break;
        case right:
            trig_pin = TRIG_PIN_RIGHT;
            echo_pin = ECHO_PIN_RIGHT;
            break;
        default:
            return static_cast<uint32_t>(-1);
    }

    // Send trigger pulse
    gpio_set_level(trig_pin, 0);
    esp_rom_delay_us(2);
    gpio_set_level(trig_pin, 1);
    esp_rom_delay_us(10);
    gpio_set_level(trig_pin, 0);

    // Measure the echo pulse duration
    int64_t startTime = esp_timer_get_time();
    while (gpio_get_level(echo_pin) == 0) {
        if ((esp_timer_get_time() - startTime) > MAX_DURATION) {
            return MAX_DISTANCE;  // Return max distance if timeout
        }
    }
    
    int64_t echoStart = esp_timer_get_time();
    while (gpio_get_level(echo_pin) == 1) {
        if ((esp_timer_get_time() - echoStart) > MAX_DURATION) {
            return MAX_DISTANCE;  // Return max distance if timeout
        }
    }
    
    int64_t echoEnd = esp_timer_get_time();
    int duration = (int)(echoEnd - echoStart);
    int distance = duration * MULTIPLIER / DIVIDER;  // Convert time to distance in mm
    return distance;
}

uint32_t DistanceSensor::init(int direction) {
    gpio_set_direction(TRIG_PIN_FRONT, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECHO_PIN_FRONT, GPIO_MODE_INPUT);

    _measure_func = internalMeasureDistance;
    return measureDistance(direction);
}