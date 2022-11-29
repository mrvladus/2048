BUILD_DIR = ./buld

prepare:
	mkdir -p $(BUILD_DIR)

build: prepare
	cc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o $(BUILD_DIR)/2048

run: build
	$(BUILD_DIR)/2048
	rm $(BUILD_DIR)/2048