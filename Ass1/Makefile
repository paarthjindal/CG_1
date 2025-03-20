# Simple Makefile for Marble Solitaire Game
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude
LDFLAGS = -lGL -lGLEW -lglfw

# ImGui source files
IMGUI_SRC = external/imgui/imgui.cpp \
	        external/imgui/imgui_demo.cpp \
	        external/imgui/imgui_draw.cpp \
	        external/imgui/imgui_tables.cpp \
	        external/imgui/imgui_widgets.cpp \
	        external/imgui/backends/imgui_impl_glfw.cpp \
	        external/imgui/backends/imgui_impl_opengl3_fix.cpp

# Project source files
SRC = src/main.cpp \
	  src/game.cpp \
	  src/renderer.cpp \
	  src/shader.cpp \
	  src/theme.cpp 

OBJ = $(SRC:.cpp=.o) $(IMGUI_SRC:.cpp=.o)

TARGET = marble_solitaire

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean