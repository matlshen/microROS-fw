#include "bsp_motors.h"
#include <stdlib.h>
#include <geometry_msgs/msg/twist.h>

void Motors::init(int motor, int speed) {
    // Configure LEDC timer, 256kHz, 7-bit duty resolution
    const ledc_timer_config_t ledc_tim_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_7_BIT,    // 2^7=128
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 256000,                      // 2k cycles/s
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_tim_cfg));

    // Configure 4 LEDC timer channels
    ledc_channel_config_t ledc_channel_cfg = {
        .gpio_num = FL1_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags = {
            .output_invert = 0
        },
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
    ledc_channel_cfg.gpio_num = FR1_PIN;
    ledc_channel_cfg.channel = LEDC_CHANNEL_1;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
    ledc_channel_cfg.gpio_num = BL1_PIN;
    ledc_channel_cfg.channel = LEDC_CHANNEL_2;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));
    ledc_channel_cfg.gpio_num = BR1_PIN;
    ledc_channel_cfg.channel = LEDC_CHANNEL_3;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_cfg));

    setMotorSpeed = internalSetMotorSpeed;
    setMotorSpeed(motor, speed);
}

void Motors::internalSetMotorSpeed(int motor, int speed) {
    gpio_num_t forward_pin = getForwardMotorPin(motor);
    gpio_num_t reverse_pin = getReverseMotorPin(motor);
    ledc_channel_t ledc_channel = getLedcChannel(motor);

    if (speed >= 0) {
        ledc_set_pin(forward_pin, LEDC_LOW_SPEED_MODE, ledc_channel);
        gpio_set_direction(reverse_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(reverse_pin, 0);
    }
    else {
        ledc_set_pin(reverse_pin, LEDC_LOW_SPEED_MODE, ledc_channel);
        gpio_set_direction(forward_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(forward_pin, 0);
    }

    ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel, abs(speed));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ledc_channel);
}

void Motors::setObstacle(int direction, bool obstacle) {
    if (direction <= BACK) {
        _obstacle[direction] = obstacle;
    }
}

void Motors::cmdVelCallback(const void * msgin) {
    const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;

    // Convert linear speeds from double 0-1 to int 0-100
    int scaled_vx = msg->linear.x * 128;
    int scaled_vy = msg->linear.y * 128;

    // Restrict motion if obstacle detected
    if (_obstacle[FRONT] && scaled_vx > 0) {
        scaled_vx = 0;
    }
    if (_obstacle[BACK] && scaled_vx < 0) {
        scaled_vx = 0;
    }
    if (_obstacle[LEFT] && scaled_vy > 0) {
        scaled_vy = 0;
    }
    if (_obstacle[RIGHT] && scaled_vy < 0) {
        scaled_vy = 0;
    }
    

    // Convert angular speed from double 0-1 to int 0-50
    int scaled_w = msg->angular.z * 64;

    // Calculate motor speeds
    int fl_speed = scaled_vx - scaled_vy - scaled_w;
    int fr_speed = scaled_vx + scaled_vy + scaled_w;
    int bl_speed = scaled_vx + scaled_vy - scaled_w;
    int br_speed = scaled_vx - scaled_vy + scaled_w;

    setMotorSpeed(FRONT_LEFT, fl_speed);
    setMotorSpeed(FRONT_RIGHT, fr_speed);
    setMotorSpeed(BACK_LEFT, bl_speed);
    setMotorSpeed(BACK_RIGHT, br_speed);
}