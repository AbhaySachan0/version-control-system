#include <stdio.h>
#include <string.h>
#include <unistd.h> // access()
#include <sys/stat.h>


void init_repo() {
    
    if(access(".myvc",F_OK)==0){
        printf("Repo already exists\n");
        return;
    }
    
    if(mkdir(".myvc",0755) == -1){
        perror("mkdir");
        return;
    }
    printf("Initialized empty repository\n");
}

int main(int argc, char* argv[]) {

    if(argc < 2) {
        fprintf(stderr,"Usage: %s No commands\n",argv[0]);
        return 1;
    }
    
    if(strcmp(argv[1],"init") == 0) {
        init_repo();
    } 
    
    return 0;
}