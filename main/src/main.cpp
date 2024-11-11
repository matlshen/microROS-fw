#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/uart.h"
#include "driver/gptimer.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/string.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include "esp32_serial_transport.h"

#include "timer.h"
#include "log.h"
#include "motor.h"
#include "bsp_gpio.h"

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;

rcl_subscription_t subscriber;
std_msgs__msg__Int32 recv_msg;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time);
void sub_callback(const void * msgin);
void micro_ros_task(void * arg);

static size_t uart_port = UART_NUM_0;
static gptimer_handle_t gptimer = NULL;
static gptimer_config_t timer_config = {
	.clk_src = GPTIMER_CLK_SRC_DEFAULT,
	.direction = GPTIMER_COUNT_UP,
	.resolution_hz = 1 * 1000 * 1000, // 1 MHz
};

extern "C" void app_main(void)
{
#if defined(RMW_UXRCE_TRANSPORT_CUSTOM)
	rmw_uros_set_custom_transport(
		true,
		(void *) &uart_port,
		esp32_serial_open,
		esp32_serial_close,
		esp32_serial_write,
		esp32_serial_read
	);
#else
#error micro-ROS transports misconfigured
#endif  // RMW_UXRCE_TRANSPORT_CUSTOM

	// BSP Init
	bsp_gpio_init();
	gpio_set_level((gpio_num_t)LED_PIN, 1);

	// Just test GPIO
	while (1) {
		gpio_set_level((gpio_num_t)SENSOR_A_TRIG_PIN, 1);
		gpio_set_level((gpio_num_t)LED_PIN, 1);
		usleep(10);
		gpio_set_level((gpio_num_t)SENSOR_A_TRIG_PIN, 0);
		gpio_set_level((gpio_num_t)LED_PIN, 0);
		usleep(100000);
	}
	

    xTaskCreate(micro_ros_task,
            "uros_task",
            CONFIG_MICRO_ROS_APP_STACK,
            NULL,
            CONFIG_MICRO_ROS_APP_TASK_PRIO,
            NULL);

	// Set up timer stuff
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
	gptimer_set_raw_count(gptimer, 0);
	gptimer_enable(gptimer);
	gptimer_start(gptimer);
}

void sub_callback(const void * msgin)
{
	const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
	log::info("Received: %d", msg->data);
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
	RCLC_UNUSED(last_call_time);
	if (timer != NULL) {
		// hw_timer.reset();
		RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
		msg.data++;
		uint64_t gptimer_get;
		gptimer_get_raw_count(gptimer, &gptimer_get);
		log::info("Hello World: %llu", gptimer_get);
	}
}

void micro_ros_task(void * arg)
{
	rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	// create init_options
	RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

	// create node
	rcl_node_t node;
	RCCHECK(rclc_node_init_default(&node, "esp32_int32_publisher", "", &support));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"freertos_int32_publisher"));

	// create subscriber
	RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
		"freertos_int32_subscriber"));
	
	// create timer,
	rcl_timer_t timer;
	const unsigned int timer_timeout = 1000;
	RCCHECK(rclc_timer_init_default(
		&timer,
		&support,
		RCL_MS_TO_NS(timer_timeout),
		timer_callback));

	// create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
	RCCHECK(rclc_executor_add_timer(&executor, &timer));
	RCCHECK(rclc_executor_add_subscription(&executor, 
		&subscriber, &recv_msg, &sub_callback, ON_NEW_DATA));

	// User initialization begin
	log::init(&node);
	// User initialization end

	msg.data = 0;

	while(1){
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
		usleep(10000);
	}

	// free resources
	RCCHECK(rcl_publisher_fini(&publisher, &node));
	RCCHECK(rcl_subscription_fini(&subscriber, &node));
	RCCHECK(rcl_node_fini(&node));

  	vTaskDelete(NULL);
}