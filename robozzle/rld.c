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

static bool is_hex(int c) {
    return hv(c) >= 0;
}

static int cv(int c) {
    return c - 'G' + '0';
}

static bool is_sym(int c) {
    return c >= 'G' && c <= 'N';
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
    int c, count, sym;
    int k;

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

    k = w;

    while ((c = fgetc(fp0)) != EOF) {
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') continue;

        if (is_hex(c)) {
            count = hv(c);
            while ((c = fgetc(fp0)) != EOF && is_hex(c)) {
                count = count * 16 + hv(c);
            }
            if (c == EOF) break;

            if (is_sym(c)) {
                sym = cv(c);
                for (int i = 0; i < count; i++) {
                    fputc(sym, fp1);
                    enter(&k, w, fp1);
                }
            } else {
                ungetc(c, fp0);
            }
        } else if (is_sym(c)) {
            fputc(cv(c), fp1);
            enter(&k, w, fp1);
        }
    }

    fclose(fp1);
    fclose(fp0);

    return 0;
}

