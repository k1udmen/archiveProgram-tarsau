#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "common.h"

void readOrganizationSection(FILE *archiveFile, FileInfo *fileInfo, int *numFiles);

void extractFiles(const char *archiveFileName, const char *extractPath) {
    FILE *archiveFile = fopen(archiveFileName, "rb");

    if (!archiveFile) {
        printf("Error: Could not open archive file %s for reading.\n", archiveFileName);
        exit(1);
    }

    // Checking whether the archive file name is appropriate
    char *extension = strrchr(archiveFileName, '.');
    if (extension == NULL || strcmp(extension, ".sau") != 0) {
        printf("Error: Archive file is inappropriate or corrupt!\n");
        fclose(archiveFile);
        exit(1);
    }

    // Creating the extract directory if it doesn't exist
    struct stat st = {0};
    if (stat(extractPath, &st) == -1) {
        mkdir(extractPath, 0700);  // Creating directory with read, write, execute permissions for the owner
    }

    FileInfo fileInfo[MAX_FILES];
    int numFiles;
    readOrganizationSection(archiveFile, fileInfo, &numFiles);

    for (int i = 0; i < numFiles; ++i) {
        // Constructing the full path for the extracted file
        char extractFilePath[MAX_FILENAME_LENGTH + 100];  // Adjust the buffer size as needed
        snprintf(extractFilePath, sizeof(extractFilePath), "%s/%s", extractPath, fileInfo[i].filename);

        FILE *extractedFile = fopen(extractFilePath, "wb");

        if (!extractedFile) {
            printf("Error: Could not open output file %s for writing.\n", extractFilePath);
            fclose(archiveFile);
            exit(1);
        }

        char buffer[1024];
        size_t bytesRead;

        for (size_t bytesRemaining = fileInfo[i].size; bytesRemaining > 0; bytesRemaining -= bytesRead) {
            size_t chunkSize = (bytesRemaining > sizeof(buffer)) ? sizeof(buffer) : bytesRemaining;
            bytesRead = fread(buffer, 1, chunkSize, archiveFile);
            fwrite(buffer, 1, bytesRead, extractedFile);
        }

        fclose(extractedFile);
    }

    fclose(archiveFile);
}
