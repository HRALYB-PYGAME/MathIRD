CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
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
	   rangeobject.cpp

# This transforms 'main.cpp' into 'build/main.o'
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.cpp=.o))

all: $(TARGET)

# Link the objects into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Rule for compiling .cpp to .o inside the build directory
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
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

.PHONY: all clean run crun