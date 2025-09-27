# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iicalendarlib

# Source files
SRC = main.cpp $(wildcard icalendarlib/*.cpp)
OBJ = $(SRC:.cpp=.o)

# Output binary
TARGET = calendar_app

# Default rule
all: $(TARGET)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Run a test example (outputs to text file)
test: $(TARGET)
	./$(TARGET) icalendarlib/calendar.ics test_output.json list 20250101T000000 20251231T235959

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(TARGET) *.o *.txt

# Phony targets
.PHONY: all clean test
