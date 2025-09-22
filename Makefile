BUILD_DIR := build

CMAKE := cmake
MAKE_CMD := $(MAKE)

.PHONY: all clean run like_ci run_test

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

like_ci:
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	cd $(BUILD_DIR)
# 	cmake .. -DPATCH_VERSION=42 -DWITH_BOOST_TEST=ON
# 	cmake --build .
# 	cmake --build . --target test
# 	cmake --build . --target package
# 	cd -