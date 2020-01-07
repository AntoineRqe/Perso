CFLAGS=-W -Wall -Wextra -fdiagnostics-color -std=c99
EXEC=white

all: $(EXEC)

white: white.o
	gcc -g -pg -o white white.o $(LDFLAGS)

white.o: white.c
	$(CC) -g -pg -c white.c $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
