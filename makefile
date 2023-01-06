CC=gcc
CFLAGS=-Wall -pedantic

all: check advance winnable game

game: display.o parser.o moves.o thiccHelper.o smallHelper.o tinyHelper.o simplicity.o randState.o
	$(CC) $(CFLAGS) -o game display.o parser.o moves.o thiccHelper.o smallHelper.o tinyHelper.o simplicity.o randState.o -lncurses;

winnable: Part3.o parser.o moves.o thiccHelper.o smallHelper.o tinyHelper.o simplicity.o linkedQueue.o charStack.o bfHashtable.o
	$(CC) $(CFLAGS) -o winnable Part3.o parser.o moves.o thiccHelper.o smallHelper.o tinyHelper.o simplicity.o linkedQueue.o charStack.o bfHashtable.o;

advance: Part2.o parser.o moves.o thiccHelper.o smallHelper.o tinyHelper.o simplicity.o
	$(CC) $(CFLAGS) -o advance Part2.o parser.o moves.o thiccHelper.o smallHelper.o tinyHelper.o simplicity.o;

check: Part1.o parser.o tinyHelper.o smallHelper.o thiccHelper.o moves.o simplicity.o
	$(CC) $(CFLAGS) -o check Part1.o parser.o tinyHelper.o smallHelper.o thiccHelper.o moves.o simplicity.o;

display.o: display.c parser.h simplicity.h thiccHelper.h
	$(CC) $(CFLAGS) -c display.c -lncurses;

Part3.o: Part3.c parser.h simplicity.h thiccHelper.h tinyHelper.h moves.h dataStructures/linkedQueue.h dataStructures/charStack.h dataStructures/bfHashtable.h
	$(CC) $(CFLAGS) -c Part3.c;

Part2.o: Part2.c parser.h simplicity.h thiccHelper.h
	$(CC) $(CFLAGS) -c Part2.c;

Part1.o: Part1.c parser.h simplicity.h
	$(CC) $(CFLAGS) -c Part1.c;

parser.o: parser.c parser.h moves.h tinyHelper.h smallHelper.h thiccHelper.h simplicity.h
	$(CC) $(CFLAGS) -c parser.c;

tinyHelper.o: tinyHelper.c tinyHelper.h
	$(CC) $(CFLAGS) -c tinyHelper.c;

smallHelper.o: smallHelper.c smallHelper.h tinyHelper.h
	$(CC) $(CFLAGS) -c smallHelper.c;

thiccHelper.o: thiccHelper.c thiccHelper.h tinyHelper.h simplicity.h
	$(CC) $(CFLAGS) -c thiccHelper.c;

moves.o: moves.c moves.h tinyHelper.h thiccHelper.h simplicity.h
	$(CC) $(CFLAGS) -c moves.c;

simplicity.o: simplicity.h simplicity.c
	$(CC) $(CFLAGS) -c simplicity.c;

bfHashtable.o: dataStructures/bfHashtable.h dataStructures/bfHashtable.c simplicity.h tinyHelper.h
	$(CC) $(CFLAGS) -c dataStructures/bfHashtable.c

charStack.o: dataStructures/charStack.h dataStructures/charStack.c
	$(CC) $(CFLAGS) -c dataStructures/charStack.c

linkedQueue.o: dataStructures/linkedQueue.h dataStructures/linkedQueue.c
	$(CC) $(CFLAGS) -c dataStructures/linkedQueue.c

cunit: cunit.c
	$(CC) $(CFLAGS) -o cunit cunit.c;

randstate: randState.o thiccHelper.o tinyHelper.o simplicity.o moves.o
	$(CC) $(CFLAGS) -o randstate randState.o thiccHelper.o tinyHelper.o simplicity.o moves.o

randState.o: randState.c thiccHelper.h tinyHelper.h simplicity.h moves.h randState.h
	$(CC) $(CFLAGS) -c randState.c

test: all cunit
	./advance test/p2/testmoves1.txt | ./cunit test/res/res1
	./advance foo.txt test/p2/testmoves1.txt -m 43 -m 15 | ./cunit test/res/res2
	./advance test/p2/testmoves2.txt | ./cunit test/res/res3
	./advance test/p2/testmoves2.txt -m 0 | ./cunit test/res/res4
	./advance test/p2/testmoves2.txt -m 1 -x | ./cunit test/res/res5
	./advance test/p2/magnustest1.txt -m 75 -x | ./cunit test/res/res6
	./advance test/p2/magnustest2.txt | ./cunit test/res/res7
	./advance test/p2/codytest1.txt | ./cunit test/res/res8
	./winnable test/p3/testwin3.txt | ./cunit test/res/res9
	./winnable test/p3/testwin1.txt | ./cunit test/res/res10

clean:
	-rm -f *.o check advance winnable cunit randstate
