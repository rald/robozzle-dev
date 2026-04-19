#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]) {

    if(argc!=2) {
        fprintf(stderr,"syntax: %s LevelFile\n",argv[0]);
        return 1;
    }
    
    FILE *fp=fopen(argv[1],"r");
    int c=0;
    int p=-1;
    int f=1;
    while((c=fgetc(fp))!=EOF) {
        if(c==p) f++; else p=c;
    }

    return 0;
}
