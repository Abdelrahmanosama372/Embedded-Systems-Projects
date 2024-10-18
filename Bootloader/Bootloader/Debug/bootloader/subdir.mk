################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../bootloader/bootloader.c 

OBJS += \
./bootloader/bootloader.o 

C_DEPS += \
./bootloader/bootloader.d 


# Each subdirectory must supply rules for building sources it contributes
bootloader/%.o bootloader/%.su bootloader/%.cyclo: ../bootloader/%.c bootloader/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"/media/abdelrahman/New Volume/stm/Bootloader/bootloader" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-bootloader

clean-bootloader:
	-$(RM) ./bootloader/bootloader.cyclo ./bootloader/bootloader.d ./bootloader/bootloader.o ./bootloader/bootloader.su

.PHONY: clean-bootloader

