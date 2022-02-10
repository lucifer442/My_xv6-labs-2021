#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLEN 32

int main(int argc, char *argv[])
{
    char *path = "echo";
    char buf[MAXLEN * MAXARG] = {0}, *p;
    char *params[MAXARG];
    int oriParamCnt = 0;
    int paramIdx;
    int paramLen;
    int i;

    char ch;
    int res;

    if (argc > 1) {
        path = argv[1];
        for (i = 1; i < argc;i++) {
            params[oriParamCnt++] = argv[i];
        }
    } else {
        params[oriParamCnt++] = path;
    }

    paramIdx = oriParamCnt;
    p = buf;
    paramLen = 0;
    while (1) {
        res = read(0, p, 1);
        ch = *p;

        if (res != 0 && ch != ' ' && ch != '\n') {
            ++paramLen;
            ++p;
            continue;
        }

        params[paramIdx++] = p - paramLen;

        paramLen = 0;

        *p = 0;
        ++p;

        if (paramIdx == MAXARG && ch == ' ') {
            while ((res = read(0, &ch, 1)) != 0) {
                if (ch == '\n') {
                    break;
                }
            }
        }
        if (ch != ' ') {
            if (fork() == 0) {
                exec(path, params);
                exit(0);
            } else {
                wait((int *) 0);
                paramIdx = oriParamCnt;
                p = buf;
            }
        }
        if (res == 0) {
            break;
        }
    }
    exit(0);
}