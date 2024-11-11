#ifndef _SRC_TIMER_H_
#define _SRC_TIMER_H_

#include "driver/timer.h"

#define TIMER_DIVIDER (16)
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)

class Timer {
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
    uint64_t timer_counter_value;
public:
    Timer() = default;

    void init() {
        timer_config_t config;
        config.divider = TIMER_DIVIDER;
        config.counter_dir = TIMER_COUNT_UP;
        config.counter_en = TIMER_PAUSE;
        config.alarm_en = TIMER_ALARM_DIS;
        config.auto_reload = TIMER_AUTORELOAD_EN;

        timer_init(TIMER_GROUP_0, TIMER_0, &config);
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    }

    void start() {
        timer_start(TIMER_GROUP_0, TIMER_0);
    }

    void reset() {
        stop();
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
        start();
    }

    void stop() {
        timer_pause(TIMER_GROUP_0, TIMER_0);
    }

    uint64_t count() {
        uint64_t count;
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &count);
        return count;
    }
};

#endif  // _SRC_TIMER_H_