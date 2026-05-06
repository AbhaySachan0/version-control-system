
// includes
#include "config.h"
#include "index.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h> // read(), write(),close()
#include<fcntl.h> // open()


static void create_empty_index() {
    
    int fd = open(INDEX_FILE_PATH, O_WRONLY | O_CREAT, 0644);

    if(fd==-1) {
        perror("index");
        exit(1);
    }

    IndexHeader header;
    memcpy(header.signature, "MYVC", 4);
    header.version = 1;
    header.entry_count = 0;

    write(fd, &header, sizeof(header));
    close(fd);
}


static IndexHeader read_header(int fd) {
    IndexHeader header;
    read(fd, &header, sizeof(IndexHeader));

    if(strncmp(header.signature, "MYVC", 4)!=0) {
        fprintf(stderr, "Invalid index file\n");
        exit(1);
    }

    return header;
}

static void write_entry(int fd, IndexEntry *entry) {

    write(fd, &entry->mode, sizeof(uint32_t));

    write(fd, entry->hash, 20);

    write(fd, &entry->path_len, sizeof(uint16_t));

    write(fd, entry->path, entry->path_len);
}

static IndexEntry read_entry(int fd) {
    IndexEntry entry;

    read(fd, &entry.mode, sizeof(uint32_t));
    read(fd, entry.hash, 20);
    read(fd, &entry.path_len, sizeof(uint16_t));

    entry.path = malloc(entry.path_len+1);
    read(fd, entry.path, entry.path_len);
    entry.path[entry.path_len] = '\0';

    return entry;
}