

# Minimal makefile for STM32F3 Neucleo board.
TARGET := arm-none-eabi

# devtools
CC := $(TARGET)-gcc
LD := $(TARGET)-gcc
AS := $(TARGET)-as
OBJCOPY := $(TARGET)-objcopy
STFLASH := st-flash
STUTIL := st-util
PORT := /dev/ttyACM0

CFLAGS = -g3 -mcpu=cortex-m4 -gdwarf-2
CFLAGS += -mtune=cortex-m4  -march=armv7e-m  
CFLAGS += -mthumb  -Os -ffunction-sections -Wall
CFLAGS += -fdata-sections -Wl,--gc-sections -std=gnu99 -mfloat-abi=softfp
CFLAGS += -std=gnu99 -mfloat-abi=softfp 
CFLAGS += -mfpu=fpv4-sp-d16  -D__FPU_PRESENT 
CMSISLIB = ./official/CMSIS/
CMSIS_CORE = $(CMSISLIB)/core
CMSIS_DSP = $(CMSISLIB)/DSP
MATHLIB = $(CMSISLIB)/Lib/ARM/arm_cortexM4lf_math.lib
INCLUDE = ./include
#INCPATHS = ./include 

INCPATHS = $(CMSIS_CORE) $(CMSIS_DSP)/Include $(INCLUDE)
#INCPATHS = $(INCLUDE)

INCLUDES     = $(addprefix -I ,$(INCPATHS))
 
LDFLAGS = -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mtune=cortex-m4 -lm -u _printf_float -Wl,--defsym=malloc_getpagesize_P=0x1000

OBJDIR = obj

SOURCES = $(shell find * -name "*.c")
SRCDIR = $(dir $(SOURCES))
BINDIR = $(addprefix $(OBJDIR)/, $(SRCDIR))

ASMS = $(shell find * -name "*.s")
ASMDIR = $(dir $(ASMS))
ASMBIN = $(addprefix $(OBJDIR)/, $(ASMDIR))

OBJECTS = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SOURCES))) $(addprefix $(OBJDIR)/, $(patsubst %.s, %.o, $(ASMS))) 
DEPENDS = $(OBJECTS:.o=.d)

ifeq "$(strip $(OBJDIR))" ""
	OBJDIR = .
endif

MEMORYMAP = STM32F446XE.ld
OBJECTNAME = STM

default :
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@[ -d "$(BINDIR)" ] || mkdir -p $(BINDIR)
	@[ -d "$(ASMBIN)" ] || mkdir -p $(ASMBIN)
	@make all --no-print-directory

all: $(OBJECTS) $(OBJECTNAME).bin $(OBJECTNAME).elf

$(OBJECTNAME).elf: $(OBJECTNAME).bin
	$(OBJCOPY) -I binary -O elf32-little $(OBJECTNAME).bin $(OBJECTNAME).elf

$(OBJECTNAME).bin: $(OBJECTNAME).out
	$(OBJCOPY) $(OBJECTNAME).out -I ihex -O binary $(OBJECTNAME).bin

$(OBJECTNAME).out: $(OBJECTS)
	$(LD) -T $(MEMORYMAP) $(OBJECTS) $(CFLAGS)  $(LDFLAGS) -o $(OBJECTNAME).out

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -S $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: %.s
	$(AS) -g $< -o $@

.PHONY: clean flash

st-flash: $(OBJECTNAME).bin
	$(STFLASH) write $(OBJECTNAME).bin 0x8000000

st-util: $(OBJECTNAME).bin
	$(STUTIL) $(OBJECTNAME).bin

clean:
	rm -rf $(OBJDIR) *.out *.bin *.elf
