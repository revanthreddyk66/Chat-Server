# Makefile for the Medium-Level Chat Server and Client
CXX = g++
# Use -std=c++17 for modern features and -pthread for std::thread/mutex
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Define targets
all: server client

# Rule to build the server
server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

# Rule to build the client
client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

# Rule to clean up build files
clean:
	rm -f server client

# Phony targets don't represent actual files
.PHONY: all clean
