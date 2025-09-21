BUILD_DIR := build

CMAKE := cmake
MAKE_CMD := $(MAKE)

.PHONY: all clean

all:
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) ../otus_hws/hw1
	$(MAKE_CMD) -C $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
