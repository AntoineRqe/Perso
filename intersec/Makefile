all: main

main: main.o list.o dict.o
		gcc -o main list.o dict.o main.o

list.o: list.c
		gcc -o list.o -c list.c -W -Wall -Werror

dict.o: dict.c
		gcc -o dict.o -c dict.c -W -Wall -Werror

main.o: main.c list.h dict.h
		gcc -o main.o -c main.c -W -Wall -Werror

clean:
		rm -rf *.o
		rm -rf *.exe
