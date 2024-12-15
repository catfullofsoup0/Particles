SRC_DIR := .
OBJ_DIR := .
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -fsanitize=thread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
CXXFLAGS := -g -Wall -fpermissive -std=c++17 -Iinclude
TARGET := Particles.out

# MacOS should compile this using homebrew's sfml
# brew install sfml
OS_NAME := $(shell uname -s)
ifeq ($(OS_NAME), Darwin)
	LDFLAGS += -L/opt/homebrew/lib
	CXXFLAGS += -I/opt/homebrew/include
endif

$(TARGET): $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<
run:
	./$(TARGET)

clean:
	rm $(TARGET) *.o
