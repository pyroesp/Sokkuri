#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>


void file_Init(s_File *f){
    memset(f, 0, sizeof(s_File));
}

void file_SetName(s_File *f, char *name){
    if (f && name){
        f->name = (char*)malloc(sizeof(char) * strlen(name));
        strcpy(f->name, name);
        char *ext = strrchr(name, '.') + 1;
        f->ext = (char*)malloc(sizeof(char) * strlen(ext));
        strcpy(f->ext, ext);
    }
}


void file_SetPath(s_File *f, char *path){
    if (f && path){
        f->path = (char*)malloc(sizeof(char) * strlen(path));
        strcpy(f->path, path);
    }
}

s_File* file_Add(s_File *f, int *size_of_files, char *name, char *path){
    (*size_of_files)++; // increase array size
    if (f == NULL){ // if null / doesn't exist
        f = (s_File*)malloc(sizeof(s_File)); // create new file with malloc
    }else{ // if file array already exists
        f = (s_File*)realloc(f, (*size_of_files) * sizeof(s_File)); // reallocate
    }

    file_Init(&f[(*size_of_files) - 1]); // memset new s_File to 0
    file_SetName(&f[(*size_of_files) - 1], name); // copy name to new file
    file_SetPath(&f[(*size_of_files) - 1], path); // copy path to new file

    return f ? f : NULL;
}


s_File* file_GetList(s_File *f, int *size_of_files, char *dir){
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    char path[2048];

    sprintf(path, "%s\\*.*", dir);
    hFind = FindFirstFile(path, &fdFile);
    if(hFind == INVALID_HANDLE_VALUE)
        return NULL;

    do{
        // first 'file' found is always "." and ".."
        if (strcmp(".", fdFile.cFileName) != 0 && strcmp("..", fdFile.cFileName) != 0){
            if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){ // if folder
                sprintf(path, "%s\\%s", dir, fdFile.cFileName);
                f = file_GetList(f, size_of_files, path);
            }else{ // if file
                printf("%s / %s\n", dir, fdFile.cFileName);
                f = file_Add(f, size_of_files, fdFile.cFileName, dir);
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); // find next file
    FindClose(hFind); // close handle
    return f;
}

void file_PrintList(s_File *f, int size){
    if (f){
        int i;
        for (i = 0; i < size; i++)
            printf("\t%s / %s - %s\n", f[i].path, f[i].name, f[i].ext);
    }
}



void file_Open(s_File *f){
    FILE *fp;
    uint32_t size;
    char file_path[512];

    strcpy(file_path, f->path);
    strcat(file_path, "\\");
    strcat(file_path, f->name);

    fp = fopen(file_path, "rb");
    if (!fp)
        return;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    f->data = (uint8_t*)malloc(sizeof(uint8_t) * size);
    if (!f->data)
        return;
    f->data_size = size;
    rewind(fp);
    fread(f->data, sizeof(uint8_t), size, fp);
    fclose(fp);
}


void file_Close(s_File *f){
    if (f)
        free(f->data);
}


void file_Free(s_File *f, int size_of_files){
    int i;
    for (i = 0; i < size_of_files; i++){
        printf("Freeing file[%d] - %s\n", i, f[i].name);
        if (f[i].path)
            free(f[i].path);
        if (f[i].name)
            free(f[i].name);
        if (f[i].ext)
            free(f[i].ext);
    }
    free(f);
}
