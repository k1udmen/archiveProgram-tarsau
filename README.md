# archiveProgram-tarsau

program called "tarsau" that works like tar, rar or zip, but does not compress
This project implements a simple archiving program called "tarsau" in C. The program can merge text files into a single text file and extract files from an archive.

## Usage

- To merge files into an archive:

- To extract files from an archive:
  
## File Structure

-SRC
	- `tarsau.c`: Main source code file.
	- `extract.c`: Source code for the extraction functionality.
	- `extract.h`: Header file for the extraction functionality.
	- `merge.c`: Source code for the merging functionality.
	- `merge.h`: Header file for the merging functionality.
	- `common.h`: Header file with common definitions.

- `Makefile`: Makefile for convenient compilation.

- '.gitignore': To ensure that certain files not tracked by Git remain untracked

-OBJ
	This is where the intermediate object files (.o files) generated during compilation are placed. 

-BIN
	This is where the final executable (binary) files are placed after the linking process.

  
## Building and Compilation

To compile the project, follow these steps:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/k1udmen/archiveProgram-tarsau.git
   cd archiveProgram-tarsau
   make
   ./tarsau -b outputfile file1 file2 ...
   ./tarsau -a archivefile extractpath
    
This section provides step-by-step instructions on how to clone the repository, compile the project using the provided Makefile, run the program, and clean up compiled files if needed. Adjust the content based on your actual project structure and requirements.

## Contact or Support

salih.doganer@ogr.sakarya.edu.tr
