#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char getNextC(FILE*);

/**
 * Takes in a filename and makes sure that stdin matches it exactly except 1 whitespace can be replaced by many or other kinds
 */
int main(int argc, char **argv) {
	if (argc!=2) {fprintf(stderr, "Usage: cunit filename\n");return 1;}
	FILE* correctFile = fopen(argv[1],"r");
	if (!correctFile) {fprintf(stderr, "No such file exists!\n");return 1;}

	while (1) {
		char c = getNextC(stdin);
		char correct = getNextC(correctFile);
		if (c!=correct) {printf("Test case failed! %s\n",argv[1]);return 0;}
		if (EOF==correct) break;
	}
	//printf("Test passed\n");
}

/**
 * Returns the next character in the file or ' ' if a whitespace, consuming every following whitespace also
 */
char getNextC(FILE *f) {
	char ws = 0;
	while (1) {
		char n = fgetc(f);
		switch(n) {
			case '\n':
			case '\r':
			case '\t':
			case ' ':
				ws = 1;
				continue;
			default:
				if (ws) {
					ungetc(n,f);
					return ' ';
				} else return n;
		}
	}
}
