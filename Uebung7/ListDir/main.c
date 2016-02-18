#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "printDir.h"

int main(int argc, char** argv)
{

    if(argc < 2){
        printf("To few arguments, ussage: ListDir <dirName>");
        return EXIT_FAILURE;
    }

    if(strlen(argv[1]) > 0) {
        printf("[%s]\n", argv[1]);
    }

    //printf("[%s]\n", buf);
    printDirectories(argv[1], 0);
    return EXIT_SUCCESS;
}
