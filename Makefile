
STM_COMMON=./resources

OPENOCD_BOARD_DIR=/usr/share/openocd/scripts/board
OPENOCD_PROC_FILE=$(STM_COMMON)/prog_file/stm32f3-openocd.cfg

SRCS = main.c usart.c  usb.c usb_desc.c usb_hid.c $(STM_COMMON)/startup/system_stm32f30x.c stm32f30x_it.c
SRCS += $(STM_COMMON)/STM32F30x_StdPeriph_Driver/src/stm32f30x_rcc.c $(STM_COMMON)/STM32F30x_StdPeriph_Driver/src/stm32f30x_gpio.c 
SRCS += $(STM_COMMON)/STM32F30x_StdPeriph_Driver/src/stm32f30x_misc.c
SRCS += $(STM_COMMON)/STM32_USB-FS-Device_Driver/src/usb_regs.c
SRCS += $(STM_COMMON)/STM32_USB-FS-Device_Driver/src/usb_mem.c


PROJ_NAME=usart

#######################################################################################

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

CFLAGS  = -Wall -g -std=c99   
CFLAGS += -mlittle-endian -mcpu=cortex-m4  -march=armv7e-m -mthumb
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS += -ffunction-sections -fdata-sections 

# Include files from STM libraries
CFLAGS += -I$(STM_COMMON)/Include/  # core libs
CFLAGS += -I$(STM_COMMON)/startup/  # system and startup libs
CFLAGS += -I$(STM_COMMON)/STM32F30x_StdPeriph_Driver/inc  # peripheral libs
CFLAGS += -I$(STM_COMMON)/STM32_USB-FS-Device_Driver/inc
#CFLAGS += -I/usr/arm-none-eabi/include/
CFLAGS += -include stm32f30x_conf.h    # it contails assert macro thats why we include it in every file with -include directive


LDFLAGS = -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map


ST=$(STM_COMMON)/startup/startup_stm32f30x.s
STOB = $(ST:.s=.o)
# add startup file to build

OBJS = $(SRCS:.c=.o)


.PHONY: all
all: $(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS) startup_stm32f30x.o	
	@rm -rf ./output/
	@mkdir output	
	@$(CC) $(CFLAGS) $(LDFLAGS) -T$(STM_COMMON)/linker_scripts/STM32_FLASH.ld  $(OBJS) startup_stm32f30x.o -o $@
	rm @rm -f *.o $(OBJS) startup_stm32f30x.o $(PROJ_NAME).map
	$(info [003]  Linking done )	
	@$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex	
	$(info [004]  Hex file generated )
	@$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	$(info [005]  bin file generated )
	$(info [005]  Size details are )
	@$(SIZE) $(PROJ_NAME).elf
	@mv $(PROJ_NAME).hex $(PROJ_NAME).bin $(PROJ_NAME).elf ./output/

OBJS/%.o: SRCS/%.c
	$(CC) $(CFLAGS) -c $^ -o $@
	
startup_stm32f30x.o: $(ST)
	$(info [001]  Object Files Created)
	@$(CC) $(CFLAGS) -c $(ST) -o $@ 
	$(info [002]  Startup.o File Created)

program: 
	@openocd -f $(OPENOCD_BOARD_DIR)/stm32f3discovery.cfg -f $(OPENOCD_PROC_FILE) -c "stm_flash `pwd`/output/$(PROJ_NAME).bin" -c shutdown	
	$(info [001]  Program daal diya he ustad)
	
erase: 
	@openocd -f $(OPENOCD_BOARD_DIR)/stm32f3discovery.cfg -f $(OPENOCD_PROC_FILE) -c "stm_erase" -c shutdown
	$(info [001]  Maal saf krdya he)

halt: 
	@openocd -f $(OPENOCD_BOARD_DIR)/stm32f3discovery.cfg -f $(OPENOCD_PROC_FILE) -c "halt" -c shutdown
	$(info [001]  rok diya he ustaad)
run: 
	@openocd -f $(OPENOCD_BOARD_DIR)/stm32f3discovery.cfg -f $(OPENOCD_PROC_FILE) -c "run" -c shutdown
	$(info [001]  han ab dekh chal rha hoga)

.PHONY:clean
clean: 
	@rm -f ./output/*.o ./output/$(PROJ_NAME).elf ./output/$(PROJ_NAME).hex ./output/$(PROJ_NAME).bin $(OBJS)
	$(info [001]  Done cleaning)





