/*
 * bootloader.c
 *
 *  Created on: Sep 28, 2024
 *      Author: abdelrahman
 */

#include "bootloader.h"

//===============================================
//Global Variables
//===============================================
static uint8_t BL_Buffer[BL_BUFFER_LENGTH];

uint8_t BL_Commands[] = {
		BL_GET_VER_CMD,
		BL_GET_HELP_CMD,
		BL_GET_CID_CMD,
		BL_GET_RDP_STATUS_CMD,
		BL_GO_TO_ADDR_CMD,
		BL_FLASH_ERASE_CMD,
		BL_MEM_WRITE_CMD,
		BL_MEM_READ_CMD,
		BL_JUMP_TO_MAIN,
		BL_CHANGE_RDP_Level_CMD,
};

#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
	// used to test the Bootloader_Read_Memory command
	char hello_msg[] = "hello world from bootloader";
#endif



//===============================================
//type definitions
//===============================================
typedef void (*PFunc)();



/*
* ===============================================
* APIs Supported by "Bootloader"
* ===============================================
*/
void Bootloader_Write_Message(char *format,...);
static void Bootloader_Get_Version(uint8_t *data);
static void Bootloader_Get_Help(uint8_t *data);
static void Bootloader_Get_Chip_ID(uint8_t *data);
static void Bootloader_Get_Read_Protection_Status(uint8_t *data);

static BL_Status Bootloader_Go_TO_Address(uint8_t *data);
static BL_Status Bootloader_Erase_Flash(uint8_t *data);
static BL_Status Bootloader_Write_Memory(uint8_t *data);
static BL_Status Bootloader_Read_Memory(uint8_t *data);
static BL_Status Bootloader_Set_Read_Protection_Level(uint8_t *data);
static void Jump_To_App_Main(uint8_t *data);

static void Bootloader_Send_Ack();
static void Bootloader_Send_NAck();
static uint8_t Bootloader_CRC_Verification(uint8_t *pData, uint16_t data_length, uint32_t host_CRC);


/*
* ===============================================
* Helper functions
* ===============================================
*/
#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
	// used to test the Bootloader_Go_TO_Address command
	void print_hello_test()
	{
		Bootloader_Write_Message("hello world from Bootloader");
	}
#endif


/*
* ===============================================
* Bootloader APIs Definition
* ===============================================
*/

/**================================================================
* @Fn- Bootloader_Get_Command
* @brief - Handles receiving and processing bootloader commands from the host.
* @param [in] - None
* @param [out] - None
* @retval - BL_Status (Bootloader operation status: BL_OK or BL_Error)
* Note- It receives the command and data from UART, parses the command, and executes the corresponding bootloader action.
*/
BL_Status Bootloader_Get_Command()
{
	memset(BL_Buffer, 0, BL_BUFFER_LENGTH);
	BL_Status bl_status = BL_Error;
    HAL_StatusTypeDef HAL_Status = HAL_ERROR;
    uint8_t CRC_ver_status = CRC_VERIFICATION_FAILED;

	HAL_Status = HAL_UART_Receive(BL_UART, BL_Buffer, 2, BL_MAX_TIMEOUT);
	if(HAL_Status == HAL_OK)
	{
		uint16_t data_length = *((uint16_t*)BL_Buffer);
		HAL_Status = HAL_UART_Receive(BL_UART, BL_Buffer + 2, data_length, BL_MAX_TIMEOUT);

		uint32_t host_CRC = *((uint32_t *)(BL_Buffer + 2 + (data_length - 4)));
		CRC_ver_status = Bootloader_CRC_Verification(BL_Buffer, 3, host_CRC);

		if(HAL_Status == HAL_OK && CRC_ver_status == CRC_VERIFICATION_SUCCESS)
		{
			switch(BL_Buffer[2])
			{
				case BL_GET_VER_CMD:
					Bootloader_Get_Version(BL_Buffer);
					bl_status = BL_OK;
					break;

				case BL_GET_HELP_CMD:
					Bootloader_Get_Help(BL_Buffer);
					bl_status = BL_OK;
					break;

				case BL_GET_CID_CMD:
					Bootloader_Get_Chip_ID(BL_Buffer);
					bl_status = BL_OK;
					break;

				case BL_GET_RDP_STATUS_CMD:
					Bootloader_Get_Read_Protection_Status(BL_Buffer);
					bl_status = BL_OK;
					break;

				case BL_GO_TO_ADDR_CMD:
					bl_status = Bootloader_Go_TO_Address(BL_Buffer);
					break;

				case BL_FLASH_ERASE_CMD:
					bl_status = Bootloader_Erase_Flash(BL_Buffer);
					break;

				case BL_MEM_WRITE_CMD:
					bl_status = Bootloader_Write_Memory(BL_Buffer);
					break;

				case BL_MEM_READ_CMD:
					bl_status = Bootloader_Read_Memory(BL_Buffer);
					break;

				case BL_JUMP_TO_MAIN:
					Jump_To_App_Main(BL_Buffer);
					break;

				case BL_CHANGE_RDP_Level_CMD:
					bl_status = Bootloader_Set_Read_Protection_Level(BL_Buffer);
					break;
				default:
					break;
			}
		}else {
			#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
				Bootloader_Write_Message("bl could not receive the command");
			#endif
				Bootloader_Send_NAck();
		}
	}else {
		#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
			Bootloader_Write_Message("bl could not receive the command length");
		#endif
		Bootloader_Send_NAck();
	}

	return bl_status;
}

