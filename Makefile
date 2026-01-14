CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I./raylib/include
# -L tells the linker where to look for the file, -Wl,-rpath tells the program where to look when it RUNS
LDFLAGS = -L./raylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,./raylib/lib
BUILD_DIR = build
TARGET = $(BUILD_DIR)/MathIRD

SRCS = main.cpp \
       game_state.cpp \
       variable.cpp \
       variable_value.cpp \
       tokenizer.cpp \
       expressiontree.cpp \
       insight.cpp \
       term.cpp \
       utils.cpp \
       rangeobject.cpp \
       button.cpp \
	   insightable.cpp

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	clear

run: $(TARGET)
	./$(TARGET)

crun: $(TARGET)
	clear
	./$(TARGET)

remake:
	make clean
	make

rerun:
	make clean
	make run

.PHONY: all clean run crun