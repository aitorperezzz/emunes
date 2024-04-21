CC=g++
CFLAGS := -g -Wall -Werror -std=c++17 -fsanitize=address -I./src
SOURCES := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))
TARGET := emunes

.phony: all clean

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPENDS)

-include $(DEPENDS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

src/%.o: src/%.cpp Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@