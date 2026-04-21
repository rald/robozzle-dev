#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

typedef enum {
    GAME_STATE_CODE=0,
    GAME_STATE_RUN,
    GAME_STATE_END,
    GAME_STATE_MAX
} GameState;

GameState gamestate=GAME_STATE_CODE;

typedef struct {
    int w,h,x,y,d,g;
    int cx,cy;
    char *cells;
} Board;

typedef struct {
    int w,h,x,y;
    int cx,cy;
    int ip,fn;
    int *colors;
    char *instructions;
} Code;

bool quit=false;
int key=0;
int maxx,maxy;

Board *board0=NULL;
Board *board1=NULL;
Code *code=NULL;

FILE *levelfp=NULL;

#define CSTK_MAX 100
int cstk[CSTK_MAX];
int csp=CSTK_MAX;

void push(int ip) {
    cstk[--csp]=ip;
}

int pop() {
    int ip=cstk[csp++];
    return ip;
}

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

Board *Board_New(FILE *fp) {
    Board *board=malloc(sizeof(*board));
    char *enc=NULL;
    char *dec=NULL;
    int i,j,k;
    int color;
    bool star;

    char *level=NULL;
    size_t llen=0;
    ssize_t rlen=0;
    
    rlen=getline(&level,&llen,fp);
    
    if(rlen==-1) return NULL;
       
    if(!board) return NULL;
    if(!level) return NULL;

    sscanf(level,"%d,%d,%d,%d,%d",
        &board->w,&board->h,
        &board->cx,&board->cy,&board->d);

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

    free(level);

    return board;
}

void Board_Free(Board *board) {
    free(board->cells);
    free(board);
}

Board *Board_Copy(Board *b0) {
    int i;

    Board *b1=malloc(sizeof(*b1));

    if(!b1) return NULL;

    b1->w=b0->w;
    b1->h=b0->h;
    b1->x=b0->x;
    b1->y=b0->y;
    b1->d=b0->d;
    b1->g=b0->g;
    b1->cx=b0->cx;
    b1->cy=b0->cy;
    
    b1->cells=calloc(b1->w*b1->h,sizeof(*b1->cells));

    for(i=0;i<b1->w*b1->h;i++) b1->cells[i]=b0->cells[i];
    
    return b1;
}

