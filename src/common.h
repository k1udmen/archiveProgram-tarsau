#ifndef COMMON_H
#define COMMON_H

#include <sys/stat.h>

#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 32

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    mode_t permissions;
    size_t size;
} FileInfo;

void checkFileCompatibility(const char *filename);
size_t getTotalSize(int argc, char *argv[]);

#endif
