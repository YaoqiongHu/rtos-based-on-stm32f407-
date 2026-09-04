# STM32F4 RTOS 演示工程
# 依赖:arm-none-eabi-gcc + openocd(ST-Link)

TARGET  = rtos.elf
BUILD   = build
CC      = arm-none-eabi-gcc
CFLAGS  = -mcpu=cortex-m4 -mthumb -std=gnu17 -Wall -Wextra -Iinclude
LDFLAGS = -mcpu=cortex-m4 -mthumb -T link.ld -nostdlib -nostartfiles
OPENOCD = openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

SRCS_C = $(wildcard src/*.c)
SRCS_S = $(wildcard src/*.s)
OBJS   = $(patsubst src/%.c,$(BUILD)/%.o,$(SRCS_C)) \
         $(patsubst src/%.s,$(BUILD)/%.o,$(SRCS_S))

all: $(TARGET)

$(TARGET): $(OBJS) link.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.s | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

flash: $(TARGET)
	$(OPENOCD) -c "program $(TARGET) verify reset exit"

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all flash clean
