#ifndef CONIO_H
#define CONIO_H

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

// Classic Color Definitions
#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHTGRAY     7
#define DARKGRAY      8
#define LIGHTBLUE     9
#define LIGHTGREEN    10
#define LIGHTCYAN     11
#define LIGHTRED      12
#define LIGHTMAGENTA  13
#define YELLOW        14
#define WHITE         15
#define BLINK         128

// Prototypes
void clreol();
void insline();
void delline();
void gotoxy(int x, int y);
void clrscr();
void textbackground(int color);
void textcolor(short color);
int ungetch(int ch);
int getch_echo(bool echo);
int getch();
int getche();
int wherexy(int *x, int *y);
int wherex();
int wherey();
int getmaxxy(int *x, int *y);
int kbhit();
int putch(const char c);
int cputs(const char *str);

#ifdef CONIO_IMPLEMENTATION

static int bgc = 40;

void clreol() {
    printf("\033[K");
}

void insline() {
    printf("\x1b[1L");
}

void delline() {
    printf("\033[1M");
}

void gotoxy(int x, int y) {
    printf("\033[%d;%df", y, x);
}

void clrscr() {
    printf("\033[%dm\033[2J\033[1;1f", bgc);
}

void textbackground(int color) {
    switch(color % 16) {
        case BLACK:        bgc = 40; break;
        case BLUE:         bgc = 44; break;
        case GREEN:        bgc = 42; break;
        case CYAN:         bgc = 46; break;
        case RED:          bgc = 41; break;
        case MAGENTA:      bgc = 45; break;
        case BROWN:        bgc = 43; break;
        case LIGHTGRAY:    bgc = 47; break;
        case DARKGRAY:     bgc = 40; break;
        case LIGHTBLUE:    bgc = 44; break;
        case LIGHTGREEN:   bgc = 42; break;
        case LIGHTCYAN:    bgc = 46; break;
        case LIGHTRED:     bgc = 41; break;
        case LIGHTMAGENTA: bgc = 45; break;
        case YELLOW:       bgc = 43; break;
        case WHITE:        bgc = 47; break;
    }
}

void textcolor(short color) {
    int intensity = (color > 7) ? 1 : 0;
    int fg;
    switch(color % 8) {
        case 0: fg = 30; break; // Black
        case 1: fg = 34; break; // Blue
        case 2: fg = 32; break; // Green
        case 3: fg = 36; break; // Cyan
        case 4: fg = 31; break; // Red
        case 5: fg = 35; break; // Magenta
        case 6: fg = 33; break; // Yellow/Brown
        case 7: fg = 37; break; // White/Gray
    }
    printf("\033[%d;%d;%dm", intensity, fg, bgc);
}

int ungetch(int ch) {
    return ungetc(ch, stdin);
}

int getch_echo(bool echo) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ICANON;
    if (echo)
        newt.c_lflag |= ECHO;  // Fixed: use OR to enable
    else
        newt.c_lflag &= ~ECHO; // Keep as AND NOT to disable
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int getch() {
    return getch_echo(false);
}

int getche() {
    return getch_echo(true);
}

int wherexy(int *x, int *y) {
    printf("\033[6n");
    fflush(stdout); // CRITICAL: Prevent the program from hanging here
    
    if (getch() != '\x1B') return -1;
    if (getch() != '\x5B') return -1;
    
    int in, ly = 0, lx = 0;
    while ((in = getch()) != ';')
        ly = ly * 10 + in - '0';
    while ((in = getch()) != 'R')
        lx = lx * 10 + in - '0';
    
    if (x) *x = lx;
    if (y) *y = ly;
    return 0;
}

int wherex() {
    int x = 0, y = 0;
    wherexy(&x, &y);
    return x;
}

int wherey() {
    int x = 0, y = 0;
    wherexy(&x, &y);
    return y;
}

/**
 * ANSI-only version of getmaxxy. 
 * Moves cursor to the extreme bottom-right and queries position.
 */
int getmaxxy(int *x, int *y) {
    // 1. Save current cursor position (SCO sequence)
    printf("\033[s"); 
    
    // 2. Attempt to move to a coordinate larger than any screen
    // The terminal will clamp the cursor to the actual max rows/cols.
    printf("\033[999;999H");
    fflush(stdout);

    // 3. Use your existing wherexy to ask "Where are we now?"
    int result = wherexy(x, y);

    // 4. Restore the cursor to where it was originally
    printf("\033[u");
    fflush(stdout);

    return result;
}

int kbhit() {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int putch(const char c) {
    return putchar(c);
}

int cputs(const char *str) {
    return printf("%s", str);
}

void cursorhide() {
    printf("\033[?25l");
}

void cursorshow() {
    printf("\033[?25h");
}



#endif /* CONIO_IMPLEMENTATION */



#endif /* CONIO_H */
