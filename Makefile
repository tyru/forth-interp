.PHONY: all full-test test leak-test depend clean

NDEBUG   = -DNDEBUG=1

CC       = gcc
INCLUDES =
CFLAGS   = $(NDEBUG) $(INCLUDES) -g -Wall -W -pedantic -std=c99
LIBS     =

EXE      = myforth
SRC      = main.c forth.c util.c stack.c parser.c word.c token.c
OBJS     = $(SRC:.c=.o)

OBJS_NOMAIN = `for i in $(OBJS); do echo $$i; done | grep -v 'main\.o'`
TEST        = word-test stack-test parser-test util-test
TEST_DIR    = t



all: $(EXE)

# link
$(EXE): $(OBJS)
	$(CC) $(LIBS) -o $(EXE) $(OBJS) $(LIBS)

# compile
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


# full-test
full-test: test leak-test

# test
test: $(EXE)
	for i in $(TEST); do rm -f t/$$i; done    # delete all previous tests
	(for i in $(TEST); do $(CC) $(CFLAGS) $(TEST_DIR)/$$i.c -o $(TEST_DIR)/$$i $(OBJS_NOMAIN); echo "test '$$i' begin."; $(TEST_DIR)/$$i || exit -1; echo "test '$$i' end."; done) && echo "＼(＾o＾)／: all test was successful."

# leak-test
leak-test:
	### $(EXE)
	echo "leak test of $(EXE)..."
	sleep 1
	valgrind --leak-check=full ./$(EXE)
	### tests
	echo "leak test of test codes..."
	sleep 1
	for i in $(TEST); do valgrind --leak-check=full $(TEST_DIR)/$$i; done

# depend
depend:
	\makedepend -- $(CFLAGS) -- $(SRC)

# clean
clean:
	\rm -f $(OBJS) $(EXE)
