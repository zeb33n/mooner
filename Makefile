TARGET_DEVICE = gameblaster
PROGRAM_NAME  = mooner

BUILD_DIR  ?= .build
OUTPUT_DIR ?= .

ELF_RELEASE   = $(OUTPUT_DIR)/$(PROGRAM_NAME).elf
ELF_DEBUG     = $(OUTPUT_DIR)/$(PROGRAM_NAME)-debug.elf
LINKER_SCRIPT = firmware/device/$(TARGET_DEVICE)/program-sram.ld

CROSS_COMPILE = arm-none-eabi-
TARGET_AS     = $(CROSS_COMPILE)as
TARGET_CC     = $(CROSS_COMPILE)gcc
TARGET_LD     = $(CROSS_COMPILE)gcc
TARGET_SIZE   = $(CROSS_COMPILE)size

INCLUDE         = -I include -I firmware/api -I libraries -I $(BUILD_DIR)/include
FLAGS_ARCH      = -mlittle-endian -mcpu=cortex-m4
ASFLAGS         = $(FLAGS_ARCH)
CFLAGS_COMMON   = -std=c11 $(FLAGS_ARCH) $(INCLUDE) -MD -MP -Wall -Wextra -Werror -ffreestanding
CFLAGS_RELEASE  = -Os -DNDEBUG $(CFLAGS_COMMON)
CFLAGS_DEBUG    = -Og -g $(CFLAGS_COMMON)
LDFLAGS         = $(FLAGS_ARCH) -Wl,--nmagic -T $(LINKER_SCRIPT) -nostartfiles -nostdlib -nodefaultlibs -lgcc -Wl,--print-memory-usage
LDFLAGS_RELEASE = $(LDFLAGS) -s

SRC_C       = $(shell find src libraries -type f -name "*.c")
SRC_S       = $(shell find src libraries -type f -name "*.s")
OBJ_RELEASE = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_C)) $(patsubst %.s, $(BUILD_DIR)/%.o, $(SRC_S))
DEP_RELEASE = $(patsubst %.c, $(BUILD_DIR)/%.d, $(SRC_C)) $(patsubst %.s, $(BUILD_DIR)/%.d, $(SRC_S))
OBJ_DEBUG   = $(patsubst %.c, $(BUILD_DIR)/debug/%.o, $(SRC_C)) $(patsubst %.s, $(BUILD_DIR)/debug/%.o, $(SRC_S))
DEP_DEBUG   = $(patsubst %.c, $(BUILD_DIR)/debug/%.d, $(SRC_C)) $(patsubst %.s, $(BUILD_DIR)/debug/%.d, $(SRC_S))
TOOLS       = $(shell find tools -type d)
TOOLS_BUILD = $(patsubst %, $(BUILD_DIR)/%, $(TOOLS))

OPENOCD_CONFIG ?= firmware/device/$(TARGET_DEVICE)/openocd.cfg


all: release

release: $(ELF_RELEASE)

debug: $(ELF_DEBUG)

$(BUILD_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(TARGET_AS) $(ASFLAGS) -MD $(patsubst %o, %d, $@) -o $@ $<

$(BUILD_DIR)/debug/%.o: %.s
	mkdir -p $(dir $@)
	$(TARGET_AS) $(ASFLAGS) -MD $(patsubst %o, %d, $@) -o $@ $<

$(BUILD_DIR)/%.o: %.c | autogen
	mkdir -p $(dir $@)
	$(TARGET_CC) -c $(CFLAGS_RELEASE) -o $@ $<

$(BUILD_DIR)/debug/%.o: %.c | autogen
	mkdir -p $(dir $@)
	$(TARGET_CC) -c $(CFLAGS_DEBUG) -o $@ $<

$(BUILD_DIR)/tools/bin/%: tools/%
	mkdir -p $(dir $@)
	$(MAKE) BASE_DIR=$< BUILD_DIR=$(BUILD_DIR)/$< OUTPUT_DIR=$(BUILD_DIR)/tools/bin -f $</Makefile

$(BUILD_DIR)/include/gen/pb_blocks.h: $(BUILD_DIR)/tools/bin/pgm2h data/pb_blocks.pgm
	mkdir -p $(dir $@)
	$< data/pb_blocks.pgm 2 > $@

autogen: $(BUILD_DIR)/include/gen/pb_blocks.h

$(ELF_RELEASE): $(OBJ_RELEASE) $(LINKER_SCRIPT)
	$(TARGET_LD) -o $@ $(OBJ_RELEASE) $(LDFLAGS_RELEASE)

$(ELF_DEBUG): $(OBJ_DEBUG) $(LINKER_SCRIPT)
	$(TARGET_LD) -o $@ $(OBJ_DEBUG) $(LDFLAGS)

flash-debug: $(ELF_DEBUG)
	openocd -f $(OPENOCD_CONFIG) -c "program $(ELF_DEBUG) verify reset exit"

flash: $(ELF_RELEASE)
	openocd -f $(OPENOCD_CONFIG) -c "program $(ELF_RELEASE) verify reset exit"

clean:
	rm -rf $(BUILD_DIR) $(ELF_RELEASE) $(ELF_DEBUG)

$(BUILD_DIR)/cppcheck.log: $(SRC_C)
	cppcheck -I include --enable=all --force --quiet $^ 2> $@

gdb:
	gdb-multiarch $(ELF_DEBUG) -ex "target extended-remote | openocd -f $(OPENOCD_CONFIG) -c \"gdb_port pipe\""

cppcheck: $(BUILD_DIR)/cppcheck.log
	cat $<

size: $(ELF_RELEASE) $(ELF_DEBUG)
	$(TARGET_SIZE) $^

loc: $(SRC_C) $(SRC_ASM)
	wc -l $^

-include $(DEP_RELEASE) $(DEP_DEBUG)

.PHONY : all $(TOOLS_BUILD) release debug flash clean gdb cppcheck size loc
