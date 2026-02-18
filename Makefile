CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.libs/raylib/include
# -L tells the linker where to look for the file, -Wl,-rpath tells the program where to look when it RUNS
LDFLAGS = -L./raylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wl,-rpath,./raylib/lib
BUILD_DIR = build
SRC_DIR = src
TARGET = $(BUILD_DIR)/MathIRD

SRCS = utils.cpp \
       tokenizer.cpp \
	   insightable.cpp \
       variable_value.cpp \
       insight.cpp \
       rangeobject.cpp \
       expressiontree.cpp \
	   constantnode.cpp \
	   variablenode.cpp \
	   functionnode.cpp \
	   expression.cpp \
	   operandnode.cpp \
       variable.cpp \
       term.cpp \
       button.cpp \
	   packet.cpp \
	   process.cpp \
       game_state.cpp \
	   main.cpp

OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -f output
	clear

cleanoutput:
	rm -f output

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