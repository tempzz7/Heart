
CC = gcc
CFLAGS = -Wall -g

SRC = main.c screen.c keyboard.c timer.c
OBJ = main.o screen.o keyboard.o timer.o

TARGET = heart_program

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

screen.o: screen.c
	$(CC) $(CFLAGS) -c screen.c

keyboard.o: keyboard.c
	$(CC) $(CFLAGS) -c keyboard.c

timer.o: timer.c
	$(CC) $(CFLAGS) -c timer.c

clean:
	rm -f $(OBJ) $(TARGET)