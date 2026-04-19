#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ncurses.h>

char *levels[]={
}

typedef enum {
    GAME_STATE_CODE=0,
    GAME_STATE_RUN,
    GAME_STATE_MAX
} GameState;

GameState gamestate=GAME_STATE_CODE;

typedef struct {
    int w,h,x,y,d,g;
    int *colors;
    bool *stars;
} Board;

typedef struct {
    int w,h,x,y,px,py;
    int *colors;
    int *instruction;
} Code;

bool quit=false;
int key=0;

int kbhit(void) {
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

void init() {
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	nodelay(stdscr, TRUE); 
	
	start_color();

	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(2,COLOR_WHITE,COLOR_RED);
	init_pair(3,COLOR_WHITE,COLOR_GREEN);
	init_pair(4,COLOR_WHITE,COLOR_BLUE);
	init_pair(5,COLOR_YELLOW,COLOR_BLACK);
	init_pair(6,COLOR_YELLOW,COLOR_RED);
	init_pair(7,COLOR_YELLOW,COLOR_GREEN);
	init_pair(8,COLOR_YELLOW,COLOR_BLUE);
}

void cleanup() {
	endwin();
}

void show(int u,int v) {
}

void code(int u,int v) {
}

int main(void) {
    init();
    cleanup();
    return 0;
}
