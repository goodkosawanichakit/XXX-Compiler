CXX = g++ # C compiler
CXXFLAGS = -Wall -Wextra -Werror -g -Ikiwi/includes # Compiler flags
SRC_DIR = kiwi/src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET = $(BIN_DIR)/compiler

all: $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled: $<"

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned"

rebuild: clean all

run: $(TARGET)
	$(TARGET)

help:
	@echo "Available targets:"
	@echo "  make          - Build project"
	@echo "  make clean    - Remove compiled files"
	@echo "  make rebuild  - Clean and build"
	@echo "  make run      - Build and run"

.PHONY: all clean rebuild run help
