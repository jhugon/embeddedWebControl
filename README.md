Embedded Web Control
====================

This library makes it so you can use a web browser (and REST API) to check the
status of and configure an embedded project. This is an Mbed OS project for
Cortex-M.

Also, check out this udev rule:

https://github.com/mbedmicro/pyOCD/blob/master/udev/50-pyocd.rules

To debug, startup `pyocd-gdbserver` in one terminal and then run
`arm-none-eabi-gdbBUILD/K64F/GCC_ARM-DEBUG/*.elf` and then inside gdb: `target
remote localhost:3333`
