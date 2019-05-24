/********** mls.c file *************/
#include "../kernel/ucode.c"

STAT *stuff;
char a[128];

int fg;

main(int argc, char const *argv[]) {
    int f = -1, n = 1;
    int i = 0;
    char buf[1024] = "", *cp, fullname[1024], entry[256];
    DIR *dp;
    char *t1 = "xwrxwrxwr-------";
    char *t2 = "----------------";
    char local[256];

    // args empty vs not
    if (argc > 1) {
        strcpy(local, argv[1]);
        f = open(local, O_RDONLY);
    } else {
        f = open(".", O_RDONLY);
    }

    n = read(f, buf, 1024);

    fstat(f, a);
    stuff = (STAT *)a;

    // if dir
    if ((stuff->st_mode & 0xF000) == 0x4000) {
        while (n > 0) {
            cp = buf;
            // dp = (DIR *)cp;
            while (n > 0) {
                dp = (DIR *)cp;

                for (i = 0; i < dp->name_len; i++) {
                    entry[i] = dp->name[i];
                }
                entry[i] = 0;
                strcpy(fullname, local);
                strcat(fullname, "/");
                strcat(fullname, entry);

                // printf("%s\n", entry, -1);

                fg = open(fullname, O_RDONLY);
                fstat(fg, a);
                // printf("%s\n", a, -1);
                stuff = (STAT *)a;

                if ((stuff->st_mode & 0xF000) == 0x8000) printf("%c", '-', -1);
                if ((stuff->st_mode & 0xF000) == 0x4000) printf("%c", 'd', -1);
                if ((stuff->st_mode & 0xF000) == 0xA000) printf("%c", 'l', -1);

                for (i = 8; i >= 0; i--) {
                    if (stuff->st_mode & (1 << i))
                        printf("%c", t1[i], -1);
                    else
                        printf("%c", t2[i], -1);
                }

                printf(" %d ", stuff->st_nlink);
                printf("%d ", stuff->st_gid);
                printf("%d ", stuff->st_uid);
                printf("%d ", stuff->st_size);

                printf("%s\n", entry, -1);
                close(fg);

                cp += dp->rec_len;
                n -= dp->rec_len;
            }
            n = read(f, buf, 1024);
        }
        close(f);
    } else {
        fstat(f, a);
        // printf("%s\n", a, -1);
        stuff = (STAT *)a;

        if ((stuff->st_mode & 0xF000) == 0x8000) printf("%c", '-', -1);
        if ((stuff->st_mode & 0xF000) == 0x4000) printf("%c", 'd', -1);
        if ((stuff->st_mode & 0xF000) == 0xA000) printf("%c", 'l', -1);

        for (i = 8; i >= 0; i--) {
            if (stuff->st_mode & (1 << i))
                printf("%c", t1[i], -1);
            else
                printf("%c", t2[i], -1);
        }

        printf(" %d ", stuff->st_nlink);
        printf("%d ", stuff->st_gid);
        printf("%d ", stuff->st_uid);
        printf("%d ", stuff->st_size);

        printf("%s\n", local, -1);
        close(f);
    }

    return 0;
}
