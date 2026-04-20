#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum {
    DECODE_STATE_DEF=0,
    DECODE_STATE_HEX,
    DECODE_STATE_STR,
    DECODE_STATE_MAX
} DecodeState;

int main(int argc,char *argv[]) {

    DecodeState s=DECODE_STATE_DEF;
    int w,h,x,y,d;
    int c=-1;
    char t[1024];
    int f=0;
    int i,j;

    if(argc!=3) {
        fprintf(stderr,"syntax: %s EncFile DecFile\n",argv[0]);
        return 1;
    }

    FILE *fp0=fopen(argv[1],"r");

    if(!fp0) {
        fprintf(stderr,"Error opening file %s: %s\n",argv[1],strerror(errno));
        return 1;
    }

    FILE *fp1=fopen(argv[2],"w");

    if(!fp1) {
        fprintf(stderr,"Error opening file %s: %s\n",argv[2],strerror(errno));
        return 1;
    }

    fscanf(fp0,"%d,%d,%d,%d,%d,",&w,&h,&x,&y,&d);
    fprintf(fp1,"%d,%d,%d,%d,%d\n",w,h,x,y,d);

    i=0;
    f=0;
    t[0]='\0';
    while((c=fgetc(fp0))!=EOF) {
        switch(s) {
        case DECODE_STATE_DEF:
            if((c>='0' && c<='9') ||
               (c>='A' && c<='F')) {
                strcat(t,(char[]) {c,'\0'});
                s=DECODE_STATE_HEX;
            } else if(c>='G' && c<='N') {
                if(f>0) {
                    while(f) {
                        fprintf(fp1,"%c",c-'G'+'0');
                        i++;
                        if(i>w) {
                            i=0;
                            fprintf(fp1,"\n");
                        }
                        f--;
                    }
                    s=DECODE_STATE_DEF;
                } else {
                    strcat(t,(char[]) {c-'G'+'0','\0'});
                    i++;
                    if(i>w) {
                        i=0;
                        fprintf(fp1,"\n");
                    }
                    s=DECODE_STATE_STR;
                }
            }
            break;
        case DECODE_STATE_HEX:
            if((c>='0' && c<='9') ||
               (c>='A' && c<='F')) {
                strcat(t,(char[]) {c,'\0'});
            } else {
                sscanf(t,"%X",&f);
                t[0]='\0';
                ungetc(c,fp0);
                s=DECODE_STATE_DEF;
            }
            break;
        case DECODE_STATE_STR:
            if(c>='G' && c<='N') {
                strcat(t,(char[]) {c-'G'+'0','\0'});
            } else {
                for(j=0; t[j]; j++) {
                    fprintf(fp1,"%c",t[j]);
                    i++;
                    if(i>w) {
                        i=0;
                        fprintf(fp1,"\n");
                    }
                }
                t[0]='\0';
                ungetc(c,fp0);
                s=DECODE_STATE_DEF;
            }
            break;
        }
    }

    fclose(fp1);
    fclose(fp0);

    return 0;
}
