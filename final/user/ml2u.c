/********** mmore.c file *************/
#include "../kernel/ucode.c"

char upper(char c) {
    // is lowercase
    if (c >= 'a' && c <= 'z') {
        return c - 32;  // to upper
    }

    // otherwise return same character
    return c;
}

int main(int argc, char *argv[]) {
    int fi = -1, fo = -1, n = 0;
    char buf[1], input, bigbuf[256];
    printf("**************************************\n", -1);
    printf("This is Kellan's version of l2u!\n", -1);
    printf("**************************************\n\n\n", -1);

    // if no filename
    if (argc < 2) {
        while (1) {
            gets(bigbuf);
            for (n = 0; n < strlen(bigbuf); n++) {
                mputc(upper(bigbuf[n]));
            }
            printf("\n", -1);
        }

        return 0;
    }

    // open file
    fi = open(argv[1], O_RDONLY);
    fo = open(argv[2], O_WRONLY);

    if (fi < 0) {
        printf("%s: no such file...\n", argv[1]);
        return 0;
    }

    if (fo < 0) {
        printf("%s: unable to open or create file...\n", argv[2]);
        close(fi);
        return 0;
    }

    while (1) {
        n = read(fi, buf, 1);   // read 1 byte
        if (n == 0) return 0;   // break on eof
        input = upper(buf[0]);  // toupper
        // mputc(input);
        write(fo, &input, 1);  // write uppercase to outfile
    }

    close(fi);
    close(fo);

    return 0;
}