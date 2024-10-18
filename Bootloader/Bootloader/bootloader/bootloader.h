/*
 * bootloader.h
 *
 *  Created on: Sep 28, 2024
 *      Author: abdelrahman
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

//-----------------------------
//Includes
//-----------------------------
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "stm32f1xx_hal_crc.h"

#include <stdint.h>
#include <stdarg.h>



extern UART_HandleTypeDef huart1;
extern CRC_HandleTypeDef hcrc;

//-----------------------------
// Command Macros
//-----------------------------

// @brief Bootloader command to get the version.
#define BL_GET_VER_CMD              0x10

// @brief Bootloader command to get available help commands.

#define BL_GET_HELP_CMD             0x11

// @brief Bootloader command to get chip ID.
#define BL_GET_CID_CMD              0x12

// @brief Bootloader command to get the read protection level.
#define BL_GET_RDP_STATUS_CMD       0x13

// @brief Bootloader command to jump to a specific address.
#define BL_GO_TO_ADDR_CMD           0x14

// @brief Bootloader command to erase flash memory.
#define BL_FLASH_ERASE_CMD          0x15

// @brief Bootloader command to write data to memory.
#define BL_MEM_WRITE_CMD            0x16

// @brief Bootloader command to read data from memory.
#define BL_MEM_READ_CMD             0x17

//  @brief Bootloader command to jump to the main application.
#define BL_JUMP_TO_MAIN             0x18

// @brief Bootloader command to change read protection level.
#define BL_CHANGE_RDP_Level_CMD     0x19



// @brief UART interface for bootloader communication.
#define BL_UART                   &huart1


// @brief Bootloader acknowledgment code.
#define BL_ACK                      0x01
// @brief Bootloader not-acknowledgment code.
#define BL_NACK                     0x00

//-----------------------------
// Version Information Macros
//-----------------------------
// @brief Vendor ID for bootloader versioning.
#define BL_VENDOR_ID                  10
// @brief Bootloader software major version.
#define BL_SW_MAJOR_VERSION            1
// @brief Bootloader software minor version.
#define BL_SW_MINOR_VERSION            0
// @brief Bootloader software patch version.
#define BL_SW_PATCH_VERSION            0

//-----------------------------
// Flash Memory and Page Information
//-----------------------------
// @brief Number of flash memory pages.
#define NUM_OF_PAGES                 128
// @brief Size of each flash memory page in bytes.
#define PAGE_SIZE                   1024

// @brief Total flash memory size in bytes.
#define FLASH_SIZE                0x20000  // (128 kB)
// @brief Total SRAM memory size in bytes.
#define SRAM_SIZE                  0x5000  // (20 kB)



//-----------------------------
// Page Erase Status Macros
//-----------------------------
// @brief Status indicating page erase error.
#define PAGE_ERASE_ERROR              0x0
// @brief Status indicating page erase success.
#define PAGE_ERASE_SUCCESS            0x1


//-----------------------------
// Flash Write Status Macros
//-----------------------------
// @brief Status indicating flash write error.
#define FLASH_WRITE_ERROR             0x0
// @brief Status indicating flash write success.
#define FLASH_WRITE_SUCCESS           0x1

//-----------------------------
// CRC Verification Status Macros
//-----------------------------
// @brief Status indicating CRC verification error.
#define CRC_VERIFICATION_FAILED       0x0
// @brief Status indicating CRC verification success.
#define CRC_VERIFICATION_SUCCESS      0x1

//-----------------------------
// Bootloader Configuration
//-----------------------------
// @brief Maximum length of the bootloader buffer.
#define BL_BUFFER_LENGTH             1050
// @brief Maximum UART timeout for bootloader operations in milliseconds.
#define BL_MAX_TIMEOUT             100000

// @brief Build type (debug or release).
#define BUILD_TYPE_DEBUG             0
#define BUILD_TYPE_RELEASE           1

//  @brief Current build type.
#define BUILD_TYPE    BUILD_TYPE_RELEASE


// bootloader command execution status
typedef enum {
	BL_OK,
	BL_Error,
}BL_Status;

/*
* ===============================================
* APIs Supported by "Bootloader"
* ===============================================
*/
BL_Status Bootloader_Get_Command();


#endif /* BOOTLOADER_H_ */



