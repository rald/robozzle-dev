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

bool quit=false;
int key=0;

int *b=NULL;
int w=0,h=0;
int x=0,y=0,d=0,g=0;

int *c=NULL;
int cw=10,ch=10;
int cx=0,cy=0;

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
                setcolor(fg,bg);
                printw(".");
            }
        }
    }
    refresh();
}

void code(int u,int v) {
    int i,j;
    
    move(v,u+1);
    for(i=0;i<cw;i++) printw("%d",i);
    for(j=0;j<ch;j++) {
        move(j+v+1,u);
        printw("%d",j);
        for(i=0;i<cw;i++) {
            move(j+v+1,i+u+1);
            addch('.');            
        }
    }
    move(cy+v+1,cx+u+1);
    refresh();
}

int main(int argc,char *argv[]) {

	if(argc!=2) {
		fprintf(stderr,"Syntax: %s LevelFile\n",argv[0]);
		return 1;
	}

	load(argv[1]);

	c=calloc(cw*ch,sizeof(*c));

	
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

	show(16,1);
	code(0,0);
	
	
    while(!quit) {
	    if(kbhit()) {
	        key=getch();
	        if(key==0) key=getch()+256;
	        if(key==27) quit=true;
//            printf("%d\n",key);	        
	        if(key==259) if(cy>0)  cy--;
	        if(key==258) if(cy<9) cy++;
	        if(key==260) if(cx>0)  cx--;
	        if(key==261) if(cx<9) cx++;
	        move(cy+1,cx+1);
	    }
	}
	
	
	endwin();

    return 0;
}
