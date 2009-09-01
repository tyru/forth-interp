.PHONY: all db full-test test leak-test depend clean

CC       = gcc
LIBS     =
INCLUDES =
# CFLAGS   = $(INCLUDES) -Wall -W -pedantic -std=gnu99 -O2    # release build
CFLAGS   = -DNDEBUG=1 $(INCLUDES) -g -Wall -W -pedantic -std=gnu99    # debug build

EXE      = myforth
SRC      = main.c forth.c util.c stack.c parser.c word.c token.c digit.c signal.c
OBJS     = $(SRC:.c=.o)

OBJS_NOMAIN = `for i in $(OBJS); do echo $$i; done | grep -v 'main\.o'`
TEST        = word-test stack-test parser-test util-test digit-test
TEST_DIR    = t



all: $(EXE)

# link
$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LIBS)

# compile
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


# db
db: $(EXE)
	\gdb ./$(EXE)


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
	echo -n "press enter to continue..."
	read
	valgrind --leak-check=full ./$(EXE)
	### tests
	echo "leak test of test codes..."
	echo -n "press enter to continue..."
	read
	for i in $(TEST); do valgrind --leak-check=full $(TEST_DIR)/$$i; done


# depend
depend:
	\makedepend -- $(CFLAGS) -- $(SRC)


# clean
clean:
	\rm -f $(OBJS) $(EXE)
# DO NOT DELETE

main.o: forth.h type.h word.h digit.h /usr/include/stdlib.h
main.o: /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
main.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
main.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/sys/select.h /usr/include/bits/select.h
main.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
main.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
main.o: /usr/include/alloca.h stack.h /usr/include/stdio.h
main.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
main.o: /usr/include/bits/wchar.h /usr/include/gconv.h
main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
main.o: signal.h util.h token.h /usr/include/string.h
forth.o: forth.h type.h word.h digit.h /usr/include/stdlib.h
forth.o: /usr/include/features.h /usr/include/sys/cdefs.h
forth.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
forth.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
forth.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
forth.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
forth.o: /usr/include/sys/select.h /usr/include/bits/select.h
forth.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
forth.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
forth.o: /usr/include/alloca.h stack.h /usr/include/stdio.h
forth.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
forth.o: /usr/include/bits/wchar.h /usr/include/gconv.h
forth.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
forth.o: constant.h token.h parser.h util.h /usr/include/string.h
forth.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
forth.o: /usr/include/bits/confname.h /usr/include/getopt.h
forth.o: /usr/include/ctype.h /usr/include/errno.h /usr/include/bits/errno.h
forth.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
forth.o: /usr/include/asm-generic/errno.h
forth.o: /usr/include/asm-generic/errno-base.h
util.o: util.h token.h type.h word.h digit.h /usr/include/stdlib.h
util.o: /usr/include/features.h /usr/include/sys/cdefs.h
util.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
util.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
util.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
util.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
util.o: /usr/include/sys/select.h /usr/include/bits/select.h
util.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
util.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
util.o: /usr/include/alloca.h /usr/include/string.h /usr/include/stdio.h
util.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
util.o: /usr/include/bits/wchar.h /usr/include/gconv.h
util.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
util.o: /usr/include/ctype.h /usr/include/errno.h /usr/include/bits/errno.h
util.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
util.o: /usr/include/asm-generic/errno.h
util.o: /usr/include/asm-generic/errno-base.h
stack.o: /usr/include/stdlib.h /usr/include/features.h
stack.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
stack.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
stack.o: /usr/include/sys/types.h /usr/include/bits/types.h
stack.o: /usr/include/bits/typesizes.h /usr/include/time.h
stack.o: /usr/include/endian.h /usr/include/bits/endian.h
stack.o: /usr/include/sys/select.h /usr/include/bits/select.h
stack.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
stack.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
stack.o: /usr/include/alloca.h /usr/include/string.h stack.h type.h util.h
stack.o: token.h word.h digit.h
parser.o: parser.h type.h /usr/include/stdio.h /usr/include/features.h
parser.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
parser.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
parser.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
parser.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
parser.o: /usr/include/bits/wchar.h /usr/include/gconv.h
parser.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
parser.o: util.h token.h word.h digit.h /usr/include/stdlib.h
parser.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
parser.o: /usr/include/bits/endian.h /usr/include/sys/select.h
parser.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
parser.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
parser.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
parser.o: /usr/include/string.h forth.h stack.h /usr/include/ctype.h
word.o: word.h type.h digit.h /usr/include/stdlib.h /usr/include/features.h
word.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
word.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
word.o: /usr/include/sys/types.h /usr/include/bits/types.h
word.o: /usr/include/bits/typesizes.h /usr/include/time.h
word.o: /usr/include/endian.h /usr/include/bits/endian.h
word.o: /usr/include/sys/select.h /usr/include/bits/select.h
word.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
word.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
word.o: /usr/include/alloca.h util.h token.h /usr/include/string.h forth.h
word.o: stack.h /usr/include/stdio.h /usr/include/libio.h
word.o: /usr/include/_G_config.h /usr/include/wchar.h
word.o: /usr/include/bits/wchar.h /usr/include/gconv.h
word.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
word.o: /usr/include/errno.h /usr/include/bits/errno.h
word.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
word.o: /usr/include/asm-generic/errno.h
word.o: /usr/include/asm-generic/errno-base.h
token.o: token.h type.h util.h word.h digit.h /usr/include/stdlib.h
token.o: /usr/include/features.h /usr/include/sys/cdefs.h
token.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
token.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
token.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
token.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
token.o: /usr/include/sys/select.h /usr/include/bits/select.h
token.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
token.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
token.o: /usr/include/alloca.h /usr/include/string.h /usr/include/ctype.h
digit.o: digit.h
signal.o: signal.h type.h util.h token.h word.h digit.h /usr/include/stdlib.h
signal.o: /usr/include/features.h /usr/include/sys/cdefs.h
signal.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
signal.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
signal.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
signal.o: /usr/include/time.h /usr/include/endian.h
signal.o: /usr/include/bits/endian.h /usr/include/sys/select.h
signal.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
signal.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
signal.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
signal.o: /usr/include/string.h forth.h stack.h /usr/include/stdio.h
signal.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
signal.o: /usr/include/bits/wchar.h /usr/include/gconv.h
signal.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
