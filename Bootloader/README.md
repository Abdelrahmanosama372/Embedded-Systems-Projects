# Bootloader for MCU with UART Communication

This project implements a bootloader for an MCU that communicates with a host via UART. The bootloader allows the host to perform various operations, such as reading/writing memory, erasing flash, and checking the MCU's read protection status. A `host.py` script is included to handle the communication between the host machine and the MCU over a serial connection.

## Features

- **UART Communication:** The bootloader communicates with the host via UART.
- **Command Handling:** Supports multiple bootloader commands, including:
  - Get bootloader version
  - Get list of supported commands
  - Get chip ID
  - Get read protection (RDP) status
  - Jump to a specified memory address
  - Erase flash memory pages
  - Write data to flash memory
  - Read data from flash memory
  - Set read protection level
- **Memory Operations:** Allows reading and writing to the MCU's flash memory.
- **Error Handling:** Implements ACK/NACK signaling for successful or failed command processing.
- **Debug Mode:** Conditional debug messages for easier development and debugging.

## Commands

The bootloader supports the following commands, which can be sent by the host:

- `BL_GET_VER_CMD` - Get bootloader version
- `BL_GET_HELP_CMD` - Get list of supported commands
- `BL_GET_CID_CMD` - Get MCU Chip ID
- `BL_GET_RDP_STATUS_CMD` - Get Read Protection (RDP) status
- `BL_GO_TO_ADDR_CMD` - Jump to a specified memory address
- `BL_FLASH_ERASE_CMD` - Erase specified flash memory pages
- `BL_MEM_WRITE_CMD` - Write data to flash memory
- `BL_MEM_READ_CMD` - Read data from flash memory
- `BL_JUMP_TO_MAIN` - Jump to the main application
- `BL_CHANGE_RDP_LEVEL_CMD` - Set the RDP (Read Protection) level

## File Structure

- **bootloader.h & bootloader.c**: Contains the bootloader's implementation, including command handling, memory operations, and UART communication.
- **host.py**: Python script used to communicate with the bootloader over a UART serial connection from the host machine.

## Host.py Overview

`host.py` is a simple Python script that interacts with the bootloader. It sends commands via UART and receives responses from the bootloader, allowing the user to control the bootloader from a host machine.

### Requirements

- Python 3.x
- `pyserial` library: Install via pip using `pip install pyserial`

### Usage

1. Connect your MCU to the host machine via UART (e.g., using a USB-to-serial adapter).
2. Run the `host.py` script, which sends bootloader commands to the MCU and reads responses.

Example command to run `host.py`:
```bash
python3 host.py
```

## Communication Protocol

The bootloader and host communicate via UART with the following structure:

    Each command sent from the host to the bootloader starts with a 2-byte length field, followed by the command code and optional data.
    The bootloader responds with either an acknowledgment (ACK) or a not-acknowledgment (NACK) signal based on the success of the command execution.

## CRC Verification

Each data transfer includes a CRC check to ensure data integrity. If the CRC check fails, the bootloader sends a NACK to the host.

## Debugging

If compiled in debug mode (BUILD_TYPE_DEBUG), the bootloader sends debug messages over UART. These messages can be useful for development and troubleshooting.

## How to Build

1. Clone this repository.
2. Adjust the bootloader.h file to match your MCU and project requirements (e.g., UART port, buffer sizes).
3. Build and flash the bootloader code to your MCU.