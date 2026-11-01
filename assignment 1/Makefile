# Makefile for the image search program (assignment 1)

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(shell pkg-config --cflags opencv4)
LDLIBS   = $(shell pkg-config --libs opencv4)

SRC_DIR = src
OBJ_DIR = obj
TARGET  = search

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

# -MMD -MP: also create .d files so a change in a header rebuilds the .cpp files that use it
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

-include $(OBJECTS:.o=.d)

.PHONY: all clean
