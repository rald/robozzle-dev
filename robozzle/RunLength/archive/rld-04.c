#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

int hv(char c) {
    char *h="0123456789ABCDEF";
    int i,j=-1;
    for(i=0;i<16;i++) {
        if(c==h[i]) {
            j=i;
            break;
        }
    }
    return j;
}

int ov(char c) {
    return c-'G'+'0';
}

int main(int argc,char *argv[]) {

    int w,h,x,y,d;
    int c0,c1;
    int i,j,k;

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

    i=0; k=w;
    while((c0=fgetc(fp0))!=EOF) {
        if(isxdigit((unsigned char)c0)) {
            i=i*16+hv(c0);
            while((c1=fgetc(fp0))!=EOF && isxdigit((unsigned char)c1)) {
                i=i*16+hv(c1);
            }
            if(c1!=EOF) {
                for(j=0;j<i;j++) {
                    fputc(ov(c1),fp1);
                    k--;
                    if(k<=0) {
                        k=w;
                        fputc('\n',fp1);
                    }
                }
            }
            i=0;
        } else {
            fputc(ov(c0),fp1);
            k--;
            if(k<=0) {
                k=w;
                fputc('\n',fp1);
            }
        }
    }

    fclose(fp1);
    fclose(fp0);

    return 0;
}


