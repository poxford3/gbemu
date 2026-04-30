CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/sfml/lib \
          -L/opt/homebrew/opt/nativefiledialog-extended/lib

LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lnfd

SRC = $(wildcard src/*.cpp)
OUT = gbemu_app

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) $(LIBS)