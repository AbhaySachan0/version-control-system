
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // access()
#include <sys/stat.h> // mkdir()
#include <linux/limits.h>
#include <fcntl.h>
#include <openssl/sha.h>



typedef struct{
    char* repo_root; // repo root path
    char* git_dir; // .git path
    char* object_dir;    // .git/objects
    char* ref_dir;      //.git/ref
    char* head_dir;
    char* head_file; //.git/HEAD
           
    char* index_dir;    //.git/index
}Paths;

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

int read_file(const char* path, char **content, size_t *file_size){
    struct stat st;
    if(stat(path,&st)==-1){
        perror("file");
        return -1;
    }

    size_t size = st.st_size;

    int fd = open(path, O_RDONLY);

    if(fd==-1) {
        perror("file");
        close(fd);
        return -1;
    }

    char* buffer = malloc(size);
    size_t total = 0;
    while(total <size) {
        size_t n = read(fd, buffer+total, size-total);
        if(n<=0) {
            perror("read");
            close(fd);
            return -1;
        }
        total += n;
    }
    close(fd);

    *content = buffer;
    *file_size = size;

    return 0;
}

void hash_content(char* hex,char *content, size_t size) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)content,size,hash);

    for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
        sprintf(hex+(i*2), "%02x", hash[i]);
    }
    hex[SHA_DIGEST_LENGTH*2] = '\0';

}

void add_file(const char* path) {
    char *content;
    size_t size;

    if(read_file(path, &content, &size) == -1){
        return;
    }
    // hash the content
    char hex[41];
    hash_content(&hex,content,size);
    //store content


    //update index

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
        add_directory(path);
    } else {
        printf("INVALID PATH");
    }
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
    
    struct stat st;

    return 0;
}