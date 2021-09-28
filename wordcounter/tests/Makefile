# Unit-test Makefile
#--------------------------------------------------------- Definitions
TGT1_NAME    =    list
TGT1_SRC     =    ../src/list.c

TGT2_NAME    =    toolbox
TGT2_SRC     =    ../src/toolbox.c ../src/list.c

OPT         =    -g -fprofile-arcs -ftest-coverage
ERR         =    -Wall -Werror
INC_PATH    =    -I. -I../src/
LIB_PATH    =    -L..
LD_PATH     =     ..

#---------------------------------------------------------
CC          =    gcc
RM          =    rm -f

# NOTE: check libs must be enclosed by --whole-archive directives
CHECK_LIBS  =    -Wl,--whole-archive -lcheck -Wl,--no-whole-archive 
LIBS        =    $(CHECK_LIBS) -lm -lsubunit -lrt -lgcov -lpthread

# NOTE: UNIT_TEST enables the static-function test case in stuff.c
CC_FLAGS    =     $(INC_PATH) $(OPT) $(ERR) -DUNIT_TEST
# NOTE: check libs must be enclosed by --whole-archive directives
LD_FLAGS    =    $(LIB_PATH)


# Test Definitions (to be added later)
TESTS       = utest-list utest-toolbox
TEST1       = utest-list
TEST1_SRC   = utest-list.c $(TGT1_SRC)
TEST2       = utest-toolbox
TEST2_SRC   = utest-toolbox.c $(TGT2_SRC)

TESTS_FLAGS = $(CC_FLAGS) $(LD_FLAGS)
TESTS_LIBS  = $(LIBS)

#---------------------------------------------------------- Targets
all: $(TESTS)
.PHONY: all clean check


clean:
	$(RM) $(TESTS) *.gcno *.gcda


$(TEST1): $(TEST1_SRC)
	$(CC) $(TESTS_FLAGS) -o $@ $^ $(TESTS_LIBS)

$(TEST2): $(TEST2_SRC)
	$(CC) $(TESTS_FLAGS) -o $@ $^ $(TESTS_LIBS)

check:
	@for t in $(TESTS); do                          \
		LD_LIBRARY_PATH='$(LD_PATH)' ./$$t;     \
	done