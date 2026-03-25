################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/Src/subdir.mk
-include Startup/subdir.mk
-include SSC/Src/subdir.mk
-include Libraries/XMCLib/src/subdir.mk
-include Libraries/Newlib/subdir.mk
-include Dave/Generated/UART/subdir.mk
-include Dave/Generated/TIMER/subdir.mk
-include Dave/Generated/INTERRUPT/subdir.mk
-include Dave/Generated/GLOBAL_DMA/subdir.mk
-include Dave/Generated/GLOBAL_CCU4/subdir.mk
-include Dave/Generated/GLOBAL_CAN/subdir.mk
-include Dave/Generated/E_EEPROM_XMC4/subdir.mk
-include Dave/Generated/EVENT_GENERATOR/subdir.mk
-include Dave/Generated/EVENT_DETECTOR/subdir.mk
-include Dave/Generated/ECAT_SSC/subdir.mk
-include Dave/Generated/DIGITAL_IO/subdir.mk
-include Dave/Generated/CPU_CTRL_XMC4/subdir.mk
-include Dave/Generated/CLOCK_XMC4/subdir.mk
-include Dave/Generated/CAN_NODE/subdir.mk
-include Dave/Generated/subdir.mk
-include BSP/subdir.mk
-include Application/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
XMC4300-boot.hex \

SECONDARY_LIST += \
XMC4300-boot.lst \

SECONDARY_SIZE += \
XMC4300-boot.siz \


LIST_FILES = $(addsuffix .o.lst, $(basename $(OBJS))) 
MAP_FILE = "XMC4300-boot.map"

TOOLCHAIN_ROOT=D:/Infineon/Tools/DAVE IDE/4.5.0.202105191637/eclipse/ARM-GCC-49
PROJECT_LOC=D:/Infineon/Tools/DAVE IDE/4.5.0.202105191637/eclipse/workspace/XMC4300-boot

# All Target
all: XMC4300-boot.elf secondary-outputs

# Tool invocations
XMC4300-boot.elf: $(OBJS) D:/Infineon/Tools/DAVE\ IDE/4.5.0.202105191637/eclipse/workspace/XMC4300-boot/boot_linker.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM-GCC C Linker'
	$(file >objects.rsp,$(OBJS))
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -T"D:/Infineon/Tools/DAVE IDE/4.5.0.202105191637/eclipse/workspace/XMC4300-boot/boot_linker.ld" -nostartfiles -Xlinker --gc-sections -specs=nano.specs -specs=nosys.specs -Wl,-Map,$(MAP_FILE) -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -g -gdwarf-2 -o "XMC4300-boot.elf" "@objects.rsp" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo.
XMC4300-boot.hex: XMC4300-boot.elf
	@echo 'Invoking: ARM-GCC Create Flash Image'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-objcopy" -O ihex "XMC4300-boot.elf" "XMC4300-boot.hex"
	@echo 'Finished building: $@'
	@echo.
XMC4300-boot.lst: XMC4300-boot.elf
	@echo 'Invoking: ARM-GCC Create Listing'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-objdump" -h -S "XMC4300-boot.elf" > "XMC4300-boot.lst"
	@echo 'Finished building: $@'
	@echo.
XMC4300-boot.siz: XMC4300-boot.elf
	@echo 'Invoking: ARM-GCC Print Size'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-size" --format=berkeley "XMC4300-boot.elf"
	@echo 'Finished building: $@'
	@echo.
