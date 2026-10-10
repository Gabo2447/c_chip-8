CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=build/%.o)

TARGET = read_rom
BUILD_DIR := ./build

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

# Pattern Rules
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf build/

.PHONY: all clean