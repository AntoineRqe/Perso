CC=gcc
RM=rm -rf

TGT_NAME=wordcounter

CC_FLAGS=-g -W -Wall -Werror

.PHONY=all clean

all: wordcounter

wordcounter: wordcounter.o list.o toolbox.o
		$(CC) -o $@ list.o toolbox.o wordcounter.o

list.o: list.c
		$(CC) $(CC_FLAGS) -o $@ -c $^

toolbox.o: toolbox.c
		$(CC) $(CC_FLAGS) -o $@ -c $^

wordcounter.o: wordcounter.c list.h toolbox.h
		$(CC) $(CC_FLAGS) -o $@ -c wordcounter.c

utest-list: utest-list.c list.c list.h
	$(CC) $(CC_FLAGS) -o $@ $^

clean:
		$(RM) *.o
		$(RM) *.exe
		$(RM) *.gcno *.gcda
		$(RM) $(TGT_NAME)
