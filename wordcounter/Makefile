all: wordcounter

wordcounter: wordcounter.o list.o toolbox.o
		gcc -o wordcounter list.o toolbox.o wordcounter.o

list.o: list.c
		gcc -o list.o -c list.c -W -Wall -Werror

toolbox.o: toolbox.c
		gcc -o toolbox.o -c toolbox.c -W -Wall -Werror

wordcounter.o: wordcounter.c list.h toolbox.h
		gcc -o wordcounter.o -c wordcounter.c -W -Wall -Werror

clean:
		rm -rf *.o
		rm -rf *.exe
