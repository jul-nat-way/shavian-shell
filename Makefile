CC := gcc
CFLAGS := -Wall -O2 -fms-extensions
SRC_DIR := 𐑕𐑹𐑕
BIN_DIR := 𐑚𐑦𐑯

SRCS := $(wildcard $(SRC_DIR)/*.c)

BINS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRCS))

all: $(BINS)

.PHONY: all clean

# Rule to build each individual executable inside the target directory
$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN_DIR)
sanitize: $(BINS)

.PHONY: all clean

# Rule to build each individual executable inside the target directory
$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -fsanitize=address -g $< -o $@
