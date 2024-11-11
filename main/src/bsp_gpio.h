#ifndef _SRC_BSP_GPIO_H_
#define _SRC_BSP_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define SENSOR_A_TRIG_PIN GPIO_NUM_16
#define SENSOR_A_ECHO_PIN GPIO_NUM_17

void bsp_gpio_init();
void bsp_timer_start();

#ifdef __cplusplus
}
#endif

#endif // _SRC_BSP_GPIO_H_