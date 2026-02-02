# Makefile for STM32 Virtual Simulation Tests
# Builds and runs virtual drivers for testing without hardware

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I.
LDFLAGS = -lm

# Source files
SRC_DIR = .
BUILD_DIR = build

# Simulation sources
SIM_SRCS = sim_adc.c sim_gpio.c sim_nvic.c sim_hal_wrapper.c

# Object files
SIM_OBJS = $(SIM_SRCS:%.c=$(BUILD_DIR)/%.o)

# Executables
TARGETS = $(BUILD_DIR)/test_adc \
          $(BUILD_DIR)/test_gpio \
          $(BUILD_DIR)/test_nvic \
          $(BUILD_DIR)/test_hal_wrapper

# Default target
all: $(BUILD_DIR) $(TARGETS)
	@echo "=== Build Complete ==="
	@echo "Available test executables:"
	@for target in $(TARGETS); do echo "  - $$target"; done

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile individual simulation files
$(BUILD_DIR)/sim_adc.o: sim_adc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/sim_gpio.o: sim_gpio.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/sim_nvic.o: sim_nvic.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/sim_hal_wrapper.o: sim_hal_wrapper.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executables
$(BUILD_DIR)/test_adc: sim_adc.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/test_gpio: sim_gpio.c
	$(CC) $(CFLAGS) -DRUN_STANDALONE_TEST $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/test_nvic: sim_nvic.c
	$(CC) $(CFLAGS) -DRUN_STANDALONE_TEST $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/test_hal_wrapper: sim_hal_wrapper.c sim_gpio.c sim_nvic.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run all tests
test: all
	@echo ""
	@echo "==================================="
	@echo "Running ADC Simulation Test"
	@echo "==================================="
	@$(BUILD_DIR)/test_adc
	@echo ""
	@echo "==================================="
	@echo "Running GPIO Simulation Test"
	@echo "==================================="
	@$(BUILD_DIR)/test_gpio
	@echo ""
	@echo "==================================="
	@echo "Running NVIC Simulation Test"
	@echo "==================================="
	@$(BUILD_DIR)/test_nvic
	@echo ""
	@echo "==================================="
	@echo "Running HAL Wrapper Test"
	@echo "==================================="
	@$(BUILD_DIR)/test_hal_wrapper
	@echo ""
	@echo "==================================="
	@echo "All Tests Complete!"
	@echo "==================================="

# Run individual tests
test-adc: $(BUILD_DIR)/test_adc
	@echo "Running ADC test..."
	@$(BUILD_DIR)/test_adc

test-gpio: $(BUILD_DIR)/test_gpio
	@echo "Running GPIO test..."
	@$(BUILD_DIR)/test_gpio

test-nvic: $(BUILD_DIR)/test_nvic
	@echo "Running NVIC test..."
	@$(BUILD_DIR)/test_nvic

test-hal: $(BUILD_DIR)/test_hal_wrapper
	@echo "Running HAL wrapper test..."
	@$(BUILD_DIR)/test_hal_wrapper

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned"

# Show help
help:
	@echo "STM32 Virtual Simulation Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build all test executables (default)"
	@echo "  test          - Build and run all tests"
	@echo "  test-adc      - Run ADC simulation test"
	@echo "  test-gpio     - Run GPIO simulation test"
	@echo "  test-nvic     - Run NVIC simulation test"
	@echo "  test-hal      - Run HAL wrapper test"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make all      # Build all tests"
	@echo "  make test     # Build and run all tests"
	@echo "  make test-gpio # Run only GPIO test"
	@echo "  make clean    # Clean build directory"

.PHONY: all test test-adc test-gpio test-nvic test-hal clean help
