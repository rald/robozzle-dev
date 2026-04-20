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
    char *b=calloc(board->w*board->h,sizeof(*b));
    int count;
    int c;
    int i,j,k,l;
    int sym=0;
    int size=board->w*board->h;

    for(i=0;i<board->w*board->h;i++) b[i]='0';

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
                    b[l++]=sym;
                }
            }   
        } else if(is_sym(e[i])) {
            b[l++]=symval(e[i]);
        }
        i++;
    }
    return b;
}

Board *load(const char *filename) {
    Board *board=malloc(sizeof(*board));
    char *level=randline(filename);
    char *enc=NULL;
    char *dec=NULL;
    int i,j,k;

    if(!board) return NULL;
    if(!level) return NULL;

    sscanf(level,"%d,%d,%d,%d,%d",
        &board->w,&board->h,
        &board->x,&board->y,&board->d);

    board->colors=calloc(board->w*board->h,sizeof(*board->colors));
    board->stars=calloc(board->w*board->h,sizeof(*board->stars));

    enc=strrchr(level,',')+1;

    dec=decode(board,enc);

    k=0;
    for(j=0;j<board->h;j++) {
        for(i=0;i<board->w;i++) {
            printf("%c",dec[k++]);
        }
        printf("\n");
    }
    printf("\n");
    
    return board;
}

int main(void) {

    srand(time(NULL));
    
    Board *board=load("LEVELS.TXT");

    return 0;
}
