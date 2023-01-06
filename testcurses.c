#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>

//stdlib gives srand(int seed) which sets a seed
//stdlib gives rand() which returns an int between -3345275 and +543252345

/**
 * Returns a number between 0 inclusive and max exclusive
 */
unsigned randInt(unsigned max) {
	return rand()%max;
}

void refreshScreen(struct params *params) {
	clear();

	//draw the whole gamestate

	/*


	*/
	refresh();
}

int main() {
	//initialization
	setlocale(LC_ALL,"");
	initscr();
	noecho();	//no more updating display when we get input
	curs_set(0);	//stops drawing the cursor position
	wtimeout(stdscr,0);	//if 2nd arg is 0, don't wait for input. If 1, wait. If 2, wait for enter
	keypad(stdscr, 1);	//this makes input easy to read
	start_color();
	#define CARD_FRONT 1
	init_pair(CARD_FRONT,COLOR_RED,COLOR_WHITE);
	init_pair(2,COLOR_WHITE,COLOR_RED);

	//code
	printw("Hello world? %lc\n", L'\u2663');
	attron(COLOR_PAIR(CARD_FRONT));
	printw("COLORFUL\n");
	attron(COLOR_PAIR(2));
	unsigned char ix, iy;
	for (ix=10; ix<=20;ix++) {
		for (iy=10;iy<=15;iy++) {
			mvprintw(iy,ix," ");
			//move(iy,ix);
			//printw(" ");
		}
	}


//	mvprintw(/*y*/10,/*x*/5,"Weird\n");
	attron(COLOR_PAIR(0));
//	printw("Hello world? %lc", L'U+2713');

	//parse file
	struct gamestate state;
	struct parserConfig config;
	initConfig(&config);
	parseCheck(&state,&config);
	//make params struct

	refreshScreen(params);
	while (1) {
		int c = getch();	//if wtimeout is 0, then return ERR if no input
		if (c==ERR);
		else if (KEY_BACKSPACE==c) {endwin();return 0;}
		else {
			handleKeyboardInput(c);
			//prints the params->output
			//remove the red box and put a 5
			refreshScreen(params);
		}
	}
	endwin();

	return 0;
}
/*
struct params {
	char source;
	int movenumber;
	struct gamestate *laststate;	// one undo
	struct stackOfGamestates *stack; //infinite undos
	struct gamestate *currentState;
};

void handleKeyboardInput(int c, struct params *params) {
	//store what we are currently selecting
	//if arrow key figure out how to change that
	//if space or enter or somethnig then do logic based on what we selected

	if (c=='f') {
		//try to move to foundations
		if (source==NOSOURCE) strcpy(params->output,"Invalid move");
	} else if (c=='w') {
		//try to move from waste
	} else if (c>'0'&&c<'8') {
		if (params->source == NOSOURCE) params->source = c-'0';	//c-'0' is a number between 1 and 7
		else {
			//try to move from source to tableau
			unsigned char chosenTableau = c-'1'; //between 0 and 6
			if (canMoveTableau)
			moveTableau(params->gamestate,params->source);
			params->source=NOSOURCE;
		}
	}
}

void printBlock() {
	//make a block red


void printCard(int x, int y, char card) {
	printBlock(x,y,x+3,y+3);
	mvprintw(y,x,"%d",rank);
	mvprintw(y,x+1,"%lw",getUnicodeSuit(suit));
	mvprintw(y+3,x+3,"%d",rank);
	mvprintw(y+3,x+2,"%lw",getUnicodeSuit(suit));
}
*/
