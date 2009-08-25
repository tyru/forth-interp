.PHONY: all test depend clean

CC       = gcc
INCLUDES =
CFLAGS   = $(INCLUDES) -g -Wall -W -pedantic -std=c99
LIBS     =

EXE      = myforth
SRC      = main.c forth.c util.c stack.c parser.c word.c
OBJS     = $(SRC:.c=.o)

OBJS_NOMAIN = `for i in $(OBJS); do echo $$i; done | grep -v 'main\.'`
TEST        = stack-test word-test



all: $(EXE)

# link
$(EXE): $(OBJS)
	$(CC) $(LIBS) -o $(EXE) $(OBJS) $(LIBS)

# compile
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# test
test: $(OBJS)
	for i in $(TEST); do $(CC) $(CFLAGS) t/$$i.c -o t/$$i $(OBJS_NOMAIN); echo "test '$$i' begin."; t/$$i; echo "test '$$i' end."; done

# depend
depend:
	\makedepend -- $(CFLAGS) -- $(SRC)

# clean
clean:
	\rm -f $(OBJS) $(EXE)
