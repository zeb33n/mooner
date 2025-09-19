TARGET_DEVICE   = gameblaster
TARGET_REVISION = 8
PROGRAM_NAME    = gbfw

BUILD_DIR  ?= .build
OUTPUT_DIR ?= .

ELF_RELEASE   = $(OUTPUT_DIR)/$(PROGRAM_NAME)-$(TARGET_DEVICE).elf
ELF_DEBUG     = $(OUTPUT_DIR)/$(PROGRAM_NAME)-$(TARGET_DEVICE)-debug.elf
LINKER_SCRIPT = device/$(TARGET_DEVICE)/firmware.ld

CROSS_COMPILE = arm-none-eabi-
TARGET_CC     = $(CROSS_COMPILE)gcc
TARGET_LD     = $(CROSS_COMPILE)gcc
TARGET_SIZE   = $(CROSS_COMPILE)size

INCLUDE        = -I include -I $(BUILD_DIR)/include -I api
FLAGS_ARCH     = -mlittle-endian -mcpu=cortex-m4
ASFLAGS        = $(FLAGS_ARCH)
CFLAGS_COMMON  = -flto -std=c11 $(FLAGS_ARCH) $(INCLUDE) -MD -MP -Wall -Wextra -Werror -ffreestanding -DFIRMWARE -DREVISION=$(TARGET_REVISION)
CFLAGS_RELEASE = -Os -DNDEBUG $(CFLAGS_COMMON)
CFLAGS_DEBUG   = -Og -g $(CFLAGS_COMMON)
LDFLAGS        = $(FLAGS_ARCH) -flto -Wl,--nmagic,--print-memory-usage -T $(LINKER_SCRIPT) -nostartfiles -nostdlib -nodefaultlibs -lgcc

SRC         = $(shell find src -type f -name "*.c")
OBJ_RELEASE = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRC))
DEP_RELEASE = $(patsubst src/%.c, $(BUILD_DIR)/%.d, $(SRC))
OBJ_DEBUG   = $(patsubst src/%.c, $(BUILD_DIR)/debug/%.o, $(SRC))
DEP_DEBUG   = $(patsubst src/%.c, $(BUILD_DIR)/debug/%.d, $(SRC))
TOOLS       = $(shell find tools -type d)
TOOLS_BUILD = $(patsubst %, $(BUILD_DIR)/%, $(TOOLS))

OPENOCD_CONFIG ?= device/$(TARGET_DEVICE)/openocd.cfg


all: release

release: $(ELF_RELEASE)

debug: $(ELF_DEBUG)

$(BUILD_DIR)/%.o: src/%.c | autogen
	mkdir -p $(dir $@)
	$(TARGET_CC) -c $(CFLAGS_RELEASE) -o $@ $<

$(BUILD_DIR)/debug/%.o: src/%.c | autogen
	mkdir -p $(dir $@)
	$(TARGET_CC) -c $(CFLAGS_DEBUG) -o $@ $<

$(BUILD_DIR)/tools/bin/%: tools/%
	mkdir -p $(dir $@)
	$(MAKE) BASE_DIR=$< BUILD_DIR=$(BUILD_DIR)/$< OUTPUT_DIR=$(BUILD_DIR)/tools/bin -f $</Makefile

$(BUILD_DIR)/include/gen/logo_top.h: $(BUILD_DIR)/tools/bin/pbm2h data/logo_top.pbm
	mkdir -p $(dir $@)
	$< data/logo_top.pbm logo_top > $@

$(BUILD_DIR)/include/gen/logo_bottom.h: $(BUILD_DIR)/tools/bin/pbm2h data/logo_bottom.pbm
	mkdir -p $(dir $@)
	$< data/logo_bottom.pbm logo_bottom > $@

$(BUILD_DIR)/include/gen/font_default.h: $(BUILD_DIR)/tools/bin/pbm2font data/font_default.pbm
	mkdir -p $(dir $@)
	$< data/font_default.pbm 5 9 16 8 1 1 27 127 gbfw default > $@

$(BUILD_DIR)/include/gen/font_small.h: $(BUILD_DIR)/tools/bin/pbm2font data/font_small.pbm
	mkdir -p $(dir $@)
	$< data/font_small.pbm 3 5 16 8 1 1 27 127 gbfw small > $@

autogen: $(BUILD_DIR)/include/gen/logo_top.h $(BUILD_DIR)/include/gen/logo_bottom.h $(BUILD_DIR)/include/gen/font_default.h $(BUILD_DIR)/include/gen/font_small.h

$(ELF_RELEASE): $(OBJ_RELEASE) $(LINKER_SCRIPT)
	$(TARGET_LD) $(LDFLAGS) -o $@ $(OBJ_RELEASE)

$(ELF_DEBUG): $(OBJ_DEBUG) $(LINKER_SCRIPT)
	$(TARGET_LD) $(LDFLAGS) -o $@ $(OBJ_DEBUG)

flash-debug: $(ELF_DEBUG)
	openocd -f $(OPENOCD_CONFIG) -c "program $(ELF_DEBUG) verify reset exit"

flash: $(ELF_RELEASE)
	openocd -f $(OPENOCD_CONFIG) -c "program $(ELF_RELEASE) verify reset exit"

clean:
	rm -rf $(BUILD_DIR) $(ELF_RELEASE) $(ELF_DEBUG)

$(BUILD_DIR)/cppcheck.log: $(SRC)
	cppcheck $(INCLUDE) --enable=all --force --quiet $^ 2> $@

gdb:
	gdb-multiarch $(ELF_DEBUG) -ex "target extended-remote | openocd -f $(OPENOCD_CONFIG) -c \"gdb_port pipe\""

cppcheck: $(BUILD_DIR)/cppcheck.log
	cat $<

size: $(ELF_RELEASE) $(ELF_DEBUG)
	$(TARGET_SIZE) $^

loc: $(SRC)
	wc -l $^

-include $(DEP_RELEASE) $(DEP_DEBUG)

.PHONY : all $(TOOLS_BUILD) release debug flash clean gdb cppcheck size loc
