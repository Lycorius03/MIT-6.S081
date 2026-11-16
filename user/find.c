#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

char*
fmtname(char *path) {
  char *p = path + strlen(path);

  while (p > path && *(p - 1) != '/') {
    p--;
  }

  return p;
}

void
find(char *dp, char *fp) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(dp, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", dp);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", dp);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE: {
                   char *fmt_dp = fmtname(dp);

                   if (strcmp(fmt_dp, fp) == 0) {
                     printf("%s\n", dp);
                   }
                   break;
                 }

    case T_DIR: {
                  if (strlen(dp) + 1 + DIRSIZ + 1 > sizeof buf){
                    printf("find: path too long\n");
                    break;
                  }
                  strcpy(buf, dp);
                  p = buf+strlen(buf);
                  *p++ = '/';
                  while(read(fd, &de, sizeof(de)) == sizeof(de)){
                    if (de.inum == 0) {
                      continue;
                    }
                    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                      continue;
                    }
                    memmove(p, de.name, DIRSIZ);
                    p[DIRSIZ] = 0;
                    find(buf, fp);
                  }
                }
  }

  close(fd);
}

int
main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "usage: find <path> <filename>\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}

