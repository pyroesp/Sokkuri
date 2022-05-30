#ifndef _FILE_H
#define _FILE_H

#include <stdint.h>

// file list structure
typedef struct{
    char *path; // file path
    char *name; // file name
    char *ext; // file extension

    uint8_t *data; // file data
    uint32_t data_size; // file data size
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

// Read file to data buffer
void file_Open(s_File *f);
// Free file data
void file_Close(s_File *f);

// Free file path, name and extension
void file_Free(s_File *f, int size_of_files);


#endif // _FILE_H
