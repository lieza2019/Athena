MV = mv
CP = cp
RM = rm
TOUCH = touch
MAKE = make

CC = /usr/bin/gcc
CFLAGS = -Wall -g -c
CFLAGS_YACC = -g -c
LD = /usr/bin/gcc
LDFLAGS =

YACC = bison
YFLAGS = -dy -Wconflicts-sr -Wconflicts-rr -Wcounterexamples -Wother
LEX = flex
LFLAGS = -l

athena : main.o mem.o misc.o symtbl.o type.o decl.o stmt.o lisp.o parse.o y.tab.o lex.yy.o
	$(LD) $(LDFLAGS) -o $@ $^

main.o : main.c athena.h
	$(CC) $(CFLAGS) $<
mem.o : mem.c ath_mem.h
	$(CC) $(CFLAGS) $<
misc.o : misc.c athena.h
	$(CC) $(CFLAGS) $<
symtbl.o : symtbl.c athena.h
	$(CC) $(CFLAGS) $<
type.o: type.c athena.h
	$(CC) $(CFLAGS) $<
decl.o : decl.c athena.h
	$(CC) $(CFLAGS) $<
stmt.o : stmt.c athena.h
	$(CC) $(CFLAGS) $<
lisp.o : lisp.c athena.h
	$(CC) $(CFLAGS) $<
parse.o : parse.c athena.h
	$(CC) $(CFLAGS) $<
y.tab.o : y.tab.c
	$(CC) $(CFLAGS_YACC) $<
y.tab.c : ath_parse.y athena.h
	$(YACC) $(YFLAGS) $<
lex.yy.o : lex.yy.c athena.h
	$(CC) $(CFLAGS_YACC) $<
lex.yy.c : ath_lex.l
	$(LEX) $(LFLAGS) $<

athena.h : ath_misc.h ath_mem.h ath_type.h ath_decl.h ath_stmt.h ath_symtbl.h
	$(TOUCH) $@

.PHONY : clean
clean:
	$(RM) -f ./a.out
	$(RM) -f ./*.o
	$(RM) -f ./*~
	$(RM) -f ./#*#
	$(RM) -f ./*.stackdump
	$(RM) -f ./lex.yy.c
	$(RM) -f ./y.tab.c ./y.tab.h
	$(RM) -f ./athena
