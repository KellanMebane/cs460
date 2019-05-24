/********** mgrep.c file *************/
#include "../kernel/ucode.c"

int main(int argc, char *argv[]) {
    int f = -1, argi = 1, n = 0, i = 0;
    char buf[1], input;
    char pattern[128] = "";
    char linebuf[256] = "";
    char c[1], *lb = linebuf;
    printf("**************************************\n", -1);
    printf("This is Kellan's version of grep!\n", -1);
    printf("**************************************\n\n\n", -1);

    // if no filename
    if (argc < 3) {
        if (argc < 2) {
            printf("usage: `mgrep pattern [filename]`\n");
            return 0;
        }
        f = dup(0);
        close(0);
    } else
        f = open(argv[argc - 1], O_RDONLY);

    // isn't a file
    if (f < 0) {
        printf("file `%s` not found\n", argv[argc - 1]);
        return 0;
    }

    // string input
    if (argc > 3) {
        if ((argv[1][0] == '"') &&
            (argv[argc - 2][strlen(argv[argc - 2]) - 1] == '"')) {
            // correct string input style
            // concat the input string
            // remove first double quote
            strcat(pattern, ++argv[argi]);
            argv[argi]--;
            argi = 2;
            // copy args into pattern
            for (argi = 2; argi < argc - 1; argi++) {
                strcat(pattern, " ");
                strcat(pattern, argv[argi]);
            }
            pattern[strlen(pattern) - 1] = 0;

        } else {
            // incorrect pattern usage
            printf("usage: `mgrep \"string pattern\" [filename]`\n", -1);
            return 0;
        }
    } else {
        // 1 word pattern
        strcpy(pattern, argv[1]);
    }

    // visually checking if correct
    printf("pattern: %s\nfile: %s\n", pattern, argv[argc - 1]);

    while (1) {
        n = read(f, c, 1);
        if (n == 0) {
            close(f);
            return 0;
        }

        // if a line is complete
        if (c[0] == '\r' || c[0] == '\n') {
            lb = linebuf;
            // printf("%s\n", linebuf, -1); // debug print
            if (strstr(linebuf, pattern)) {
                printf("%s\n", linebuf, -1);
            }
            memset(linebuf, 0, 256);
        } else
            *lb++ = c[0];  // otherwise add to line
    }

    close(f);

    return 0;
}