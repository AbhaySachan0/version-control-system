#include "repo.h"
#include "object.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>


int repo_exist(char* path){
    return access(path,F_OK)==0;
}

static char* allocate_path(void) {
    char* p = malloc(4096);

    if(!p){
        perror("malloc");
        exit(1);
    }
    return p;
}

void initialize_path(Paths *p, const char *root) {
    p->repo_root = allocate_path();
    p->git_dir = allocate_path();
    p->object_dir = allocate_path();
    p->ref_dir = allocate_path();
    p->head_dir = allocate_path();
    p->head_file = allocate_path();
    p->index_dir = allocate_path();
    
    
    snprintf(p->repo_root,4096,"%s",root);
    snprintf(p->git_dir,4096,"%s/.myvc",root);
    snprintf(p->object_dir,4096,"%s/.myvc/objects",root);
    snprintf(p->ref_dir,4096,"%s/.myvc/refs",root);
    snprintf(p->head_dir,4096,"%s/.myvc/refs/heads",root);
    snprintf(p->head_file,4096,"%s/.myvc/HEAD",root);
    snprintf(p->index_dir,4096,"%s/.myvc/index",root);
    
}

void make_dir(const char *path) {
    if(mkdir(path,0755) == -1){
        perror("mkdir");
        return;
    }
    // printf("%s\n",path);
}

void process_path(const char* path) {
    if(access(path,F_OK)!=0){
        perror("file");
        return;
    }

    struct stat st;

    if(stat(path,&st)==-1){
        perror(path);
        return;
    }

    if(S_ISREG(st.st_mode)){
        add_file(path);
    } else if (S_ISDIR(st.st_mode)) {
        // add_directory(path);
        printf("Directory\n");
    } else {
        printf("INVALID PATH");
    }
}

void init_repo(Paths *p) {
    
    if(repo_exist(".myvc")){
        printf("Repo already exists\n");
        return;
    }
    
    make_dir(p->git_dir);
    make_dir(p->object_dir);
    make_dir(p->index_dir);
    make_dir(p->ref_dir);
    make_dir(p->head_dir);
    
    printf("Initialized empty repository\n");
}

void add(int argc,char* argv[]){

    if(!repo_exist(".myvc")){
            printf("Not a repo\n");
        return;
    }  

    for(int i=2;i<argc;i++){
        process_path(argv[i]);
    }

}