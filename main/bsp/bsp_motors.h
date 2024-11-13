#ifndef _BSP_MOTOR_H_
#define _BSP_MOTOR_H_

#include "driver/gpio.h"
#include "driver/ledc.h"

class Motors {
    static inline bool _obstacle[4] = {false, false, false, false};
private:
    enum {
        FRONT = 0,
        LEFT = 1,
        RIGHT = 2,
        BACK = 3,
        FRONT_LEFT = 4,
        FRONT_RIGHT = 5,
        BACK_LEFT = 6,
        BACK_RIGHT = 7,
    };

    static constexpr gpio_num_t FL1_PIN = GPIO_NUM_4;
    static constexpr gpio_num_t FL2_PIN = GPIO_NUM_5;
    static constexpr gpio_num_t FR1_PIN = GPIO_NUM_6;
    static constexpr gpio_num_t FR2_PIN = GPIO_NUM_7;
    static constexpr gpio_num_t BL1_PIN = GPIO_NUM_15;
    static constexpr gpio_num_t BL2_PIN = GPIO_NUM_16;
    static constexpr gpio_num_t BR1_PIN = GPIO_NUM_17;
    static constexpr gpio_num_t BR2_PIN = GPIO_NUM_18;

    static constexpr gpio_num_t getForwardMotorPin(int motor) {
        switch (motor) {
            case FRONT_LEFT:
                return FL1_PIN;
            case FRONT_RIGHT:
                return FR1_PIN;
            case BACK_LEFT:
                return BL1_PIN;
            case BACK_RIGHT:
                return BR1_PIN;
        }
        return GPIO_NUM_NC;
    }

    static constexpr gpio_num_t getReverseMotorPin(int motor) {
        switch (motor) {
            case FRONT_LEFT:
                return FL2_PIN;
            case FRONT_RIGHT:
                return FR2_PIN;
            case BACK_LEFT:
                return BL2_PIN;
            case BACK_RIGHT:
                return BR2_PIN;
        }
        return GPIO_NUM_NC;
    }

    static constexpr ledc_channel_t getLedcChannel(int motor) {
        switch (motor) {
            case FRONT_LEFT:
                return LEDC_CHANNEL_0;
            case FRONT_RIGHT:
                return LEDC_CHANNEL_1;
            case BACK_LEFT:
                return LEDC_CHANNEL_2;
            case BACK_RIGHT:
                return LEDC_CHANNEL_3;
        }
        return LEDC_CHANNEL_MAX;
    }
    
private:
    static void init(int direction, int speed);
    static void internalSetMotorSpeed(int direction, int speed);

    typedef void (*motor_func_t)(int, int);
    static inline motor_func_t setMotorSpeed{init};

public:
    static void setObstacle(int direction, bool obstacle);
    static void cmdVelCallback(const void * msgin);
};

#endif // _BSP_MOTOR_H_