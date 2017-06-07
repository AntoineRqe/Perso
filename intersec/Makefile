all: main

main: main.o list.o
		gcc -o main list.o main.o

list.o: list.c
		gcc -o list.o -c list.c -W -Wall -Werror

main.o: main.c list.h
		gcc -o main.o -c main.c -W -Wall -Werror

clean:
		rm -rf *.o
		rm -rf *.exe
