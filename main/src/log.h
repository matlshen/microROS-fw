#ifndef _SRC_LOG_H_
#define _SRC_LOG_H_

#include <rcl/rcl.h>
#include <std_msgs/msg/string.h>
#include <rclc/rclc.h>
#include <cstring>
#include <stdarg.h>

class log {
public:
    static inline rcl_publisher_t _log_publisher;
    static inline std_msgs__msg__String _log_msg;
    static inline bool _initialized = false;

    static void publish(const char *msg) {
        if (_initialized) {
            rcl_publish(&_log_publisher, &_log_msg, NULL);
        }
    }

    static void publish(const char *msg, va_list args) {
        vsnprintf(_log_msg.data.data, _log_msg.data.capacity, msg, args);
        _log_msg.data.data[_log_msg.data.capacity - 1] = '\0';
        _log_msg.data.size = strlen(_log_msg.data.data);
        publish(_log_msg.data.data);
    }

    static void init(const rcl_node_t *node) {
        _log_msg.data.data = (char *) malloc(100 * sizeof(char));
	    _log_msg.data.capacity = 100;

        rclc_publisher_init_default(
            &_log_publisher,
            node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
            "microROS_log");

        _initialized = true;
    }

    static void info(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        publish(fmt, args);
        va_end(args);
    }
};

#endif  // _SRC_LOG_H_