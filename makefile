CXX = g++
CXXFLAGS = -std=c++17 -Iinclude

SRC = $(wildcard src/*.cpp)
OUT = gbemu_app

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)