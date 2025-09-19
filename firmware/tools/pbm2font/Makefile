BASE_DIR   ?= .
OUTPUT_DIR ?= .
BUILD_DIR  ?= .build

BINARY  = $(OUTPUT_DIR)/pbm2font
INCLUDE = -I $(BASE_DIR)/include
CFLAGS  = -DNDEBUG -O3 $(INCLUDE) -Wall -Wextra -Werror -MD -MP
LDFLAGS = -s -lm

SRC = $(shell find $(BASE_DIR)/src -type f)
OBJ = $(patsubst $(BASE_DIR)/src/%.c, $(BUILD_DIR)/%.o, $(SRC))
DEP = $(patsubst $(BASE_DIR)/src/%.c, $(BUILD_DIR)/%.d, $(SRC))

PREFIX ?= $(DESTDIR)/usr/local
BINDIR ?= $(PREFIX)/bin

all : $(BINARY)

$(BUILD_DIR)/%.o: $(BASE_DIR)/src/%.c
	mkdir -p $(basename $@)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BINARY): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(BINARY)

install: $(BINARY)
	install -d $(BINDIR)
	install $(BINARY) $(BINDIR)

uninstall:
	rm -rf $(addprefix $(BINDIR)/,$(BINARY_RELEASE))

$(BUILD_DIR)/cppcheck.log: $(SRC)
	cppcheck -I include --enable=all --force --quiet $^ 2> $@

cppcheck: $(BUILD_DIR)/cppcheck.log
	cat $<

loc:
	wc -l $(SRC)

-include $(DEP)

.PHONY : all clean install uninstall cppcheck loc
