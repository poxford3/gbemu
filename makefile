CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/sfml/lib

LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = $(wildcard src/*.cpp)
OUT = gbemu_app

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) $(LIBS)