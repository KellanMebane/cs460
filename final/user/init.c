/********** test.c file *************/
#include "../kernel/ucode.c"

// globals for special files
int console;
int s0;
int s1;

int parent() {
    int pid, status;

    // rest until children die, then reopen them.
    while (1) {
        pid = wait(&status);
        printf("INIT : wait for zombie child\n");

        if (pid == s0) {
            s0 = fork();
            if (s0)
                continue;
            else
                exec("login /dev/ttyS0");
        }

        if (pid == s1) {
            s1 = fork();
            if (s1)
                continue;
            else
                exec("login /dev/ttyS1");
        }

        if (pid == console) {
            console = fork();
            if (console)
                continue;
            else
                exec("login /dev/tty0");
        }
    }
}

int main(int argc, char *argv[]) {
    int in, out;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);
    // int mypid = getpid();
    printf("\n------------------", -1);
    printf("\nKellan Mebane INIT", -1);
    printf("\n------------------", -1);
    printf("\nINIT: fork a login proc on console\n", -1);

    // fork 3 login programs
    console = fork();
    if (console) {
        // parent
        s0 = fork();
        if (s0) {
            // parent
            s1 = fork();
            if (!s1) exec("login /dev/ttyS1");  // child runs
        } else
            exec("login /dev/ttyS0");  // child runs
    } else
        exec("login /dev/tty0");  // child runs

    // parent
    if (console && s1 && s0) parent();
}