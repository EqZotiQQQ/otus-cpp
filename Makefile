BUILD_DIR := build

CMAKE := cmake
MAKE_CMD := $(MAKE)
DOCKER_IMAGE_NAME := otus_docker
CONTAINER_NAME := otus_container

.PHONY: all allc clean run doxygen s r up a b rm

all:
	$(CMAKE) -DPATCH_VERSION=42 -DWITH_BOOST_TEST=ON -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	$(CMAKE) --build $(BUILD_DIR) -- -j 16

allc:
	$(MAKE) clean
	@mkdir -p $(BUILD_DIR)
	$(CMAKE) -DPATCH_VERSION=42 -DWITH_BOOST_TEST=ON -DCMAKE_BUILD_TYPE=Debug -B $(BUILD_DIR)
	$(CMAKE) --build $(BUILD_DIR) -- -j 16

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	rm -rf docs

# run:
# 	$(BUILD_DIR)/bin/ip_filter $(BUILD_DIR)/assets/ip_filter.tsv

doxygen:
	doxygen Doxyfile

s:
	docker stop $(CONTAINER_NAME)

rm:
	docker rm $(CONTAINER_NAME)
r:
	docker run -d --name $(CONTAINER_NAME) \
	  -v $(HOME)/my_projects/otus-cpp:/app \
	  -v $(HOME)/.vscode/extensions:/root/.vscode-server/extensions \
	  $(DOCKER_IMAGE_NAME) tail -f /dev/null
# 	  --network otus-cpp_chatnet \

up:
	docker start $(CONTAINER_NAME)

a:
	docker attach $(CONTAINER_NAME)

b:
	docker build -f Dockerfile -t $(DOCKER_IMAGE_NAME) .
