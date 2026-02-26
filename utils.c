#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int read_file(const char *path, char **content, size_t *file_size){
    struct stat st;
    if(stat(path,&st)==-1){
        perror("file");
        return -1;
    }

    size_t size = st.st_size;

    int fd = open(path, O_RDONLY);

    if(fd==-1) {
        perror("file");
        return -1;
    }

    char* buffer = malloc(size);
    size_t total = 0;
    while(total <size) {
        ssize_t n = read(fd, buffer+total, size-total);
        if(n<=0) {
            perror("read");
            free(buffer);
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
