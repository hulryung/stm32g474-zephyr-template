# shell_monitor

UART shell on LPUART1 (ST-Link VCP) with built-in Zephyr monitoring commands
and a small custom `sys` command tree. LD2 blinks at 1 Hz as a heartbeat.

## Build & flash

```bash
# from the workspace root (~/dev/zephyr)
west build -p auto -b nucleo_g474re apps/shell_monitor -d build/shell_monitor
west flash -r openocd -d build/shell_monitor
```

Or from VSCode: task **`Zephyr: Build (shell_monitor)`**, then
**`Zephyr: Flash (shell_monitor / openocd)`**.

## Connect to the shell

```bash
# auto-detect the ST-Link VCP and open at 115200
ls /dev/cu.usbmodem*                  # find the right port
screen /dev/cu.usbmodem<TAB> 115200    # exit: Ctrl-A then K
```

Or use the VSCode task **`Zephyr: Open Serial (115200)`** which auto-picks
the first matching port.

You should see the `g474> ` prompt. Press `<Tab>` for completion.

## Built-in commands (free, comes with Zephyr's shell subsystem)

| Command            | What it does                                              |
|--------------------|-----------------------------------------------------------|
| `help`             | List all top-level commands                               |
| `kernel threads`   | All threads, state, stack usage, runtime stats            |
| `kernel uptime`    | Uptime in ms                                              |
| `kernel cycles`    | CPU cycle counter                                         |
| `kernel stacks`    | Per-thread stack high-water marks                         |
| `kernel reboot`    | warm/cold reboot                                          |
| `device list`      | All devicetree devices and their init status              |
| `devmem`           | peek/poke physical memory                                 |
| `log`              | Runtime log level control per module                      |
| `version`          | Zephyr kernel version                                     |

## Custom commands (this app's `cmd_sys.c`)

| Command            | What it does                                              |
|--------------------|-----------------------------------------------------------|
| `sys info`         | Board, SoC, kernel version, uptime, CPU clock, tick rate  |
| `sys uptime`       | HH:MM:SS                                                  |
| `sys led on`       | Force LD2 on (overrides next heartbeat tick)              |
| `sys led off`      | Force LD2 off                                             |
| `sys led toggle`   | Flip LD2                                                  |
| `sys led state`    | Read current LED pin level                                |

## How to add a new shell command

See `src/cmd_sys.c`. Pattern is:

```c
static int cmd_my_thing(const struct shell *sh, size_t argc, char **argv) { … }

SHELL_STATIC_SUBCMD_SET_CREATE(sub_my,
    SHELL_CMD(thing, NULL, "help string", cmd_my_thing),
    SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(my, &sub_my, "my commands", NULL);
```

The `SHELL_*` macros place command descriptors into linker sections that
Zephyr collects at boot — no manual registration needed in `main()`.

## Files

```
apps/shell_monitor/
├── CMakeLists.txt
├── prj.conf            # SHELL=y, KERNEL_SHELL=y, DEVICE_SHELL=y, …
├── README.md
└── src/
    ├── main.c          # heartbeat thread on LD2
    └── cmd_sys.c       # `sys info|uptime|led` shell commands
```