# Other Targets
clean:
	-$(RM) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ARM_GCC_FOR_XMC_LINKER_OUTPUT_TYPE_OUTPUTS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS)
	-$(RM) Application/User_FoE_Impl.o Application/ecat_callback.o BSP/boot_init.o BSP/bootmode.o BSP/bsp_485.o BSP/bsp_can.o BSP/can_protocol.o BSP/dev_config.o BSP/e_eeprom_xmc4.o BSP/rs485_protocol.o BSP/systick.o Dave/Generated/CAN_NODE/can_node.o Dave/Generated/CAN_NODE/can_node_conf.o Dave/Generated/CLOCK_XMC4/clock_xmc4.o Dave/Generated/CLOCK_XMC4/clock_xmc4_conf.o Dave/Generated/CPU_CTRL_XMC4/cpu_ctrl_xmc4.o Dave/Generated/CPU_CTRL_XMC4/cpu_ctrl_xmc4_conf.o Dave/Generated/DAVE.o Dave/Generated/DIGITAL_IO/digital_io.o Dave/Generated/DIGITAL_IO/digital_io_conf.o Dave/Generated/ECAT_SSC/ecat_ssc.o Dave/Generated/ECAT_SSC/ecat_ssc_conf.o Dave/Generated/ECAT_SSC/xmc_eschw.o Dave/Generated/EVENT_DETECTOR/event_detector.o Dave/Generated/EVENT_DETECTOR/event_detector_conf.o Dave/Generated/EVENT_GENERATOR/event_generator.o Dave/Generated/EVENT_GENERATOR/event_generator_conf.o Dave/Generated/E_EEPROM_XMC4/e_eeprom_xmc4_conf.o Dave/Generated/GLOBAL_CAN/global_can.o Dave/Generated/GLOBAL_CAN/global_can_conf.o Dave/Generated/GLOBAL_CCU4/global_ccu4.o Dave/Generated/GLOBAL_CCU4/global_ccu4_conf.o Dave/Generated/GLOBAL_DMA/global_dma.o Dave/Generated/GLOBAL_DMA/global_dma_conf.o Dave/Generated/INTERRUPT/interrupt.o Dave/Generated/INTERRUPT/interrupt_conf.o Dave/Generated/TIMER/timer.o Dave/Generated/TIMER/timer_conf.o Dave/Generated/UART/uart.o Dave/Generated/UART/uart_conf.o Libraries/Newlib/syscalls.o Libraries/XMCLib/src/xmc4_eru.o Libraries/XMCLib/src/xmc4_flash.o Libraries/XMCLib/src/xmc4_gpio.o Libraries/XMCLib/src/xmc4_rtc.o Libraries/XMCLib/src/xmc4_scu.o Libraries/XMCLib/src/xmc_can.o Libraries/XMCLib/src/xmc_ccu4.o Libraries/XMCLib/src/xmc_ccu8.o Libraries/XMCLib/src/xmc_common.o Libraries/XMCLib/src/xmc_dac.o Libraries/XMCLib/src/xmc_dma.o Libraries/XMCLib/src/xmc_dsd.o Libraries/XMCLib/src/xmc_ebu.o Libraries/XMCLib/src/xmc_ecat.o Libraries/XMCLib/src/xmc_eru.o Libraries/XMCLib/src/xmc_eth_mac.o 
	-$(RM) Libraries/XMCLib/src/xmc_eth_phy_dp83848.o Libraries/XMCLib/src/xmc_eth_phy_ksz8031rnl.o Libraries/XMCLib/src/xmc_eth_phy_ksz8081rnb.o Libraries/XMCLib/src/xmc_fce.o Libraries/XMCLib/src/xmc_gpio.o Libraries/XMCLib/src/xmc_hrpwm.o Libraries/XMCLib/src/xmc_i2c.o Libraries/XMCLib/src/xmc_i2s.o Libraries/XMCLib/src/xmc_ledts.o Libraries/XMCLib/src/xmc_posif.o Libraries/XMCLib/src/xmc_rtc.o Libraries/XMCLib/src/xmc_sdmmc.o Libraries/XMCLib/src/xmc_spi.o Libraries/XMCLib/src/xmc_uart.o Libraries/XMCLib/src/xmc_usbd.o Libraries/XMCLib/src/xmc_usbh.o Libraries/XMCLib/src/xmc_usic.o Libraries/XMCLib/src/xmc_vadc.o Libraries/XMCLib/src/xmc_wdt.o SSC/Src/coeappl.o SSC/Src/ecatappl.o SSC/Src/ecatcoe.o SSC/Src/ecatfoe.o SSC/Src/ecatslv.o SSC/Src/mailbox.o SSC/Src/objdef.o SSC/Src/ryhand.o SSC/Src/sdoserv.o Startup/startup_XMC4300.o Startup/system_XMC4300.o User/Src/firmwareupdate_SSC.o main.o  XMC4300-boot.elf $(LIST_FILES) $(MAP_FILE) objects.rsp
	-@echo.
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
