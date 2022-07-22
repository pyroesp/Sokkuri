#ifndef _FILE_H
#define _FILE_H

#include <stdint.h>
#include <stdio.h>

#define KIBIBYTE 1024
#define MEBIBYTE 1048576
#define FILE_MAX_DATA_SIZE (128 * MEBIBYTE)

// file list structure
typedef struct{
    char *path; // file path
    char *name; // file name
    char *ext; // file extension
    uint64_t file_size; // total file size

    FILE *fp; // file pointer
    uint64_t cursor; // current file cursor position

    uint8_t split; // split file flag
    uint8_t eof; // eof file flag

    uint8_t *data; // file data
    uint32_t data_size; // data size -> max FILE_MAX_DATA_SIZE
}s_File;

// Create or add a file to the file list
s_File* file_Add(s_File *f, int *size_of_files, char *name, char *path);

// Initialize new file (memset to 0)
void file_Init(s_File *f);
// Set the file name
void file_SetName(s_File *f, char *name);
// Set the file path
void file_SetPath(s_File *f, char *path);

// Get list of files from <dir> folder and subfolders
s_File* file_GetList(s_File *f, int *size_of_files, char *dir);
// Printf the file list
void file_PrintList(s_File *f, int size);

// Open file
void file_Open(s_File *f);
// Read file to data buffer
void file_Read(s_File *f);
// Close file
void file_Close(s_File *f);

// Free file path, name and extension
void file_Free(s_File *f, int size_of_files);


#endif // _FILE_H
