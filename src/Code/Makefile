CC = g++
 
STD = -std=c++11
 
CFLAGS = -g -Wall 
 
SRC = $(wildcard *.cpp)
 
OBJ = $(patsubst %cpp, %o, $(SRC))
 
BIN = ../Release/CmCompiler

 
all:$(OBJ)
	$(CC) $(OBJ) -o $(BIN)
 
%.o:%.cpp
	$(CC) $(STD) $(CFLAGS) -c $< -o $@
 
 
.PHONY : clean
clean:
	del *.o  -rf
#windows下用del,linux下用rm