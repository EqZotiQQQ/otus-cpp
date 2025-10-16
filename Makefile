BUILD_DIR := build

CMAKE := cmake
MAKE_CMD := $(MAKE)

.PHONY: all clean run doxygen

all:
	make clean
	@mkdir -p $(BUILD_DIR)
	CC=clang CXX=clang++ $(CMAKE) -DPATCH_VERSION=42 -DWITH_BOOST_TEST=ON -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR) 
	$(CMAKE)  --build $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	rm -rf docs

run:
	$(BUILD_DIR)/bin/ip_filter $(BUILD_DIR)/assets/ip_filter.tsv

doxygen:
	doxygen Doxyfile
