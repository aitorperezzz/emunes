CC := g++

TARGET := emunes
TEST_TARGET := emunestest

CFLAGS := -g -Wall -Werror -std=c++17 -fsanitize=address -I./src
TEST_CFLAGS := $(CFLAGS)
TEST_LDFLAGS := -lcppunit

SOURCES := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

TEST_SOURCES := $(wildcard test/*.cpp) $(SOURCES)
TEST_SOURCES := $(filter-out src/main.cpp, $(TEST_SOURCES))
TEST_OBJECTS := $(patsubst %.cpp,%.o,$(TEST_SOURCES))
TEST_DEPENDS := $(patsubst %.cpp,%.d,$(TEST_SOURCES))

.phony: all clean test

all: $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

-include $(DEPENDS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

-include $(DEPENDS)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_CFLAGS) $(TEST_LDFLAGS) $(TEST_OBJECTS) -o $(TEST_TARGET)

src/%.o: src/%.cpp Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

test/%.o: test/%.cpp Makefile
	$(CC) $(TEST_CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPENDS) $(TEST_OBJECTS) $(TEST_TARGET) $(TEST_DEPENDS)