#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "merge.h"
#include "extract.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s -b outputfile file1 file2 ... OR %s -a archivefile extractpath\n", argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0) {
        size_t totalSize = getTotalSize(argc, argv);

        if (totalSize > MAX_TOTAL_SIZE) {
            printf("Error: Total size of input files exceeds the limit (200 MB).\n");
            return 1;
        }

        mergeFiles(argc, argv, (argc > 5) ? argv[argc - 1] : "a.sau");
        printf("The files have been merged.\n");
    } else if (strcmp(argv[1], "-a") == 0) {
        extractFiles(argv[2], argv[3]);
        printf("Files opened in the %s directory.\n", argv[3]);
    } else {
        printf("Error: Invalid option.\n");
        return 1;
    }

    return 0;
}
