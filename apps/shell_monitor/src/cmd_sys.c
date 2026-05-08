/*
 * Custom shell command tree: `sys ...`
 *
 * Demonstrates how to add a domain-specific shell subtree on top of Zephyr's
 * built-in shell. Pattern: SHELL_STATIC_SUBCMD_SET_CREATE for the leaf set,
 * SHELL_CMD_REGISTER for the root.
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/version.h>

extern const struct gpio_dt_spec *shell_monitor_led(void);

static int cmd_sys_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Board     : %s", CONFIG_BOARD);
	shell_print(sh, "SoC       : %s", CONFIG_SOC);
	shell_print(sh, "Zephyr    : %s", KERNEL_VERSION_STRING);
	shell_print(sh, "Uptime    : %lld ms", k_uptime_get());
	shell_print(sh, "CPU clock : %u Hz", sys_clock_hw_cycles_per_sec());
	shell_print(sh, "Tick rate : %d Hz", CONFIG_SYS_CLOCK_TICKS_PER_SEC);
	return 0;
}

static int cmd_sys_led(const struct shell *sh, size_t argc, char **argv)
{
	const struct gpio_dt_spec *led = shell_monitor_led();
	int rc;

	if (argc != 2) {
		shell_error(sh, "usage: sys led <on|off|toggle|state>");
		return -EINVAL;
	}

	if (!strcmp(argv[1], "on")) {
		rc = gpio_pin_set_dt(led, 1);
	} else if (!strcmp(argv[1], "off")) {
		rc = gpio_pin_set_dt(led, 0);
	} else if (!strcmp(argv[1], "toggle")) {
		rc = gpio_pin_toggle_dt(led);
	} else if (!strcmp(argv[1], "state")) {
		int v = gpio_pin_get_dt(led);
		shell_print(sh, "led = %s", v ? "on" : "off");
		return 0;
	} else {
		shell_error(sh, "unknown subcommand '%s'", argv[1]);
		return -EINVAL;
	}

	if (rc < 0) {
		shell_error(sh, "gpio op failed: %d", rc);
		return rc;
	}
	return 0;
}

static int cmd_sys_uptime(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	int64_t ms = k_uptime_get();
	uint64_t secs = ms / 1000;
	uint32_t h = secs / 3600;
	uint32_t m = (secs % 3600) / 60;
	uint32_t s = secs % 60;

	shell_print(sh, "%02u:%02u:%02u (%lld ms)", h, m, s, ms);
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sys,
	SHELL_CMD(info,   NULL, "Show board/SoC/uptime/clocks",         cmd_sys_info),
	SHELL_CMD_ARG(led, NULL, "Control LD2: on|off|toggle|state",
		      cmd_sys_led, 2, 0),
	SHELL_CMD(uptime, NULL, "Show uptime as HH:MM:SS",               cmd_sys_uptime),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sys, &sub_sys, "System info & control commands", NULL);
