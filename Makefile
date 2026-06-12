CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -I/opt/homebrew/opt/sfml/include
LDFLAGS := -L/opt/homebrew/opt/sfml/lib
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system
TARGET := ffh

all: $(TARGET)

$(TARGET): main.cpp camera.hpp draw.hpp input_handler.hpp item.hpp tileset.hpp
	$(CXX) $(CXXFLAGS) main.cpp $(LDFLAGS) $(LDLIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
