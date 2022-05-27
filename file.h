#ifndef _FILE_H
#define _FILE_H

#include <stdint.h>

typedef struct{
    char *path;
    char *name;
    char *ext;

    uint8_t *data;
    uint32_t data_size;
}s_File;


s_File* file_Add(s_File *f, int *size_of_files, char *name, char *path);

void file_Init(s_File *f);
void file_SetName(s_File *f, char *name);
void file_SetPath(s_File *f, char *path);

s_File* file_GetList(s_File *f, int *size_of_files, char *dir);
void file_PrintList(s_File *f, int size);


void file_Open(s_File *f);
void file_Close(s_File *f);

void file_Free(s_File *f, int size_of_files);


#endif // _FILE_H