void Board_Draw(Board *board) {
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

            move(j+board->y,i+board->x);
            attron(COLOR_PAIR(color_pair));

            if(star) {
                addch('*');
            } else if(cell=='0') {
                addch('.');
            } else {
                addch(' ');
            }

			if(board->cx==i && board->cy==j) {
				move(j+board->y,i+board->x);
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
    code->ip=0;
    code->fn=0;
    
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

void putcc(Code *code,int k) {
    move(code->cy+code->y+1,code->cx+code->x+1);
    attron(COLOR_PAIR(code->colors[k]+1));
    addch(code->instructions[k]);        
}

void Code_Input() {
    int k;
    int key;

    if(kbhit()) {
        key=getch();
        if(key==0) key=getch()+256;

//        printf("%d\n",key);

        k=code->cy*code->w+code->cx;

        switch(key) {
        case 27: quit=true; break;

        case 259: if(code->cy>0)         code->cy--; break;
        case 258: if(code->cy<code->h-1) code->cy++; break;
        case 260: if(code->cx>0)         code->cx--; break;
        case 261: if(code->cx<code->w-1) code->cx++; break;
        
        case '^': code->instructions[k]='^'; putcc(code,k); break;
        case '<': code->instructions[k]='<'; putcc(code,k); break;
        case '>': code->instructions[k]='>'; putcc(code,k); break;

        case 'X': case 'x': code->instructions[k]='X'; putcc(code,k); break;

        case '.': code->colors[k]=0; code->instructions[k]='.'; putcc(code,k); break;

        case 'r': if(code->instructions[k]!='R') code->colors[k]=1; putcc(code,k); break;
        case 'g': if(code->instructions[k]!='G') code->colors[k]=2; putcc(code,k); break;
        case 'b': if(code->instructions[k]!='B') code->colors[k]=3; putcc(code,k); break;
        
        case 'R': if(code->colors[k]!=1) code->instructions[k]='R'; putcc(code,k); break;
        case 'G': if(code->colors[k]!=2) code->instructions[k]='G'; putcc(code,k); break;
        case 'B': if(code->colors[k]!=3) code->instructions[k]='B'; putcc(code,k); break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':        
            code->instructions[k]=key; putcc(code,k); 
            break;
            
        case 9:
            Board_Free(board1);
            board1=Board_Copy(board0);
            Board_Draw(board1);
            code->ip=0;
            code->fn=0;
            getmaxyx(stdscr,maxy,maxx);
            attron(COLOR_PAIR(1));
            move(maxy-1,0); printw("RUN ");
            move(code->fn+code->y+1,code->ip+code->x+1);
            gamestate=GAME_STATE_RUN;
            break;

        default: break;
        }
        
        if(gamestate==GAME_STATE_CODE) move(code->cy+code->y+1,code->cx+code->x+1);

    }
}

void next() {
    int k;

    if((board1->cells[board1->cy*board1->w+board1->cx]&0x04)>>2) {
        board1->cells[board1->cy*board1->w+board1->cx]&=0x03;
        board1->g--;
        if(board1->g<=0) {
            getmaxyx(stdscr,maxy,maxx);
            attron(COLOR_PAIR(1));
            move(maxy-1,0); printw("GAME OVER! PRESS ENTER TO CONTINUE...");
            move(code->cy+code->y+1,code->cx+code->x+1);
            gamestate=GAME_STATE_END;
            return;
        }
    }           

    code->ip++; 
    if(code->ip>code->w-1) {
        code->ip=code->w-1;
        if(code->fn==0) {
            getmaxyx(stdscr,maxy,maxx);
            attron(COLOR_PAIR(1));
            move(maxy-1,0); printw("GAME OVER");
            move(code->cy+code->x+1,code->cx+code->x+1);
            gamestate=GAME_STATE_END; 
        } else {
            k=pop(); 
            code->ip=k%code->w; 
            code->fn=k/code->w; 
        }
    }
}

void Run_Input() {
    int i,j,k,l,m;
    int key;
    
    if(kbhit()) {
        key=getch();
        if(key==0) key=getch()+256;        

        switch(key) {
        case 27: quit=true; break;
        case 9:
            getmaxyx(stdscr,maxy,maxx);
            attron(COLOR_PAIR(1));
            move(maxy-1,0); printw("CODE");
            move(code->cy+code->y+1,code->cx+code->x+1);
            gamestate=GAME_STATE_CODE;            
            break;
        case 32:

            if((board1->cells[board1->cy*board1->w+board1->cx]&0x04)>>2) {
                board1->cells[board1->cy*board1->w+board1->cx]&=0x03;
                board1->g--;
                if(board1->g<=0) {
                    getmaxyx(stdscr,maxy,maxx);
                    attron(COLOR_PAIR(1));
                    move(maxy-1,0); printw("GAME OVER! PRESS ENTER TO CONTINUE...");
                    move(code->cy+code->y+1,code->cx+code->x+1);
                    gamestate=GAME_STATE_END;
                    return;
                }
            }           
        
            if(     (code->colors[code->fn*code->w+code->ip] == 
                    ((board1->cells[board1->cy*board1->w+board1->cx]-'0')&0x03)) ||
                    (code->colors[code->fn*code->w+code->ip] == 0)) {
                
                switch(code->instructions[code->fn*code->w+code->ip]) {

                case '.': next(); break;

                case '^':
                    
                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    if((board1->cells[board1->cy*board1->w+board1->cx] & 0x04) >> 2) {
                        addch('*');
                    } else if(board1->cells[board1->cy*board1->w+board1->cx]=='0') {
                        addch('.');
                    } else {
                        addch(' ');
                    }
                    
                    switch(board1->d) {
                    case 0: if(board1->cy>0)           board1->cy--; next(); break;
                    case 1: if(board1->cx<board1->w-1) board1->cx++; next(); break;
                    case 2: if(board1->cy<board1->h-1) board1->cy++; next(); break;
                    case 3: if(board1->cx>0)           board1->cx--; next(); break;
                    default: break;
                    }
                    
                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    switch(board1->d) {
                    case 0: addch(ACS_UARROW); break;
                    case 1: addch(ACS_RARROW); break;
                    case 2: addch(ACS_DARROW); break;
                    case 3: addch(ACS_LARROW); break;
                    }

                    break;
                    
                case '<': 
                
                    board1->d--; if(board1->d<0) board1->d=3; 

                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    switch(board1->d) {
                    case 0: addch(ACS_UARROW); break;
                    case 1: addch(ACS_RARROW); break;
                    case 2: addch(ACS_DARROW); break;
                    case 3: addch(ACS_LARROW); break;
                    }
                                        
                    next(); 
                    break;
                    
                case '>': 
                
                    board1->d++; if(board1->d>3) board1->d=0; 
                    
                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    switch(board1->d) {
                    case 0: addch(ACS_UARROW); break;
                    case 1: addch(ACS_RARROW); break;
                    case 2: addch(ACS_DARROW); break;
                    case 3: addch(ACS_LARROW); break;
                    }
                                        
                    next(); 
                    break;

                case 'R': 

                    board1->cells[board1->cy*board1->w+board1->cx]=(board1->cells[board1->cy*board1->w+board1->cx]&0x04)|0x01; 

                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    if((board1->cells[board1->cy*board1->w+board1->cx] & 0x04) >> 2) {
                        addch('*');
                    } else if(board1->cells[board1->cy*board1->w+board1->cx]=='0') {
                        addch('.');
                    } else {
                        addch(' ');
                    }

                    break;

                case 'G': 

                    board1->cells[board1->cy*board1->w+board1->cx]=(board1->cells[board1->cy*board1->w+board1->cx]&0x04)|0x01; 

                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    if((board1->cells[board1->cy*board1->w+board1->cx] & 0x04) >> 2) {
                        addch('*');
                    } else if(board1->cells[board1->cy*board1->w+board1->cx]=='0') {
                        addch('.');
                    } else {
                        addch(' ');
                    }

                    break;

                case 'B': 

                    board1->cells[board1->cy*board1->w+board1->cx]=(board1->cells[board1->cy*board1->w+board1->cx]&0x04)|0x01; 

                    move(board1->cy+board1->y,board1->cx+board1->x);
                    attron(COLOR_PAIR(((board1->cells[board1->cy*board1->w+board1->cx]-'0') & 0x03) + 5));                    

                    if((board1->cells[board1->cy*board1->w+board1->cx] & 0x04) >> 2) {
                        addch('*');
                    } else if(board1->cells[board1->cy*board1->w+board1->cx]=='0') {
                        addch('.');
                    } else {
                        addch(' ');
                    }

                    break;
                    
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':

                    push(code->fn*code->w+code->ip);
                    code->fn=code->instructions[code->fn*code->w+code->ip]-'0';
                    code->ip=0;
                    break;
                    
                case 'X':
                
                    k=pop();
                    code->fn=k/code->w;
                    code->ip=k%code->w;
                    break;

                default: break;
                }                
            } else {
                next();
            }

                    
            move(code->fn+code->y+1,code->ip+code->x+1);

            break;

        default: break;
        }
    }
    refresh();
}

void End_Input() {
    if(kbhit()) {
        key=getch();
        if(key==0) key=getch()+256;        
        if(key==27) quit=true;
        if(key==10) {
            getmaxyx(stdscr,maxy,maxx);
            Board_Free(board0);

            board0=Board_New(levelfp);

            if(!board0) {
                fclose(levelfp);
                clear();
                attron(COLOR_PAIR(1));
                printw("GAME OVER");
                refresh();
                getchar();
                cleanup();
                exit(1);
            }

            board0->x=maxx-board0->w;
            board0->y=0;
            board1=Board_Copy(board0);
            code=Code_New(0,0);
            clear();
            Board_Draw(board1);
            Code_Draw(code);
            attron(COLOR_PAIR(1));
            move(maxy-1,0); printw("CODE");
            move(code->cy+code->x+1,code->cx+code->x+1);
            
            gamestate=GAME_STATE_CODE;
        }
    }    
}

int main(void) {

    init();

    getmaxyx(stdscr,maxy,maxx);

    levelfp=fopen("LEVELS.TXT","r");

    if(!levelfp) {
        fprintf(stderr,"Error opening file \"LEVELS.TXT\".");
        exit(1);
    }
    
    board0=Board_New(levelfp);
    
    if(!board0) {
        fclose(levelfp);
        fprintf(stderr,"Error loading file \"LEVELS.TXT\".");
        exit(1);
    }
    
    board0->x=maxx-board0->w;
    board0->y=0;
    
    board1=Board_Copy(board0);

    code=Code_New(0,0);

    clear();
    Board_Draw(board1);
    Code_Draw(code);
    
    getmaxyx(stdscr,maxy,maxx);
    attron(COLOR_PAIR(1));
    move(maxy-1,0); printw("CODE");
    move(code->cy+code->x+1,code->cx+code->x+1);

    while(!quit) {
        switch(gamestate) {
        case GAME_STATE_CODE:   Code_Input();   break;
        case GAME_STATE_RUN:    Run_Input();    break;
        case GAME_STATE_END:    End_Input();    break;
        default: break;
        }
    }

    cleanup();

    return 0;
}
