#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

void writeOrganizationSection(FILE *outputFile, FileInfo *fileInfo, int numFiles);

void mergeFiles(int argc, char *argv[], const char *outputFileName) {
    FILE *outputFile = fopen(outputFileName, "wb");

    if (!outputFile) {
        printf("Error: Could not open output file for writing.\n");
        exit(1);
    }

    size_t totalSize = getTotalSize(argc, argv);
    if (totalSize > MAX_TOTAL_SIZE) {
        printf("Error: Total size of input files exceeds the limit (200 MB).\n");
        fclose(outputFile);
        exit(1);
    }

    FileInfo fileInfo[MAX_FILES];
    int numFiles = argc - 4;  // Number of input files

    // Writing the organization section
    writeOrganizationSection(outputFile, fileInfo, numFiles);

    for (int i = 4; i < argc - 1; ++i) {
        checkFileCompatibility(argv[i]);

        FILE *inputFile = fopen(argv[i], "rb");

        if (!inputFile) {
            printf("Error: Could not open input file %s for reading.\n", argv[i]);
            fclose(outputFile);
            exit(1);
        }

        char buffer[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) {
            fwrite(buffer, 1, bytesRead, outputFile);
        }

        fclose(inputFile);
    }

    fclose(outputFile);
}

