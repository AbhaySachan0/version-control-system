#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if(argc < 2) {
        printf("No Arguments...\n");
        // return 1;
    }
    
    if(strcmp(argv[1],"init") == 0) {

        printf("Init recognised\n");
    }

    return 0;
}