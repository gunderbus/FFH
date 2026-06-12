CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra $(shell pkg-config --cflags sfml-all)
LDFLAGS := $(shell pkg-config --libs-only-L sfml-all)
LDLIBS := $(shell pkg-config --libs-only-l sfml-all)
TARGET := ffh

all: $(TARGET)

$(TARGET): main.cpp camera.hpp draw.hpp input_handler.hpp item.hpp tileset.hpp
	$(CXX) $(CXXFLAGS) main.cpp $(LDFLAGS) $(LDLIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
