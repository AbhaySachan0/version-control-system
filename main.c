#include "repo.h"
#include "object.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>

int main(int argc, char* argv[]) {

    char resolved_path[PATH_MAX];
    realpath(".",resolved_path);
    // printf("%s\n",resolved_path);

    Paths p;

    initialize_path(&p,resolved_path);
    // printf("%s\n",p.git_dir);

    if(argc < 2) {
        fprintf(stderr,"Usage: %s No arguments\n",argv[0]);
        return 1;
    }    
    
    if(strcmp(argv[1],"init") == 0) {
        init_repo(&p);
    }
    else if (strcmp(argv[1],"add")==0) {
        add(argc,argv);
    }
    
    return 0;
}