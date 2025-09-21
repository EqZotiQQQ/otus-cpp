BUILD_DIR := build

CMAKE := cmake
MAKE_CMD := $(MAKE)

.PHONY: all clean run

all:
	make clean
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && CC=clang CXX=clang++ $(CMAKE) ../otus_hws/hw2
	$(MAKE_CMD) -C $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

run:
	$(BUILD_DIR)/bin/ip_filter $(BUILD_DIR)/assets/ip_filter.tsv
run_test:
	$(BUILD_DIR)/bin/ip_filter $(BUILD_DIR)/assets/ip_filter_sample.tsv