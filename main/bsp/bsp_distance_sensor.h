#ifndef _BSP_DISTANCE_SENSOR_H_
#define _BSP_DISTANCE_SENSOR_H_

#include "driver/gpio.h"

class DistanceSensor {
public:
    static constexpr int front = 0;
    static constexpr int back = 1;
    static constexpr int left = 2;
    static constexpr int right = 3;

private:
    static constexpr gpio_num_t TRIG_PIN_FRONT = GPIO_NUM_19;
    static constexpr gpio_num_t ECHO_PIN_FRONT = GPIO_NUM_20;
    static constexpr gpio_num_t TRIG_PIN_BACK = GPIO_NUM_21;
    static constexpr gpio_num_t ECHO_PIN_BACK = GPIO_NUM_33;
    static constexpr gpio_num_t TRIG_PIN_LEFT = GPIO_NUM_34;
    static constexpr gpio_num_t ECHO_PIN_LEFT = GPIO_NUM_45;
    static constexpr gpio_num_t TRIG_PIN_RIGHT = GPIO_NUM_35;
    static constexpr gpio_num_t ECHO_PIN_RIGHT = GPIO_NUM_36;

    // Distance (mm) = duration (us) * MULTIPLIER / DIVIDER
    static constexpr uint32_t MULTIPLIER = 100;
    static constexpr uint32_t DIVIDER = 58;

    // Maximum distance to measure (in mm)
    static constexpr uint32_t MAX_DISTANCE = 20000;
    // Maximum duration to measure (in us)
    static constexpr uint32_t MAX_DURATION = MAX_DISTANCE * DIVIDER / MULTIPLIER;

    static uint32_t internalMeasureDistance(int direction);
    static uint32_t init(int direction);

    typedef uint32_t (*measure_func_t)(int);
    static inline measure_func_t _measure_func{init};

public:
    static inline uint32_t measureDistance(int direction) {
        return _measure_func(direction);
    }
};

#endif // _BSP_DISTANCE_SENSOR_H_