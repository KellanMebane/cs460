/********** cat.c file *************/
#include "../kernel/ucode.c"

void cute_cat();

int main(int argc, char *argv[]) {
    int f = -1, n = 1;
    char buf[256] = "";
    char byte[1] = "";
    // printf("**************************************\n", -1);
    // printf("This is Kellan's version of kitty cat!\n", -1);
    // printf("**************************************\n\n\n", -1);

    // if no filename
    if (argc < 2) {
        // cute_cat();
        // prints("usage: `cat [filename]`\n");
        f = 0;
        while (1) {
            gets(buf);
            if (strcmp(buf, "qt_cat") == 0) {
                cute_cat();
            }
            printf("%s", buf, -1);
            //printf("%d\n", buf[0]);
        }

        /*while (read(0, byte, 1)) {
            // check for enter line and add the line in stdout
            if (byte[0] == '\n' || byte[0] == '\r') {
                write(1, "\n", 1);
                write(1, "\r", 1);
            }else {
                write(1, byte[0], 1);
            }
            // write(stdout, &userch, 1);
        }*/

        return 0;
    }

    // open file
    f = open(argv[1], O_RDONLY);

    // isn't a file
    if (f < 0) {
        printf("file `%s` not found\n", argv[1]);
        return 0;
    }

    // printf("opened on descriptor %d\n", f);

    // else file is found

    // printf("%s", buf);

    while (n > 0) {
        memset(buf, 0, 256);
        n = read(f, buf, 256);
        printf("%s", buf);
    }

    close(f);

    return 0;
}

void cute_cat() {
    printf("             *     ,MMM8&&&.            *\n", -1);
    printf("                  MMMM88&&&&&    .\n", -1);
    printf("                 MMMM88&&&&&&&\n", -1);
    printf("     *           MMM88&&&&&&&&\n", -1);
    printf("                 MMM88&&&&&&&&\n", -1);
    printf("                 'MMM88&&&&&&'\n", -1);
    printf("                   'MMM8&&&'      *\n", -1);
    printf("          |\\___/| \n", -1);
    printf("          )     (             .              '\n", -1);
    printf("         =\\     /= \n", -1);
    printf("           )===(       * \n", -1);
    printf("          /     \\ \n", -1);
    printf("          |     |\n", -1);
    printf("         /       \\ \n", -1);
    printf("         \\       /\n", -1);
    printf("  _/\\_/\\_/\\__  _/_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_ \n", -1);
    printf("  |  |  |  |( (  |  |  |  |  |  |  |  |  |  |\n", -1);
    printf("  |  |  |  | ) ) |  |  |  |  |  |  |  |  |  |\n", -1);
    printf("  |  |  |  |(_(  |  |  |  |  |  |  |  |  |  |\n", -1);
    printf("  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |\n", -1);
    printf("  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |\n", -1);
}