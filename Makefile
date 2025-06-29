# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = -lcurl -lgumbo -lpthread
TARGET = crawler
SRC = main.cpp

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(TARGET) *.o

# Install dependencies (Ubuntu)
install-deps:
	sudo apt update
	sudo apt install -y build-essential libcurl4-openssl-dev libgumbo-dev

# Run the crawler
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean install-deps run