/**================================================================
* @Fn- Bootloader_Write_Message
* @brief - Sends formatted debug messages via UART to the host.
* @param [in] - char *format: Format string for the message
* @param [in] - ...: Variable arguments for the formatted string
* @param [out] - None
* @retval - None
* Note- Used primarily for debugging in debug build mode.
*/
void Bootloader_Write_Message(char *format,...)
{
   uint8_t message[100];

   va_list args;
   va_start(args, format);

   int len = vsnprintf(message, sizeof(message), format, args);
   message[len++] = '\n';

   HAL_UART_Transmit(BL_UART, message, len, BL_MAX_TIMEOUT);

   va_end(args);
}

/**================================================================
* @Fn-          Bootloader_Send_Data_To_Host
* @brief -      Sends data from the bootloader to the host via UART.
* @param [in] - data: Pointer to the data buffer to be sent.
*               length: The length of the data to be sent.
* @param [out] - None
* @retval -     None
* Note -        Sends an initial byte indicating the length of the data followed by the actual data.
*               If the data pointer is NULL, only the length byte is sent.
*/
static void Bootloader_Send_Data_To_Host(uint8_t *data, uint8_t length)
{
	HAL_UART_Transmit(BL_UART, &length, 1, BL_MAX_TIMEOUT);
	if(data != NULL)
		HAL_UART_Transmit(BL_UART, data, length, BL_MAX_TIMEOUT);
}

/**================================================================
* @Fn- Bootloader_Send_Ack
* @brief - Sends an acknowledgment (ACK) to the host via UART.
* @param [in] - None
* @param [out] - None
* @retval - None
* Note- The ACK signal indicates that the bootloader has successfully received and processed a command.
*/
static void Bootloader_Send_Ack()
{
	uint8_t ack = BL_ACK;
	HAL_UART_Transmit(BL_UART, &ack, 1, 100000);
}

/**================================================================
* @Fn- Bootloader_Send_NAck
* @brief - Sends a not-acknowledge (NACK) signal to the host via UART.
* @param [in] - None
* @param [out] - None
* @retval - None
* Note- The NACK signal indicates that the bootloader failed to process the received command.
*/
static void Bootloader_Send_NAck()
{
	uint8_t nack = BL_NACK;
	HAL_UART_Transmit(BL_UART, &nack, 1, 100000);
}

