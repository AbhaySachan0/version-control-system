
#include <stddef.h>

void blob_object(const char* content, size_t content_size,
                 char *hex, char** out_buf, size_t *out_size);

void store_object(char* hex, char* content, size_t size);

void add_file(const char* path);