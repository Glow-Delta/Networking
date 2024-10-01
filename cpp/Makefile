# Variables
CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = server

# Source and Object files
SRC = main.cpp socket_creation.cpp socket_setup.cpp socket_handling.cpp
OBJ = $(SRC:.cpp=.o)

# External libraries (if any)
LIBS = -lpthread  # Example: Add any external libraries required

# Default target to build the program
all: clean $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJ)
	@rm -f $@  # Remove the old executable if it exists
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LIBS)

# Rule to compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Install necessary libraries using apt (Ubuntu/Debian)
setup:
	sudo apt install -y build-essential
	sudo apt install -y g++ libc-dev
	sudo apt install -y net-tools iproute2

# Run the compiled server program
run: $(TARGET)
	./$(TARGET)

# Clean the build directory by removing object files and the executable
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets (non-file targets)
.PHONY: all clean setup run
