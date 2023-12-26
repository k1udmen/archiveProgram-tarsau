#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

  //Defining maximum number of files and the length of filename
#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 32

typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    mode_t permissions;
    size_t size;
} FileInfo;

// Function prototype for merging files into an archive
void mergeFiles(int argc, char *argv[], const char *outputFileName);

// Function prototype for checking the compatibility of a file
void checkFileCompatibility(const char *filename);

// Function prototype for calculating the total size of input files
size_t getTotalSize(int argc, char *argv[]);

// Function prototype for writing the organization section of the archive
void writeOrganizationSection(FILE *outputFile, FileInfo *fileInfo, int numFiles);

// Function prototype for reading the organization section during extraction
void readOrganizationSection(FILE *archiveFile, FileInfo *fileInfo, int *numFiles);

// Function prototype for extracting files from an archive
void extractFiles(const char *archiveFileName, const char *extractPath);

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

// Function for merging input files into an archive
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

// Function for checking the compatibility of a file (ASCII text file)
void checkFileCompatibility(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open input file %s for reading.\n", filename);
        exit(1);
    }

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch < 0 || ch > 127) {
            printf("Error: File %s is not a compatible ASCII text file.\n", filename);
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
}

// Function for calculating the total size of input files
size_t getTotalSize(int argc, char *argv[]) {
    size_t totalSize = 0;

    for (int i = 4; i < argc - 1; ++i) {
        FILE *file = fopen(argv[i], "rb");

        if (!file) {
            printf("Error: Could not open input file %s for reading.\n", argv[i]);
            exit(1);
        }

        fseek(file, 0, SEEK_END);
        totalSize += ftell(file);

        fclose(file);
    }

    return totalSize;
}

void writeOrganizationSection(FILE *outputFile, FileInfo *fileInfo, int numFiles) {
    //the numerical size of the organization section
    fprintf(outputFile, "%010d|", numFiles);

    for (int i = 4; i < argc - 1; ++i) {
        // Assuming that the file name is stored in the first MAX_FILENAME_LENGTH bytes of the buffer
        strncpy(fileInfo[i - 4].filename, argv[i], MAX_FILENAME_LENGTH);
        fileInfo[i - 4].filename[MAX_FILENAME_LENGTH - 1] = '\0';  // Ensures the null-termination

        // Geting file permissions
        struct stat st;
        stat(argv[i], &st);
        fileInfo[i - 4].permissions = st.st_mode;

        // Geting file size
        FILE *file = fopen(argv[i], "rb");
        fseek(file, 0, SEEK_END);
        fileInfo[i - 4].size = ftell(file);
        fclose(file);

        // Writing organization record
        fprintf(outputFile, "%s,%o,%ld|", fileInfo[i - 4].filename, fileInfo[i - 4].permissions, fileInfo[i - 4].size);
    }
}

void readOrganizationSection(FILE *archiveFile, FileInfo *fileInfo, int *numFiles) {
    char sizeStr[11];
    fread(sizeStr, 1, 10, archiveFile);
    sizeStr[10] = '\0';
    *numFiles = atoi(sizeStr);

    for (int i = 0; i < *numFiles; ++i) {
        // Reading organization record
        fscanf(archiveFile, "%49[^,],%o,%ld|", fileInfo[i].filename, &fileInfo[i].permissions, &fileInfo[i].size);
    }
}

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
        mkdir(extractPath, 0700);  // Createing directory with read, write, execute permissions for the owner
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
