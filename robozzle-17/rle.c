#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char ov(int c) {
    return c-'0'+'G';
}

int main (int argc, char *argv[]) {
    int w,h,x,y,d;

    if (argc != 3) {
        fprintf (stderr, "syntax: %s LevFile EncFile\n", argv[0]);
        return 1;
    }

    FILE *fp0 = fopen (argv[1], "r");

    if (!fp0) {
        fprintf (stderr, "Error opening file %s: %s\n", argv[1],
                 strerror (errno));
        return 1;
    }

    FILE *fp1 = fopen (argv[2], "w");

    if (!fp1) {
        fprintf (stderr, "Error opening file %s: %s\n", argv[2],
                 strerror (errno));
        return 1;
    }

    fscanf (fp0, "%d,%d,%d,%d,%d", &w, &h, &x, &y, &d);
    fprintf (fp1, "%d,%d,%d,%d,%d,", w, h, x, y, d);

    int c0, c1;
    int i;

    while ((c0 = fgetc(fp0)) != EOF) {
        if (c0 < '0' || c0 > '7') {
            continue;
        }

        i = 1;
        while ((c1 = fgetc(fp0)) != EOF) {
            if (c1 < '0' || c1 > '7') {
                continue;
            }
            if (c1 != c0) {
                ungetc(c1, fp0);
                break;
            }
            i++;
        }

        if (i == 1) {
            fputc(ov(c0), fp1);
        } else if (i==2) {
            fputc(ov(c0), fp1);
            fputc(ov(c0), fp1);        
        } else {
            fprintf(fp1, "%X%c", i, ov(c0));
        }

        if (c1 == EOF) {
            break;
        }
    }
    fputc('\n', fp1);
    fclose (fp1);
    fclose (fp0);

    return 0;
}
