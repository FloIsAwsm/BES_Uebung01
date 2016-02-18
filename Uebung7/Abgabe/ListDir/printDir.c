#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

// Prints directories recursivly
void printDirectories(const char *name, int level)
{
    DIR *dir;
    struct dirent *item;

    if (!(dir = opendir(name))){
        printf( "Errorcode: %d, Message:%s\n", errno, strerror(errno) );
    }


    while ((item = readdir(dir)))
    {
        if (item->d_type == DT_DIR)
        {
            char path[PATH_MAX];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, item->d_name);
            path[len] = 0;
            if (!(strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)) {
                printf("|");
                printf("%*s---[%s]\n", level*2, "", item->d_name);
                printDirectories(path, level + 1);
            }
        }
        else
        {
            printf("|");
            printf("%*s|--- [%s]\n", level*2, "", item->d_name);
        }
    }
    closedir(dir);
}