/**================================================================
* @Fn- isValidAddress
* @brief - Checks if the given address is valid within the flash or SRAM range.
* @param [in] - uint32_t address: The address to be validated
* @param [out] - uint8_t: Returns 1 if the address is valid, 0 otherwise
* @retval - uint8_t (1 for valid, 0 for invalid)
* Note- Used to verify if the memory address is in the valid flash or SRAM range.
*/
static uint8_t isValidAddress(uint32_t address)
{
	uint8_t isValid = 0;
	if(address >= FLASH_BASE && address <= (FLASH_BASE + FLASH_SIZE))
	{
		isValid = 1;
	}else if(address >= SRAM_BASE && address <= (SRAM_BASE + SRAM_SIZE))
	{
		isValid = 1;
	}

	return isValid;
}

/**================================================================
* @Fn- Bootloader_Get_Version
* @brief - Retrieves the bootloader version and sends it to the host.
* @param [in] - uint8_t *data: Received data buffer (not used in this function)
* @param [out] - None
* @retval - None
* Note- Sends an array containing the vendor ID, major, minor, and patch version to the host.
*/
static void Bootloader_Get_Version(uint8_t *data)
{
	uint8_t bl_version[] = {BL_VENDOR_ID, BL_SW_MAJOR_VERSION, BL_SW_MINOR_VERSION, BL_SW_PATCH_VERSION};
	Bootloader_Send_Ack();
	Bootloader_Send_Data_To_Host(bl_version, sizeof(bl_version));
}


/**================================================================
* @Fn- Bootloader_Get_Help
* @brief - Sends the list of supported bootloader commands to the host.
* @param [in] - uint8_t *data: Received data buffer (not used in this function)
* @param [out] - None
* @retval - None
* Note- Sends an array of supported command codes to the host.
*/
static void Bootloader_Get_Help(uint8_t *data)
{
	Bootloader_Send_Ack();
	Bootloader_Send_Data_To_Host(BL_Commands, sizeof(BL_Commands));
}

/**================================================================
* @Fn- Bootloader_Get_Chip_ID
* @brief - Retrieves the chip ID of the MCU and sends it to the host.
* @param [in] - uint8_t *data: Received data buffer (not used in this function)
* @param [out] - None
* @retval - None
* Note- Retrieves the MCU ID code from the IDCODE register and sends it to the host.
*/
static void Bootloader_Get_Chip_ID(uint8_t *data)
{
	uint16_t MCU_ID_Code = (uint16_t)(DBGMCU->IDCODE & 0x7FF);

	Bootloader_Send_Ack();
	Bootloader_Send_Data_To_Host((uint8_t *)&MCU_ID_Code, sizeof(MCU_ID_Code));
}


/**================================================================
* @Fn- Bootloader_Get_Read_Protection_Status
* @brief - Retrieves the current Read Protection (RDP) level and sends it to the host.
* @param [in] - uint8_t *data: Received data buffer (not used in this function)
* @param [out] - None
* @retval - None
* Note- The RDP level is obtained using HAL_FLASHEx_OBGetConfig and is sent to the host.
*/
static void Bootloader_Get_Read_Protection_Status(uint8_t *data)
{
	FLASH_OBProgramInitTypeDef pOBInit;

	HAL_FLASHEx_OBGetConfig(&pOBInit);

	uint8_t RDP_level = pOBInit.RDPLevel;

	Bootloader_Send_Ack();
	Bootloader_Send_Data_To_Host(&RDP_level, sizeof(RDP_level));
}

/**================================================================
* @Fn- Bootloader_Go_TO_Address
* @brief - Jumps to a specified memory address to execute code.
* @param [in] - uint8_t *data: Command data containing the target address
* @param [out] - BL_Status: BL_OK if address is valid, BL_Error otherwise
* @retval - BL_Status (Bootloader operation status)
* Note- Verifies if the address is valid before jumping, sends ACK or NACK accordingly.
*/
static BL_Status Bootloader_Go_TO_Address(uint8_t *data)
{
	BL_Status bl_status = BL_Error;
	uint32_t address = *((uint32_t *)(data + 3));

	uint8_t validAddress = isValidAddress(address);

	if(validAddress)
	{
		Bootloader_Send_Ack();
		Bootloader_Send_Data_To_Host(NULL, 0);

		PFunc function = (void *)(address+1);

		function();
	}else
	{
	    Bootloader_Send_NAck();
	}
	return bl_status;
}


