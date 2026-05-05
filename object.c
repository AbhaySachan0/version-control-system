
#include "object.h"
#include "utils.h"

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>


// SHA-1 hasing algorithm
static void hash_content(char* hex,char *content, size_t size) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)content,size,hash);

    for(int i=0;i<SHA_DIGEST_LENGTH;i++) {
        sprintf(hex+(i*2), "%02x", hash[i]);
    }
    hex[SHA_DIGEST_LENGTH*2] = '\0';

    // printf("%s\n",hex);
}

// creating blob object
void blob_object(const char* content, size_t content_size, char *hex, char** out_buf, size_t *out_size) {
    char header[100];
    int header_len = snprintf(header, sizeof(header), "blob %zu", content_size);

    size_t total = content_size + header_len +1; // total size of object

    char* buffer = malloc(total); // buffer for complete object
    if(!buffer){
        perror("malloc");
        exit(1);
    }

    memcpy(buffer,header,header_len);  // excludes the null terminator
    buffer[header_len] = '\0';
    memcpy(buffer+header_len+1, content, content_size); // cpy content to buffer

    hash_content(hex,buffer,total); // hash the object into hex

    *out_buf = buffer;
    *out_size = total;

}

//storing blob object into disk
void store_object(char* hex, char* content, size_t size) {
    //first two char -> dir name
    char dir[3];
    strncpy(dir,hex,2);
    dir[2] = '\0';

    // full dir path
    char dir_path[PATH_MAX];

    snprintf(dir_path,PATH_MAX,".myvc/objects/%s",dir);
    printf("store objects\n");

    if(access(dir_path,F_OK)==-1) { // folder doesn't exist
        if(mkdir(dir_path,0755)==-1) {
            perror("mkdir");
            return;
        }
    }
    
    printf("objects\n");
    
    // file name -> after two chars
    const char* file = hex +2;
    char obj_path[PATH_MAX];

    // snprintf(obj_path,sizeof(obj_path),"%s/%s",dir_path,file);
    int n = snprintf(obj_path, sizeof(obj_path),".myvc/objects/%c%c/%s",hex[0], hex[1], hex + 2); 

    if(access(obj_path,F_OK)==0) { // for duplicate storage check
        return;
    }

    // open a file
    int fd = open(obj_path,O_WRONLY | O_CREAT | O_EXCL,0644);
    if(fd==-1) {
        perror("open");
        return;
    }
    size_t total = 0;

    // write content to a file
    while(total<size) {
        ssize_t n = write(fd,content+total,size-total);
        if(n<=0) {
            perror("write");
            close(fd);
            return;
        }
        total += n;
    }
    close(fd);
    printf("store objects\n");

}

// adding file
void add_file(const char* path) {
    char *content;
    size_t size;

    if(read_file(path, &content, &size) == -1){
        return;
    }
    // hash the content
    char hex[41];

    char *blob_buf;
    size_t blob_size;

    blob_object(content, size, hex, &blob_buf, &blob_size);

    //store content
    store_object(hex,blob_buf,blob_size);
    
    
    //update index
    
    free(blob_buf);
    free(content); 
}

// add folders