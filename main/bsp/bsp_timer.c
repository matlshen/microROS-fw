#include "bsp_timer.h"
#include "driver/gptimer.h"

struct bsp_timer_t {
    gptimer_handle_t gptimer;
    gptimer_config_t timer_config;
};

static struct bsp_timer_t bsp_timer;

void bsp_timer_init() {
    bsp_timer.gptimer = NULL;
    bsp_timer.timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    bsp_timer.timer_config.direction = GPTIMER_COUNT_UP;
    bsp_timer.timer_config.resolution_hz = 1 * 1000 * 1000; // 1 MHz

    gptimer_new_timer(&bsp_timer.timer_config, &bsp_timer.gptimer);
    gptimer_enable(bsp_timer.gptimer);
}

void bsp_timer_start() {
    gptimer_start(bsp_timer.gptimer);
}