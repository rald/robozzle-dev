#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ncurses.h>

/*
?00 NONE
?01 RED
?10 GREEN
?11 BLUE
0?? BLANK
1?? STAR
*/

typedef enum {
    GAME_STATE_CODE=0,
    GAME_STATE_RUN,
    GAME_STATE_MAX
} GameState;

GameState gamestate=GAME_STATE_CODE;

bool quit=false;
int key=0;

int *b=NULL;
int w=0,h=0;
int x=0,y=0,d=0,g=0;

int *c=NULL,*e=NULL;
int cw=10,ch=10;
int cx=0,cy=0;
int pcx=0,pcy=0;



int kbhit(void) {
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

void load(char *filename) {
    int i,j,k;
    int c;
    char *o="01234567";
    FILE *fp=fopen(filename,"r");
    fscanf(fp,"%d,%d,%d,%d,%d",&w,&h,&x,&y,&d);
    b=calloc(w*h,sizeof(*b));
    g=0;
    k=0;
    while((c=fgetc(fp))!=EOF) {
        j=-1;
        for(i=0;i<8;i++) {
            if(c==o[i]) {
                j=i;
                break;
            }
        }
        if(j!=-1) {
            b[k++]=j;
            if(j & 0x04) g=g+1;
        }
    }
    fclose(fp);
}

void setcolor(int fg,int bg) {
    if(fg==0 && bg==0) attron(COLOR_PAIR(1));
    if(fg==0 && bg==1) attron(COLOR_PAIR(2));
    if(fg==0 && bg==2) attron(COLOR_PAIR(3));
    if(fg==0 && bg==3) attron(COLOR_PAIR(4));
    if(fg==1 && bg==1) attron(COLOR_PAIR(5));
    if(fg==1 && bg==2) attron(COLOR_PAIR(6));
    if(fg==1 && bg==3) attron(COLOR_PAIR(7));
}

void show(int u,int v) {
    int i,j,k,l,fg,bg;
    attron(COLOR_PAIR(1));
    clear();
    for(j=0;j<h;j++) {
        for(i=0;i<w;i++) {
            move(j+v,i+u);
            k = b[j*w+i] & 0x03;
            l = b[j*w+i] & 0x04;
            fg = 0;
            switch(k) {
            case 0: bg=0; break;
            case 1: bg=1; break;
            case 2: bg=2; break;
            case 3: bg=3; break;
            default: break;
            }
            if(x==i && y==j) {
                setcolor(fg,bg);
                switch(d) {
                case 0: addch(ACS_UARROW); break;
                case 1: addch(ACS_RARROW); break;
                case 2: addch(ACS_DARROW); break;
                case 3: addch(ACS_LARROW); break;
                default: break;
                }
            } else if(l) {
                fg=1;
                setcolor(fg,bg);
                printw("*");
            } else if(k!=0) {
                fg=0;
                setcolor(fg,bg);
                printw(" ");
            } else {
                fg=0;
                bg=0;
                setcolor(fg,bg);
                printw(".");
            }
        }
    }
    refresh();
}

void code(int u,int v) {
    int i,j,k,l;
    move(v,u+1); for(i=0;i<cw;i++) { attron(COLOR_PAIR(8)); printw("%d",i); }
    for(j=0;j<ch;j++) {
        move(j+v+1,u);
        attron(COLOR_PAIR(8));
        printw("%d",j);
        for(i=0;i<cw;i++) {
            k=c[j*cw+i];
            l=e[j*cw+i];
            move(j+v+1,i+u+1);
            attron(COLOR_PAIR(l));
            addch(k);
        }
    }
    move(cy+v+1,cx+u+1);
    refresh();
}

int main(int argc,char *argv[]) {
    int i,j,k,l,m;

	if(argc!=2) {
		fprintf(stderr,"Syntax: %s LevelFile\n",argv[0]);
		return 1;
	}

	load(argv[1]);

	c=calloc(cw*ch,sizeof(*c));
	for(i=0;i<cw*ch;i++) c[i]='.';
	e=calloc(cw*ch,sizeof(*e));
	for(i=0;i<cw*ch;i++) e[i]=1;

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
	init_pair(5,COLOR_YELLOW,COLOR_RED);
	init_pair(6,COLOR_YELLOW,COLOR_GREEN);
	init_pair(7,COLOR_YELLOW,COLOR_BLUE);
	init_pair(8,COLOR_YELLOW,COLOR_BLACK);

	show(14,1);
	code(0,0);
	
	
    while(!quit) {
        switch(gamestate) {
        case GAME_STATE_CODE:
	        if(kbhit()) {
	            key=getch();
	            if(key==0) key=getch()+256;
	            if(key=='Q' || key=='q') quit=true;
	            if(key==27) {
	                pcx=cx; pcy=cy;
	                cx=0; cy=0;
	                move(cy+1,cx+1);
	                refresh();
	                gamestate=GAME_STATE_RUN;
	            }

                if(key==259) if(cy>0) cy--;
                if(key==258) if(cy<9) cy++;
                if(key==260) if(cx>0) cx--;
                if(key==261) if(cx<9) cx++;

                k=cy*cw+cx;

                if(key=='.') { c[k]='.'; e[k]=1; attron(COLOR_PAIR(1)); addch(c[k]); }
                if(key=='<') { c[k]='<'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='>') { c[k]='>'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='^') { c[k]='^'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='X' || key=='x')   { c[k]='X'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='R') if(e[k]!=2)   { c[k]='R'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='G') if(e[k]!=3)   { c[k]='G'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='B') if(e[k]!=4)   { c[k]='B'; attron(COLOR_PAIR(e[k])); addch(c[k]); }
                if(key=='r') if(c[k]!='R') { e[k]=2; attron(COLOR_PAIR(2)); addch(c[k]); }
                if(key=='g') if(c[k]!='G') { e[k]=3; attron(COLOR_PAIR(3)); addch(c[k]); }
                if(key=='b') if(c[k]!='B') { e[k]=4; attron(COLOR_PAIR(4)); addch(c[k]); }

                if(key>='0' && key<='9') { c[k]=key; attron(COLOR_PAIR(e[k])); addch(c[k]); }

                move(cy+1,cx+1);       
                
                refresh();
	        }
            break;

        case GAME_STATE_RUN: 
	        if(kbhit()) {
	            key=getch();
	            if(key==0) key=getch()+256;
	            if(key=='Q' || key=='q') quit=true;
	            if(key==27) {
	                cx=pcx; cy=pcy;
	                move(cy+1,cx+1);
    	            refresh();
	                gamestate=GAME_STATE_CODE;
	            }
	            if(key==13) {
	                k=b[y*w+x] & 0x03;
	                l=b[y*w+x] & 0x04;
	                m=cy*cw+cx;
	                if(k==0 || k==e[m]-2) {
	                    if(c[m]=='^') {	                    
	                        switch(d) {
	                        case 0: y--; break;
	                        case 1: x++; break;
	                        case 2: y++; break;
	                        case 3: x--; break;
	                        }
	                        if(b[y*w+x] & 0x04) {
	                            g=g-1;
	                            b[y*w+x] &= 0x03;
	                            
	                        }
	                    } else if(c[m]=='<') {
	                        d--; if(d<0) d=3;
	                    } else if(c[m]=='>') {
	                        d++; if(d>3) d=0;
                        } else if(c[m]=='R') {
                            b[y*w+x] = (b[y*w+x] & 0x04) | 0x01;
                        } else if(c[m]=='G') {
                            b[y*w+x] = (b[y*w+x] & 0x04) | 0x02;
                        } else if(c[m]=='B') {
                            b[y*w+x] = (b[y*w+x] & 0x04) | 0x03;
                        }
	                }	                
	            }
	        }
            break;

        default: break;
        }	        
    }
	
	
	endwin();

    return 0;
}
