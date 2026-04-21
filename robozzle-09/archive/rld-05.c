#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static int hv(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static bool vh(int c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

static int cv(int c) {
    return c - 'G' + '0';
}

static bool vc(int c) {
    return (c >= 'G' && c <= 'N');
}

static void enter(int *k, int w, FILE *fp) {
    (*k)--;
    if (*k <= 0) {
        *k = w;
        fputc('\n', fp);
    }
}

int main(int argc, char *argv[]) {
    int w, h, x, y, d;
    int c0, c1, c2;
    int i, j, k;

    if (argc != 3) {
        fprintf(stderr, "syntax: %s EncFile DecFile\n", argv[0]);
        return 1;
    }

    FILE *fp0 = fopen(argv[1], "r");
    if (!fp0) {
        fprintf(stderr, "Error opening file %s: %s\n", argv[1], strerror(errno));
        return 1;
    }

    FILE *fp1 = fopen(argv[2], "w");
    if (!fp1) {
        fprintf(stderr, "Error opening file %s: %s\n", argv[2], strerror(errno));
        fclose(fp0);
        return 1;
    }

    if (fscanf(fp0, "%d,%d,%d,%d,%d,", &w, &h, &x, &y, &d) != 5) {
        fprintf(stderr, "Invalid header\n");
        fclose(fp1);
        fclose(fp0);
        return 1;
    }

    fprintf(fp1, "%d,%d,%d,%d,%d\n", w, h, x, y, d);

    i = 0;
    j = 0;
    k = w;

    while((c0 = fgetc(fp0)) != EOF) {
        if(vh(c0)) {
            i=hv(c0);
            while((c1 = fgetc(fp0)) != EOF) {
                if(!vh(c1)) { ungetc(c1,fp0); break; }
                i=i*16+vh(c1);
            }
            if((c2=fgetc(fp0))!=EOF && vc(c2)) {
                for(j = 0; j < i; j++) {
                    fputc(cv(c2),fp1);
                    enter(&k,w,fp1);
                }
            }
        } else if(vc(c0)) {
            fputc(cv(c0),fp1);
            enter(&k,w,fp1);
        }
    }

    fclose(fp1);
    fclose(fp0);
    return 0;
}