/**================================================================
* @Fn- Flash_Memory_Erase_Pages
* @brief - Erases a specified number of flash pages starting from a given page number.
* @param [in] - uint8_t start_page: Starting page number for erasing
* @param [in] - uint8_t number_of_pages: Number of pages to erase
* @param [out] - uint8_t: Erase status (PAGE_ERASE_SUCCESS or PAGE_ERASE_ERROR)
* @retval - uint8_t (Erase status)
* Note- Unlocks flash, erases the specified pages, and locks flash again.
*/
static uint8_t Flash_Memory_Erase_Pages(uint8_t start_page, uint8_t number_of_pages)
{
	uint8_t erase_status = PAGE_ERASE_SUCCESS;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;

	if(start_page < NUM_OF_PAGES - 1)
	{
		if(start_page + number_of_pages > NUM_OF_PAGES)
		{
			number_of_pages = NUM_OF_PAGES - start_page;
		}

		HAL_Status = HAL_FLASH_Unlock();
		if(HAL_Status == HAL_OK)
		{
			FLASH_EraseInitTypeDef pEraseInit;
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.PageAddress = FLASH_BASE + start_page * PAGESIZE;
			pEraseInit.NbPages = number_of_pages;

			uint32_t PageError;
			HAL_Status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

			if(HAL_Status == HAL_OK && PageError == 0xFFFFFFFF)
			{
				HAL_Status = HAL_FLASH_Lock();
				while(HAL_Status == HAL_ERROR)
				{
					HAL_Status = HAL_FLASH_Lock();
					erase_status = PAGE_ERASE_ERROR;
				}
			}else
			{
				erase_status = PAGE_ERASE_ERROR;
			}
		}else
		{
			erase_status = PAGE_ERASE_ERROR;
		}
	}else
	{
		erase_status = PAGE_ERASE_ERROR;
	}

	return erase_status;
}

/**================================================================
* @Fn- Bootloader_Erase_Flash
* @brief - Erases specified flash memory pages as requested by the host.
* @param [in] - uint8_t *data: Command data containing start page and number of pages
* @param [out] - BL_Status: BL_OK if successful, BL_Error otherwise
* @retval - BL_Status (Bootloader operation status)
* Note- Sends an ACK or NACK depending on the success of the erase operation.
*/
static BL_Status Bootloader_Erase_Flash(uint8_t *data)
{
		BL_Status bl_status = BL_Error;
		uint8_t start_page = data[3];
		uint8_t number_of_pages = data[4];

		uint8_t erase_status = Flash_Memory_Erase_Pages(start_page, number_of_pages);

		if(erase_status == PAGE_ERASE_SUCCESS)
		{
			bl_status = BL_OK;
		}

		if(bl_status == BL_OK)
		{
			Bootloader_Send_Ack();
			Bootloader_Send_Data_To_Host(NULL, 0);
		}else
		{
			Bootloader_Send_NAck();
		}

		return bl_status;
}

/**================================================================
* @Fn- Flash_Memory_Write_Page
* @brief - Writes data to a specified flash page.
* @param [in] - uint8_t page_number: The flash page number to write to
* @param [in] - uint16_t payload_length: The length of the data to write
* @param [in] - uint8_t *payload: The data to be written to flash
* @param [out] - uint8_t: Write status (FLASH_WRITE_SUCCESS or FLASH_WRITE_ERROR)
* @retval - uint8_t (Write status)
* Note- Unlocks flash, writes the data to the specified page, and locks flash again.
*/
static uint8_t Flash_Memory_Write_Page(uint8_t page_number, uint16_t payload_length, uint8_t *payload)
{
	uint8_t Flash_Write_Status = FLASH_WRITE_SUCCESS;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;

	Flash_Memory_Erase_Pages(page_number, 1);
	uint32_t address = FLASH_BASE + page_number * PAGESIZE;

	HAL_Status = HAL_FLASH_Unlock();
	if(HAL_Status == HAL_OK)
	{
		for(int i=0, j=0; i< (payload_length/4 + 1) && j < 1024; i++, j +=4)
		{
			uint32_t curr_payload = *((uint32_t *) (payload + j));
			HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + j, curr_payload);
			if(HAL_Status == HAL_ERROR)
			{
				Flash_Write_Status = FLASH_WRITE_ERROR;
				break;
			}
		}

		HAL_Status = HAL_FLASH_Lock();
	}else
	{
		Flash_Write_Status = FLASH_WRITE_ERROR;
	}

	return Flash_Write_Status;
}

