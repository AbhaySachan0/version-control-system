#ifndef INDEX_H
#define INDEX_H

#include <stdint.h> // uint32_t, uint16_t

typedef struct {
    char signature[4];
    uint32_t version; // int can differ across systems
    uint32_t entry_count;
} IndexHeader;

typedef struct {
    uint32_t mode;
    unsigned char hash[20];
    uint16_t path_len;
    char *path;
} IndexEntry;

void update_index(const char *path, unsigned char hash[20]);

IndexEntry* read_index(uint32_t *count);

void write_index(IndexEntry *entries, uint32_t count);

void free_entries(IndexEntry *entries, uint32_t count);


#endif

