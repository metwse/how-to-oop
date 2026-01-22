# Compile without any optimizations, but with maximum verbosity of debug
# symbols

CC = gcc
RM = rm -rf

CFLAGS = -std=gnu17 -Wall -Wextra -O0 -g3 -lm

SRC_DIR = src
TEST_DIR = tests
DIST_DIR = target

MAIN = main.c

OBJ_DIR = $(DIST_DIR)/obj
TEST_OBJ_DIR = $(DIST_DIR)/obj/test


# no need to change rules below this line
SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_SRCS))

LIB_OBJS = $(filter-out $(OBJ_DIR)/$(patsubst %.c,%.o,$(MAIN)),$(OBJS))

TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c,$(DIST_DIR)/%.test,$(TEST_SRCS))

default: $(DIST_DIR)/main

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIST_DIR)/%.test: $(TEST_OBJ_DIR)/%.o $(LIB_OBJS) | $(DIST_DIR)
	$(CC) $(CFLAGS)    $^ -o $@

$(DIST_DIR)/main: $(OBJS) | $(DIST_DIR)
	$(CC) $(CFLAGS)    $^ -o $@

$(DIST_DIR) $(OBJ_DIR) $(TEST_OBJ_DIR):
	mkdir -p $@

tests: $(TEST_TARGETS)

all: $(DIST_DIR)/main $(TEST_TARGETS)

clean:
	$(RM) $(DIST_DIR)

docs:
	doxygen

help:
	@echo "Available targets:"
	@echo "  make        - Build main executable"
	@echo "  make tests  - Build test suite"
	@echo "  make all    - Build main + tests"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make docs   - Generate documentation"


.SECONDARY: $(OBJS) $(TEST_OBJS)
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)

.PHONY: clean docs default all tests help
