# Makefile for the image search program (assignment 1)
#   make        -> optimized program "search" (for experiments)
#   make debug  -> program "search_debug" with debug info, no optimization (for gdb / VS Code)

CXX         = g++
COMMONFLAGS = -std=c++17 -Wall -Wextra $(shell pkg-config --cflags opencv4)
CXXFLAGS    = $(COMMONFLAGS) -O2
DEBUGFLAGS  = $(COMMONFLAGS) -O0 -g
LDLIBS      = $(shell pkg-config --libs opencv4)

SRC_DIR       = src
OBJ_DIR       = obj
DEBUG_OBJ_DIR = obj_debug
TARGET        = search
DEBUG_TARGET  = search_debug

SOURCES       = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS       = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEBUG_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(DEBUG_OBJ_DIR)/%.o,$(SOURCES))

all: $(TARGET)

debug: $(DEBUG_TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CXX) $(DEBUGFLAGS) -o $@ $^ $(LDLIBS)

# -MMD -MP: also create .d files so a change in a header rebuilds the .cpp files that use it
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(DEBUG_OBJ_DIR)
	$(CXX) $(DEBUGFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(DEBUG_OBJ_DIR) $(TARGET) $(DEBUG_TARGET)

-include $(OBJECTS:.o=.d) $(DEBUG_OBJECTS:.o=.d)

.PHONY: all debug clean
