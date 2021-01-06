CC = g++
TARGET = term 
SRC = main.cpp command_utils.cpp ls.cpp cd.cpp cp.cpp dirname.cpp mv.cpp
     
CFLAGS =
FLAGS =
LIBS =  -lpthread -lreadline


OBJ = $(SRC:%.cpp=%.o)

%.cpp.o: 
	$(CC) -c $(CFLAGS) $<  -o $@ 
$(TARGET): $(OBJ)
	$(CC) $(FLAGS)  -o $(TARGET) $(OBJ) $(LIBS)
clean:
	rm *.o $(TARGET) 