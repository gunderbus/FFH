CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iheaders $(shell pkg-config --cflags sfml-all)
LDFLAGS := $(shell pkg-config --libs-only-L sfml-all)
LDLIBS := $(shell pkg-config --libs-only-l sfml-all)
TARGET := ffh
EDITOR_TARGET := tileset_editor

all: $(TARGET) $(EDITOR_TARGET)

$(TARGET): main.cpp headers/camera.hpp headers/draw.hpp headers/input_handler.hpp headers/item.hpp headers/tileset.hpp
	$(CXX) $(CXXFLAGS) main.cpp $(LDFLAGS) $(LDLIBS) -o $(TARGET)

$(EDITOR_TARGET): tileset_editor.cpp
	$(CXX) $(CXXFLAGS) tileset_editor.cpp $(LDFLAGS) $(LDLIBS) -o $(EDITOR_TARGET)

clean:
	rm -f $(TARGET) $(EDITOR_TARGET)

.PHONY: all clean
