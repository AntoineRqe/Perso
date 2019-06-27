CFLAGS=-W -Wall -Werror -Wextra -Wfatal-errors -fdiagnostics-color -std=c11 -D_BSD_SOURCE
LDFLAGS=-lyajl
EXEC=file-parser

all: $(EXEC)

file-parser: file-parser.o
	gcc -o file-parser file-parser.o $(LDFLAGS)

file-parser.o: file-parser.c
	$(CC) -c file-parser.c $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