/**================================================================
* @Fn- Jump_To_App_Main
* @brief - Jumps to the main application stored in the flash memory.
* @param [in] - uint8_t *data: Command data containing the flash page number
* @param [out] - None
* @retval - None
* Note- Configures the vector table and resets the stack pointer before jumping to the main application.
*/
static void Jump_To_App_Main(uint8_t *data)
{
    uint8_t page_number = data[3];
    uint32_t address = FLASH_BASE + page_number * PAGESIZE;

    HAL_RCC_DeInit();

    SCB->VTOR = address;

    __set_MSP(*(volatile uint32_t*)address);

    PFunc reset_handler = (PFunc)(*(volatile uint32_t*)(address + 4));
    reset_handler();
}

/**================================================================
* @Fn- Bootloader_Write_Memory
* @brief - Writes data to the flash memory as requested by the host.
* @param [in] - uint8_t *data: Command data containing the page number and data payload
* @param [out] - BL_Status: BL_OK if successful, BL_Error otherwise
* @retval - BL_Status (Bootloader operation status)
* Note- Sends an ACK or NACK based on the success of the write operation.
*/
static BL_Status Bootloader_Write_Memory(uint8_t *data)
{
	BL_Status bl_status = BL_Error;
	uint8_t page_number = data[3];

	uint16_t payload_length = *((uint16_t *)(data + 4));;

	if(page_number >=0 && page_number <= 127)
	{
		uint8_t write_status = Flash_Memory_Write_Page(page_number, payload_length, data + 6);

		if(write_status == FLASH_WRITE_SUCCESS)
		{
			bl_status = BL_OK;
		}
	}

	if(bl_status == BL_OK)
	{
		Bootloader_Send_Ack();
		Bootloader_Send_Data_To_Host((uint8_t *) &payload_length, 2);
	}else
	{
		Bootloader_Send_NAck();
	}

	return bl_status;
}

/**================================================================
* @Fn- Bootloader_Read_Memory
* @brief - Reads data from the flash memory and sends it to the host.
* @param [in] - uint8_t *data: Command data containing the start address and byte count
* @param [out] - BL_Status: BL_OK if successful, BL_Error otherwise
* @retval - BL_Status (Bootloader operation status)
* Note- Sends the requested data to the host if the address is valid, otherwise sends a NACK.
*/
static BL_Status Bootloader_Read_Memory(uint8_t *data)
{
	BL_Status bl_status = BL_Error;
	uint32_t address = *((uint32_t *)(data+3));
	uint32_t number_of_bytes = *((uint32_t *)(data+7));

	if(isValidAddress(address))
	{
		Bootloader_Send_Ack();
		Bootloader_Send_Data_To_Host((uint8_t *)address, number_of_bytes);
		bl_status = BL_OK;
	}else {
		Bootloader_Send_NAck();
		bl_status = BL_Error;
	}

	return bl_status;
}

/**================================================================
* @Fn- Bootloader_Set_Read_Protection_Level
* @brief - Sets the Read Protection (RDP) level of the flash memory.
* @param [in] - uint8_t *data: Command data containing the desired RDP level
* @param [out] - BL_Status: BL_OK if successful, BL_Error otherwise
* @retval - BL_Status (Bootloader operation status)
* Note- The RDP level is set using HAL_FLASHEx_OBProgram. Sends an ACK or NACK based on the result.
*/

