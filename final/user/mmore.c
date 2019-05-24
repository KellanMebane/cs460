/********** mmore.c file *************/
#include "../kernel/ucode.c"

void more_print(int fd, int rows) {
    int i = 0;
    char buf[1];

    for (i = 0; i < rows; i++) {
        while (1) {
            if (read(fd, buf, 1) == 0) {
                exit(0);
            }
            mputc(buf[0]);
            if (buf[0] == '\n') {
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int f = -1, i = 0;
    char buf[1], input, bigbuf[256];
    printf("**************************************\n", -1);
    printf("This is Kellan's version of more!\n", -1);
    printf("**************************************\n\n\n", -1);

    // if no filename
    if (argc < 2) {
        /*while (1) {
            input = getc();
            if (input == '\n' || input == '\r') {
                printf("%s\n", bigbuf);
                memset(bigbuf, 0, 256);
                i = 0;
            }
            bigbuf[i++] = input;
        }*/
        f = dup(0);
        close(0);
    } else
        f = open(argv[1], O_RDONLY);

    // isn't a file
    if (f < 0) {
        printf("file `%s` not found\n", argv[1]);
        return 0;
    }

    while (1) {
        input = getc();
        if (input == ' ') {
            more_print(f, 20);
        } else if (input == '\n' || input == '\r') {
            more_print(f, 1);
        } else if (input == 'q') {
            exit(0);
        }
    }

    close(f);

    return 0;
}