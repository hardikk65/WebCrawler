# Define variables
CXX = g++
CXXFLAGS = -lcurl -lgumbo
TARGET = crawler
SRCS = main.cpp 


# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) -o $(TARGET)

# Rule to clean up generated files
clean:
	rm -f $(TARGET) *.o