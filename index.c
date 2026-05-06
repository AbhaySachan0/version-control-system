
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

IndexEntry* read_index(uint32_t *count) {
    if(access(INDEX_FILE_PATH, F_OK)==-1) {
        create_empty_index();
    }

    int fd = open(INDEX_FILE_PATH, O_RDONLY);
    if(fd==-1) {
        perror("index");
        exit(1);
    }

    IndexHeader header = read_header(fd);

    *count = header.entry_count;

    IndexEntry *entries = malloc(sizeof(IndexEntry) * (*count));

    for(uint32_t i=0; i<*count; i++) {
        entries[i] = read_entry(fd);
    }
    close(fd);
    return entries;
}

void write_index(IndexEntry *entries, uint32_t count) {
    int fd = open(INDEX_FILE_PATH, O_WRONLY | O_TRUNC);
    if(fd==-1) {
        perror("index");
        exit(1);
    }

    IndexHeader header;

    memcpy(header.signature, VCS_HEADER, 4);
    header.version = 1;
    header.entry_count = count;

    write(fd, &header, sizeof(IndexHeader));

    for(uint32_t i=0; i<count; i++) {
        write_entry(fd, &entries[i]);
    }
    close(fd);
}


void free_entries(IndexEntry *entries, uint32_t count) {

    for(uint32_t i = 0; i < count; i++) {
        free(entries[i].path);
    }

    free(entries);
}

// helper
static int find_entry(IndexEntry *entries, uint32_t count, const char* path){
    for(uint32_t i=0;i<count;i++) {
        if(strcmp(entries[i].path, path)==0) {
            return i;
        }
    }
    return -1;
}


void update_index(const char* path, unsigned char hash[20]){
    uint32_t count;
    IndexEntry *entries = read_index(&count); // load entire entries into ram
    int pos = find_entry(entries,count,path); // search existing paths

    if(pos!=-1) { // exists -> update index
        memcpy(entries[pos].hash, hash, 20); // update hash

    } else { // new entry
        entries = realloc(entries, sizeof(IndexEntry)*(count+1));
        
        IndexEntry *new_entry = &entries[count]; // pointer to last new slot

        new_entry->mode = 100644;
        memcpy(new_entry->hash, hash, 20);

        new_entry->path_len = strlen(path);

        new_entry->path = malloc(new_entry->path_len+1);

        strcpy(new_entry->path, path);
        count++;
    }
    write_index(entries, count);
    free_entries(entries, count);
}