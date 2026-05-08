/*
 * shell_monitor — UART shell + system monitoring example
 *
 * - Heartbeat: blinks LD2 (PA5) at 1 Hz so the user can see the device is alive.
 * - Shell:     LPUART1 (ST-Link VCP, /dev/cu.usbmodem* @ 115200).
 *              Built-in commands: kernel, device, devmem, log, version.
 *              Custom command:    sys (see cmd_sys.c).
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(shell_monitor, LOG_LEVEL_INF);

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Exposed to cmd_sys.c so `sys led` can flip the LED state. */
const struct gpio_dt_spec *shell_monitor_led(void)
{
	return &led;
}

int main(void)
{
	if (!gpio_is_ready_dt(&led)) {
		LOG_ERR("LED device not ready");
		return -1;
	}

	if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
		LOG_ERR("LED configure failed");
		return -1;
	}

	LOG_INF("shell_monitor up — try `help`, `kernel threads`, `sys info`");

	while (1) {
		gpio_pin_toggle_dt(&led);
		k_msleep(1000);
	}
	return 0;
}
