#include "kernel/types.h"
#include "user/user.h"

int
main() {
        int p1[2];
        int p2[2];
        if (pipe(p1) < 0) {
                fprintf(2, "pipe created failed\n");
                exit(1);
        }

        if (pipe(p2) < 0) {
                fprintf(2, "pipe created failed\n");
                exit(1);
        }

        int pid = fork();
        if (pid == 0) {
                close(p1[1]);
                close(p2[0]);
                char buf[1];
                int n = read(p1[0], buf, 1);
                if (n == 1){
                        int pid_c = getpid();
                        fprintf(1, "%d: received ping\n", pid_c);
                        write(p2[1], "l", 1);
                        close(p1[0]);
                        close(p2[1]);
                        exit(0);
                }
        } else if (pid > 0){
                int status;
                close(p1[0]);
                close(p2[1]);
                write(p1[1], "l", 1);
                char buf[1];
                int n = read(p2[0], buf, 1);
                if (n == 1) {
                        int pid_p = getpid();
                       fprintf(1, "%d: received pong\n", pid_p);
                        close(p1[1]);
                        close(p2[0]);
                }
                wait(&status);
        } else {
                fprintf(2, "fork failed\n");
                exit(1);
        }
        exit(0);
}
