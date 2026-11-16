
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 2 || argc >= MAXARG) {
        exit(1);
    }

    char *new_argv[MAXARG];
    char buf[128];
    char *p = buf;
    char *start = buf;
    char c;

    // 1. 复制原来的 argv
    for (int i = 1; i < argc; i++) {
        new_argv[i - 1] = argv[i];
    }

    int k = argc - 1;

    // 2. 逐字符读取标准输入
    while (read(0, &c, 1) == 1) {

        // 遇到空格
        if (c == ' ') {
            if (start != p) {
                if (k >= MAXARG - 1) {
                    exit(1);
                }

                *p = '\0';
                new_argv[k++] = start;
                p++;
                start = p;
            }
        }

        // 遇到换行
        else if (c == '\n') {
            if (start != p) {
                if (k >= MAXARG - 1) {
                    exit(1);
                }

                *p = '\0';
                new_argv[k++] = start;
            }

            new_argv[k] = 0;

            if (k > argc - 1) {
                int pid = fork();

                if (pid < 0) {
                    exit(1);
                }

                if (pid == 0) {
                    exec(new_argv[0], new_argv);
                    exit(1);
                } else {
                    wait(0);
                }
            }

            // 重置指针和参数位置
            p = buf;
            start = buf;
            k = argc - 1;
        }

        // 普通字符
        else {
            if (p >= buf + 127) {
                exit(1);
            }

            *p = c;
            p++;
        }
    }

    // 3. 处理最后一行没有换行符的情况
    if (start != p) {
        if (k >= MAXARG - 1) {
            exit(1);
        }

        *p = '\0';
        new_argv[k++] = start;
    }

    if (k > argc - 1) {
        new_argv[k] = 0;

        int pid = fork();

        if (pid < 0) {
            exit(1);
        }

        if (pid == 0) {
            exec(new_argv[0], new_argv);
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}

