BUILD_DIR := build

CMAKE := cmake
MAKE_CMD := $(MAKE)

.PHONY: all clean

all:
	make clean
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && CC=clang CXX=clang++ $(CMAKE) ../otus_hws/hw2
	$(MAKE_CMD) -C $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