static BL_Status Bootloader_Set_Read_Protection_Level(uint8_t *data)
{
	uint8_t RDP_level = data[3];
	BL_Status bl_status = BL_Error;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	FLASH_OBProgramInitTypeDef pOBInit;

	if(RDP_level == OB_RDP_LEVEL_0 || RDP_level == OB_RDP_LEVEL_1)
	{
		HAL_FLASHEx_OBGetConfig(&pOBInit);
		pOBInit.RDPLevel = RDP_level;

		HAL_Status = HAL_FLASH_Unlock();
		if(HAL_Status == HAL_OK)
		{
			HAL_Status = HAL_FLASH_OB_Unlock();
			if(HAL_Status == HAL_OK)
			{
				HAL_Status = HAL_FLASHEx_OBProgram(&pOBInit);
				if(HAL_Status == HAL_OK)
				{
					HAL_Status = HAL_FLASH_OB_Lock();
					if(HAL_Status == HAL_OK)
					{
						HAL_Status = HAL_FLASH_Lock();
						if(HAL_Status == HAL_OK)
						{
							bl_status = BL_OK;
						}else
						{
							#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
								Bootloader_Write_Message("bl flash lock failed");
							#endif
							bl_status = BL_Error;
						}
					}else
					{
						#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
							Bootloader_Write_Message("bl flash option byte lock failed");
						#endif
						bl_status = BL_Error;
					}

				}else
				{
					#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
						Bootloader_Write_Message("bl program flash options failed");
					#endif
					bl_status = BL_Error;
				}
			}else
			{
				#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
					Bootloader_Write_Message("bl flash options unlock failed");
				#endif
				bl_status = BL_Error;
			}
		}else
		{
			#if (BUILD_TYPE == BUILD_TYPE_DEBUG)
				Bootloader_Write_Message("bl flash unlock failed");
			#endif
			bl_status = BL_Error;
		}
	}else {
		bl_status = BL_Error;
	}


	if(bl_status == BL_OK)
	{
		Bootloader_Send_Ack();
		Bootloader_Send_Data_To_Host(NULL, 0);

		// force the reload of the options bytes
		HAL_FLASH_OB_Launch();
	}else
	{
		Bootloader_Send_NAck();
	}
	return bl_status;
}

/**================================================================
* @Fn           - Bootloader_CRC_Verification
* @brief        - Verifies the integrity of the received data using CRC.
* @param [in]   - uint8_t *pData: Pointer to the data buffer that needs to be verified.
* @param [in]   - uint16_t data_length: Length of the data buffer (number of bytes).
* @param [in]   - uint32_t host_CRC: CRC value calculated on the host side for comparison.
* @param [out]  - uint8_t: Returns CRC_VERIFICATION_SUCCESS if CRC matches, or CRC_VERIFICATION_FAILED otherwise.
* @retval       - uint8_t (CRC verification status)
* Note          - The function accumulates the CRC using HAL_CRC_Accumulate() for the data and compares it with the host-provided CRC.
*                 The CRC is reset after each verification using __HAL_CRC_DR_RESET().
*/
static uint8_t Bootloader_CRC_Verification(uint8_t *pData, uint16_t data_length, uint32_t host_CRC)
{
	uint8_t CRC_ver_status = CRC_VERIFICATION_FAILED;
	uint32_t MCU_CRC = 0;
	uint8_t data_counter = 0;

	for(data_counter = 0; data_counter < data_length; data_counter++)
	{
		uint32_t data = (uint32_t)pData[data_counter];
		MCU_CRC = HAL_CRC_Accumulate(&hcrc, &data, 1);
	}

	__HAL_CRC_DR_RESET(&hcrc);

	if(MCU_CRC == host_CRC)
	{
		CRC_ver_status = CRC_VERIFICATION_SUCCESS;
	}
	else
	{
		CRC_ver_status = CRC_VERIFICATION_FAILED;
	}

	return CRC_ver_status;
}
