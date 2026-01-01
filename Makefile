SRC_DIR := ./src
INC_DIR := ./inc
LIB_DIR := ./lib
BUILD_DIR := ./build
BUILD_DIR_RELEASE := $(BUILD_DIR)/release
BUILD_DIR_DEBUG := $(BUILD_DIR)/debug

SOURCES := $(shell find $(SRC_DIR) -name '*.c')

OBJECTS_RELEASE := $(SOURCES:%.c=$(BUILD_DIR_RELEASE)/%.o)
OBJECTS_DEBUG   := $(SOURCES:%.c=$(BUILD_DIR_DEBUG)/%.o)

CC := gcc
CFLAGS_COMMON := -Wall -Wextra -I$(INC_DIR)

CFLAGS_RELEASE := $(CFLAGS_COMMON)

# Debug flags
CFLAGS_DEBUG := $(CFLAGS_COMMON) -g

TARGET := nhman
TARGET_DEBUG := $(TARGET)_debug

.PHONY: all
all: CFLAGS := $(CFLAGS_RELEASE)
all: $(TARGET)

.PHONY: debug
debug: CFLAGS := $(CFLAGS_DEBUG)
debug: $(TARGET_DEBUG)

$(TARGET): $(OBJECTS_RELEASE)
	$(CC) $^ -o $@
	cp $@ $(BUILD_DIR_RELEASE)/$@

$(TARGET_DEBUG): $(OBJECTS_DEBUG)
	$(CC) $^ -o $@
	cp $@ $(BUILD_DIR_DEBUG)/$@

$(BUILD_DIR_RELEASE)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_DEBUG)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET_DEBUG)
