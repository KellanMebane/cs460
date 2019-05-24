/********** login.c file *************/
#include "../kernel/ucode.c"

int in, out, err, p;
char username[128] = "", password[128] = "";

void checkpassword(char *line, char *n, char *ps) {
    char vector[7][128];
    int i = 0, j = 0;
    int uid, gid;
    char home[128];

    // parse password line
    while (*line) {
        if (*line != ':') {
            vector[i][j] = *line;
            j++;
        } else {
            vector[i][j++] = 0;
            i++;
            j = 0;
        }

        line++;
    }
    // null terminate last line
    vector[i][j] = 0;

    // validate
    if ((strcmp(n, vector[0]) == 0) && (strcmp(ps, vector[1]) == 0)) {
        // printf("m a t c h !\n", -1);
        gid = atoi(vector[2]);
        uid = atoi(vector[3]);

        chuid(uid, gid);
        chdir(vector[5]);
        close(p);
        exec(vector[6]);
    }
}

int main(int argc, char *argv[]) {
    char buf[256] = "";
    char line[256] = "";
    int rd = 0, i = 0;
    char *base = 0, *newline = 0;
    // close fd 0 and 1
    close(0);
    close(1);

    // open argv[1] 3 times as in, out, err
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);

    // set tty name string
    settty(argv[1]);

    // open passwd file
    p = open("/etc/passwd", O_RDONLY);

    // make sure it's valid
    if (p != -1) {
        printf("OPENED FILE DESC %d\n", p);
        // read file
        rd = read(p, buf, 256);
    }

    while (1) {
        printf("km login: ", -1);
        gets(username);
        printf("password: ", -1);
        gets(password);
        base = &buf[0];
        i = 0;

        for (i == 0; i < rd; i++) {
            if (buf[i] == 10 || buf[i] == 0) {
                buf[i] = 0;
                checkpassword(base, username, password);
                base = &buf[i + 1];
            }
        }

        printf("wrong login or password...\n", -1);
    }
}