/********** login.c file *************/
#include "../kernel/ucode.c"

int status;
char hdir[128] = "";

int has_redirect(char lines[16][64], int size, char* full_line) {
    int i = 0;
    int fd;

    // printf("size is %d\n", size);

    for (i = 0; i < size; i++) {
        // printf("has: %s\n", lines[i]);
        if (strcmp(lines[i], ">") == 0) {
            // open next slot as file
            fd = open(lines[i + 1], O_WRONLY | O_CREAT);
            dup2(fd, 1);
            exec(full_line);
            return 0;
        } else if (strcmp(lines[i], ">>") == 0) {
            fd = open(lines[i + 1], O_APPEND | O_WRONLY | O_CREAT);
            dup2(fd, 1);
            exec(full_line);
            return 0;
        } else if (strcmp(lines[i], "<") == 0) {
            fd = open(lines[i + 1], O_RDONLY);
            dup2(fd, 0);
            exec(full_line);
            return 0;
        }
    }
    return 0;
}

int has_pipe(char lines[16][64], int size) {
    int i = 0;

    // printf("size is %d\n", size);

    for (i = 0; i < size; i++) {
        // printf("has: %s\n", lines[i]);
        if (strcmp(lines[i], "|") == 0) {
            return i;  // return pipe position
        }
    }
    return 0;
}

int do_pipe(char lines[16][64], int loc) {
    int pid, pd[2];
    char cmd1[128] = "", cmd2[128] = "";
    char newlines[16][64] = {"", ""};
    int j = 0;
    int k = 0;
    int z = 0;
    int loco = 0;
    int red = 0;

    for (j = 0; j < loc; j++) {
        strcat(cmd1, lines[j]);
        strcat(cmd1, " ");
    }

    // memset(newlines, 0, 384);
    for (z = 0; z < 16; z++) {
        memset(newlines[z], 0, 64);
    };

    // printf("loc = %d\n", loc);

    j = loc + 1;

    // create second cmd and recreate left over inputs
    for (j = loc + 1; lines[j][0] != 0; j++) {
        strcat(cmd2, lines[j]);
        strcat(cmd2, " ");

        strcpy(newlines[k++], lines[j]);  // rest of the input afterfirst pipe
    }

    // printf("cmd1: %s\ncmd2: %s\n", cmd1, cmd2);

    // pipe and fork
    pipe(pd);
    pid = fork();

    if (pid) {
        // parent opens reader pipe
        close(pd[1]);
        dup2(pd[0], 0);

        // look for more pipes. if more, call do_pipe again
        loco = has_pipe(newlines, k);
        // printf("loco is %d\n", loco);
        if (loco) {
            do_pipe(newlines, loco);
        } else {
            has_redirect(newlines, k, cmd2);
            exec(cmd2);
        }
    } else {
        // child writes into pipe
        close(pd[0]);
        dup2(pd[1], 1);
        has_redirect(lines, loc - 1, cmd1);
        exec(cmd1);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    int i = 0, j = 0, z = 0;
    // int fd;
    int pid = 0;
    int loc;
    // int red = 0;

    char cmdLine[128] = "", copy[128], cwd[128] = "";
    char rebuild[128] = "";
    char tokens[16][64] = {"", ""};
    char* tkn;

    getcwd(hdir);  // store initial directory

    while (1) {
        i = 0;
        j = 0;
        z = 0;
        getcwd(cwd);
        tkn = 0;
        pid = 0;
        loc = 0;
        printf("%s@%d $ ", cwd, getpid());
        // memset(tokens, 0, 384);
        // memset(tokens, 0, sizeof(tokens[0][0] * 16 * 64));
        for (z = 0; z < 16; z++) {
            memset(tokens[z], 0, 64);
        }
        memset(cmdLine, 0, 128);
        memset(rebuild, 0, 128);
        memset(copy, 0, 128);
        gets(cmdLine);
        strcpy(copy, cmdLine);

        // close("out");

        // printf("cmd line: %s\n", cmdLine);

        // break down cmd
        tkn = mstrtok(copy, " ");
        while (tkn) {
            strcpy(tokens[i++], tkn);
            tkn = mstrtok(0, " ");
        }

        if (strcmp(tokens[0], "cd") == 0) {
            if (tokens[1][0] == 0) {
                chdir(hdir);
            } else
                chdir(tokens[1]);
        } else if (strcmp(tokens[0], "logout") == 0) {
            exit(0);
        } else if (i == 0) {
            continue;  // ignore empty input
        } else {
            // real commands
            pid = fork();
            if (pid < 0) {
                exit(0);
            }

            if (pid) {
                // parent
                pid = wait(&status);
            } else {
                // printf("we are here\n", -1);
                // child code
                loc = has_pipe(tokens, i);
                // printf("loc is %d\n", loc);
                // no pipe
                if (loc == 0) {
                    // look for redirect

                    for (j = 0; tokens[j][0] != 0; j++) {
                        strcat(rebuild, tokens[j]);
                        strcat(rebuild, " ");
                    }

                    // exec(rebuild);  // run command if no pipe
                    has_redirect(tokens, i, rebuild);
                    exec(rebuild);

                } else {
                    // do pipe
                    do_pipe(tokens, loc);
                }
                exit(0);
            }
        }

        memset(cwd, 0, 128);
    }
    return 0;
}
