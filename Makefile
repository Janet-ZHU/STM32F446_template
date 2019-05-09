##################################################################################
#
##################################################################################

######################################
# TARGET
######################################
TARGET := arm-none-eabi

######################################
# NAME
######################################
NAME := STM32F446RE1

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og

#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# c source
######################################
SOURCES = $(shell find * -name "*.c")
#SOURCE  = ./src
#CFILES = \
# $(SOURCE)/$(TARGET).c 				\

######################################
# obj
######################################
OBJDIR = obj
SRCDIR = $(dir $(SOURCES))
BINDIR = $(addprefix $(OBJDIR)/, $(SRCDIR))

######################################
# ASM sources
######################################
ASMS = $(shell find * -name "*.s")
ASMDIR = $(dir $(ASMS))
ASMBIN = $(addprefix $(OBJDIR)/, $(ASMDIR))

#######################################
# binaries
#######################################
# devtools
CC := $(TARGET)-gcc
#LD := $(TARGET)-gcc
AS := $(TARGET)-gcc -x assembler-with-cpp
CP := $(TARGET)-objcopy
SZ = $(TARGET)-size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# tools
#######################################
STFLASH := st-flash
STUTIL := st-util
PORT := /dev/ttyACM0

#######################################
# CFLAGS
####################################### 
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=softfp

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


# C defines
C_DEFS =  \
-D__FPU_PRESENT
#-DUSE_FULL_LL_DRIVER \
#-DUSE_HAL_DRIVER \
#-DSTM32F446xx

# Includes
####################################### 
CMSISLIB   = ./official/CMSIS/
CMSIS_CORE = $(CMSISLIB)/core
CMSIS_DSP  = $(CMSISLIB)/DSP
INCLUDE    = ./include
INCPATHS   = $(CMSIS_CORE) $(CMSIS_DSP)/Include $(INCLUDE)
C_INCLUDES = $(addprefix -I ,$(INCPATHS))


# AS Settings
####################################### 
# AS includes
AS_INCLUDES = 

# AS defines
AS_DEFS = 

# C FLAGS
####################################### 
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections 
# AS FLAGS
####################################### 
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections 

# ADD Debug Options
####################################### 
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information 依存関係の生成
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
 

#######################################
# LDFLAGS
#######################################
#LDFLAGS = -lc -lm -lnosys  -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mtune=cortex-m4 -lm  -lnosys  -Wl,--gc-sections -nostartfiles

# link script
LDSCRIPT = STM32F446XE.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 

# LD FLAGS 
####################################### 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(OBJDIR)/$(NAME).map,--cref -Wl,--gc-sections -u _printf_float



# default :
# 	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
# 	@[ -d "$(BINDIR)" ] || mkdir -p $(BINDIR)
# 	@[ -d "$(ASMBIN)" ] || mkdir -p $(ASMBIN)
# 	@make all --no-print-directory

all: $(OBJDIR)/$(NAME).elf $(OBJDIR)/$(NAME).hex $(OBJDIR)/$(NAME).bin

# これけっこう謎だな…
OBJECTS = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SOURCES))) $(addprefix $(OBJDIR)/, $(patsubst %.s, %.o, $(ASMS))) 
DEPENDS = $(OBJECTS:.o=.d)

ifeq "$(strip $(OBJDIR))" ""
	OBJDIR = .
endif

$(OBJDIR)/%.o: %.c Makefile | $(OBJDIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(OBJDIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(OBJDIR)/%.o: %.s Makefile | $(OBJDIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(OBJDIR)/$(NAME).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(OBJDIR)/%.hex: $(OBJDIR)/%.elf | $(OBJDIR)
	$(HEX) $< $@
	
$(OBJDIR)/%.bin: $(OBJDIR)/%.elf | $(OBJDIR)
	$(BIN) $< $@	
	
$(OBJDIR):
	mkdir $@

.PHONY: clean flash

st-flash: $(NAME).bin
	make all
	$(STFLASH) write $(OBJDIR)/$(NAME).bin 0x8000000

st-util: $(NAME).bin
	$(STUTIL) $(OBJDIR)/$(NAME).bin

#######################################
# clean up
#######################################
clean:
	-rm -fR $(OBJDIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(OBJDIR)/*.d)