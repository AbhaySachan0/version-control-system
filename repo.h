#include <stddef.h>

typedef struct{
    char* repo_root; // repo root path
    char* git_dir; // .git path
    char* object_dir;    // .git/objects
    char* ref_dir;      //.git/ref
    char* head_dir;
    char* head_file; //.git/HEAD
           
    char* index_dir;    //.git/index
}Paths;

void initialize_path(Paths *p, const char *root);
void init_repo(Paths *p);
int repo_exist(char* path);
void add(int argc, char* argv[]);