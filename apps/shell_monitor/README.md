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

| Command                  | What it does                                        |
|--------------------------|-----------------------------------------------------|
| `help`                   | Show meta-key reference + top-level commands        |
| `<Tab>`                  | Auto-complete / list subcommands at any point       |
| `kernel thread list`     | All threads with state, prio, stack usage, % CPU    |
| `kernel thread stacks`   | Per-thread stack high-water marks                   |
| `kernel uptime`          | Uptime in ms (`-p`/`--pretty` for human format)     |
| `kernel cycles`          | CPU cycle counter                                   |
| `kernel sleep <ms>`      | Block the shell thread for `<ms>`                   |
| `kernel version`         | Zephyr kernel version (e.g. `Zephyr version 4.4.99`)|
| `kernel log_level <m> <s>`| Set log level for module `<m>` to severity 0–4     |
| `device list`            | All devicetree devices and their init status        |
| `devmem <addr> [width]`  | Peek physical memory                                |
| `devmem <addr> <w> <v>`  | Poke physical memory                                |
| `date`                   | Date commands (set/get if RTC is configured)        |
| `clear`                  | Clear screen                                        |
| `history`                | Command history                                     |
| `retval`                 | Exit code of the most recent command                |

## Custom commands (this app's `cmd_sys.c`)

| Command            | What it does                                              |
|--------------------|-----------------------------------------------------------|
| `sys info`         | Board, SoC, kernel version, uptime, CPU clock, tick rate  |
| `sys uptime`       | HH:MM:SS (formatted from `k_uptime_get()`)                |
| `sys led on`       | Force LD2 on (overridden ~1s later by next heartbeat tick)|
| `sys led off`      | Force LD2 off (same caveat)                               |
| `sys led toggle`   | Flip LD2                                                  |
| `sys led state`    | Read current LED pin level                                |

> Note: the `main` thread keeps toggling LD2 every 1 s in parallel, so
> manual `sys led on/off` only sticks until the next heartbeat tick. If
> you want manual control to be persistent, the heartbeat would need a
> "manual override" flag — left as an exercise.

## Verifying shell behavior with `tether` (AI-friendly)

`tether` lets a script (or AI agent) drive the shell non-interactively. It
shares the serial port with humans via a daemon, so you can have a `screen`
session AND `tether` running at the same time.

```bash
# 1. Start the daemon (once per boot of the host machine)
tetherd -D /dev/cu.usbmodem<TAB> -b 115200 &

# 2. Send commands and capture replies — atomic, holds writer lock
SOCK=/tmp/tether-*.sock     # or look up the actual socket path
tether -s $SOCK run --until 'g474> $' --newline crlf 'sys info'
tether -s $SOCK run --until 'g474> $' --newline crlf 'kernel thread list'
tether -s $SOCK run --until 'g474> $' --newline crlf 'sys led toggle'
```

This is the recommended way for an AI agent to validate that a firmware
change still works after rebuilding/reflashing.

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
