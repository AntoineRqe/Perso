CFLAGS=-W -Wall  -Wextra -fdiagnostics-color -std=c99
EXEC=odd-occurences

all: $(EXEC)

odd-occurences: odd-occurences.o
	gcc -g -o odd-occurences odd-occurences.o $(LDFLAGS)

odd-occurences.o: odd-occurences.c
	$(CC) -g -c odd-occurences.c $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
