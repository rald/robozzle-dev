#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

typedef enum {
    GAME_STATE_CODE=0,
    GAME_STATE_RUN,
    GAME_STATE_MAX
} GameState;

GameState gamestate=GAME_STATE_CODE;

typedef struct {
    int w,h,x,y,d,g;
    char *cells;
} Board;

typedef struct {
    int w,h,x,y;
    int cx,cy;
    int *colors;
    char *instructions;
} Code;

bool quit=false;
int key=0;
int maxx,maxy;

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
    srand(time(NULL));

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

char *randline(const char *filename) {
    char *line = NULL;
    size_t llen = 0;
    ssize_t rlen;
    FILE *fp = fopen(filename, "r");
    char *chosen = NULL;
    size_t count = 0;

    if (!fp) return NULL;

    while ((rlen = getline(&line, &llen, fp)) != -1) {
        count++;
        if (rand() % count == 0) {
            line[strcspn(line, "\n")] = '\0';
            if(chosen) free(chosen);
            chosen = strdup(line);
        }
    }

    free(line);
    fclose(fp);
    return chosen;
}

static int hexval(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int symval(int c) {
    return c - 'G' + '0';
}

static bool is_hex(int c) {
    return hexval(c) != -1;
}

static bool is_sym(int c) {
    return c >= 'G' && c <= 'N';
}

char *decode(Board *board,char *e) {
    char *cells=calloc(board->w*board->h,sizeof(*cells));
    int count;
    int c;
    int i,j,k,l;
    int sym=0;
    int size=board->w*board->h;

    for(i=0;i<board->w*board->h;i++) cells[i]='0';

    i=0; j=0; k=0; l=0;
    while(i<size && e[i]) {
        if(is_hex(e[i])) {
            k=hexval(e[i]);
            i++;
            while(i<size && is_hex(e[i])) {
                k=k*16+hexval(e[i]);
                i++;
            }
            if(is_sym(e[i])) {
                sym=symval(e[i]);
                for(j=0;j<k;j++) {
                    cells[l++]=sym;
                }
            }
        } else if(is_sym(e[i])) {
            cells[l++]=symval(e[i]);
        }
        i++;
    }
    return cells;
}

Board *load(const char *filename) {
    Board *board=malloc(sizeof(*board));
    char *level=randline(filename);
    char *enc=NULL;
    char *dec=NULL;
    int i,j,k;
    int color;
    bool star;

    if(!board) return NULL;
    if(!level) return NULL;

    sscanf(level,"%d,%d,%d,%d,%d",
        &board->w,&board->h,
        &board->x,&board->y,&board->d);

    board->cells=calloc(board->w*board->h,sizeof(*board->cells));
    enc=strrchr(level,',')+1;
    board->cells=decode(board,enc);

    k=0;
    board->g=0;
    for(j=0;j<board->h;j++) {
        for(i=0;i<board->w;i++) {
            if((board->cells[k++]-'0') & 0x04) board->g++;
        }
    }

    return board;
}

void Board_Draw(Board *board,int x,int y) {
    int i,j,k;
    char cell;
    int color_pair;
    bool star;

    k=0;
    for(j=0;j<board->h;j++) {
        for(i=0;i<board->w;i++) {

            cell=board->cells[k];
            color_pair=((cell-'0') & 0x03) + 5;
            star=(((cell-'0') &0x04) >> 2)?true:false;

            move(j+y,i+x);
            attron(COLOR_PAIR(color_pair));

            if(star) {
                addch('*');
            } else if(cell=='0') {
                addch('.');
            } else {
                addch(' ');
            }

			if(board->x==i && board->y==j) {
				move(j+y,i+x);
				switch(board->d) {
				case 0: addch(ACS_UARROW); break;
				case 1: addch(ACS_RARROW); break;
				case 2: addch(ACS_DARROW); break;
				case 3: addch(ACS_LARROW); break;
				default: break;
				}
			}

            k++;
        }
    }
    refresh();
}

Code *Code_New(int x,int y) {
    int i,j,k;

    Code *code=malloc(sizeof(*code));
    if(!code) return NULL;
    
    code->w=10;
    code->h=10;
    code->x=x;
    code->y=y;
    code->cx=0;
    code->cy=0;
    
    code->colors=calloc(code->w*code->h,sizeof(*code->colors));
    code->instructions=calloc(code->w*code->h,sizeof(*code->instructions));

    k=0;
    for(j=0;j<code->h;j++) {
        for(i=0;i<code->w;i++) {
            code->colors[k]=0;
            code->instructions[k]='.';
            k++;
        }
    }

    return code;
}

void Code_Draw(Code *code) {
    int i,j,k,l,m;
    
    move(code->y,code->x+1);
    attron(COLOR_PAIR(5));
    for(i=0;i<10;i++) printw("%d",i);
    m=0;
    for(j=0;j<10;j++) {
        move(j+code->y+1,code->x);
        attron(COLOR_PAIR(5));
        printw("%d",j);
        for(i=0;i<10;i++) {
            k=code->colors[m]+1;
            l=code->instructions[m];

            move(code->y+j+1,code->x+i+1);
            attron(COLOR_PAIR(k));            
            addch(l);
            
            m++;            
        }
    }
    move(code->cy+code->y+1,code->cx+code->x+1);
    refresh();
}

void Input(Code *code) {
    int key;

    if(kbhit()) {
        key=getch();
        if(key==0) key=getch()+256;
        if(key==27) quit=true;
//        printf("%d\n",key);
        switch(key) {
        case 259: if(code->y>0) code->y--; break;
        case 258: if(code->y<9) code->y++; break;
        case 260: if(code->x>0) code->x--; break;
        case 261: if(code->x<9) code->x++; break;
        default: break;
        }
        move(code->cy+code->y+1,code->cx+code->x+1);
    }
}

int main(void) {

    init();

    Board *board=load("LEVELS.TXT");
    Code *code=Code_New(0,0);

    getmaxyx(stdscr,maxy,maxx);

    Board_Draw(board,maxx-board->w,0);
    Code_Draw(code);
    
    while(!quit) {
        Input(code);
    }

    cleanup();

    return 0;
}
