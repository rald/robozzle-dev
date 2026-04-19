#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define CONIO_IMPLEMENTATION
#include <conio.h>

char *UP="\u2191";
char *RT="\u2192";
char *DN="\u2193";
char *LF="\u2190";

/*
?00 NONE
?01 RED
?10 GREEN
?11 BLUE
0?? BLANK
1?? STAR
*/

int *b=NULL;
int w=0,h=0;
int x=0,y=0,d=0;
int key=0;
bool quit=false;

void load(char *filename) {
    int i,j,k;
    int c;
    char *o="01234567";
    FILE *fp=fopen(filename,"r");
    fscanf(fp,"%d,%d,%d,%d,%d",&w,&h,&x,&y,&d);
    b=calloc(w*h,sizeof(*b));
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
        }
    }
    fclose(fp);
}

void show(int u,int v) {
    int i,j,k,l;
    textbackground(BLACK);
    textcolor(LIGHTGRAY);
    clrscr();    
    for(j=0;j<h;j++) {
        for(i=0;i<w;i++) {
            gotoxy(i+u,j+v);
            k = b[j*w+i] & 0x03;
            l = b[j*w+i] & 0x04;
            switch(k) {
                case 0: textbackground(BLACK);  break;
                case 1: textbackground(RED);    break;
                case 2: textbackground(GREEN);  break;
                case 3: textbackground(BLUE);   break;
            }
            textcolor(YELLOW);
            if(x==i && y==j) {
                textcolor(LIGHTGRAY);
                switch(d) {
                    case 0: cputs(UP); break;
                    case 1: cputs(RT); break;
                    case 2: cputs(DN); break;
                    case 3: cputs(LF); break;
                }    
            } else if(l) {
                cputs("*");
            } else if(k!=0) {
                cputs(" ");
            } else {
                cputs(".");
            }
        }
    }
    fflush(stdout);
}

int main(int argc,char *argv[]) {

	if(argc!=2) {
		fprintf(stderr,"Syntax: %s LevelFile\n",argv[0]);
		return 1;
	}

	load(argv[1]);

    cursorhide();
	show(1,1);

	textbackground(BLACK);
	textcolor(LIGHTGRAY);
    gotoxy(1,h+2); cputs("OK\n");
	
	textbackground(BLACK);
	textcolor(LIGHTGRAY);
    cursorshow();

    return 0;
}
