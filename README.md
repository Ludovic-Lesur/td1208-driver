# Description

This repository contains the **TD1208** Sigfox module driver. For the moment it only implements the basic functions: reset, EP ID reading and uplink communications.

# Dependencies

The driver relies on:

* An external `types.h` header file defining the **standard C types** of the targeted MCU.
* The **embedded utility functions** defined in the [embedded-utils](https://github.com/Ludovic-Lesur/embedded-utils) repository.

Here is the versions compatibility table:

| **td1208-driver** | **embedded-utils** |
|:---:|:---:|
| [sw1.3](https://github.com/Ludovic-Lesur/td1208-driver/releases/tag/sw1.3) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw1.2](https://github.com/Ludovic-Lesur/td1208-driver/releases/tag/sw1.2) | >= [sw7.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw7.0) |
| [sw1.1](https://github.com/Ludovic-Lesur/td1208-driver/releases/tag/sw1.1) | [sw5.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw5.0) to [sw6.2](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.2) |
| [sw1.0](https://github.com/Ludovic-Lesur/td1208-driver/releases/tag/sw1.0) | [sw5.0](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw5.0) to [sw6.2](https://github.com/Ludovic-Lesur/embedded-utils/releases/tag/sw6.2) |

# Compilation flags

| **Flag name** | **Value** | **Description** |
|:---:|:---:|:---:|
| `TD1208_DRIVER_DISABLE_FLAGS_FILE` | `defined` / `undefined` | Disable the `td1208_driver_flags.h` header file inclusion when compilation flags are given in the project settings or by command line. |
| `TD1208_DRIVER_DISABLE` | `defined` / `undefined` | Disable the TD1208 driver. |
| `TD1208_DRIVER_UART_ERROR_BASE_LAST` | `<value>` | Last error base of the low level UART driver. |
| `TD1208_DRIVER_DELAY_ERROR_BASE_LAST` | `<value>` | Last error base of the low level delay driver. |
