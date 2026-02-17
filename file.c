#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void file_Init(s_File *f){
    memset(f, 0, sizeof(s_File));
}

void file_SetName(s_File *f, char *name){
    if (f && name){
        f->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(f->name, name);
        char *dot = strrchr(name, '.');
        if (dot) {
            f->ext = (char*)malloc(sizeof(char) * (strlen(dot + 1) + 1));
            strcpy(f->ext, dot + 1);
        } else {
            f->ext = (char*)malloc(sizeof(char) * 1);
            f->ext[0] = '\0';
        }
    }
}

void file_SetPath(s_File *f, char *path){
    if (f && path){
        f->path = (char*)malloc(sizeof(char) * (strlen(path) + 1));
        strcpy(f->path, path);
    }
}

s_File* file_Add(s_File *f, int *size_of_files, char *name, char *path){
    (*size_of_files)++; // increase array size
    if (f == NULL){ // if null / doesn't exist
        f = (s_File*)malloc(sizeof(s_File)); // create new file with malloc
    }else{ // if file array already exists
        f = (s_File*)realloc(f, (*size_of_files) * sizeof(s_File)); // realloc
    }

    file_Init(&f[(*size_of_files) - 1]); // memset new file to 0
    file_SetName(&f[(*size_of_files) - 1], name); // copy name to new file
    file_SetPath(&f[(*size_of_files) - 1], path); // copy path to new file

    return f;
}

#ifdef _WIN32
s_File* file_GetList(s_File *f, int *size_of_files, char *dir){
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    char path[2048];

    sprintf(path, "%s\\*.*", dir);
    hFind = FindFirstFile(path, &fdFile);
    if(hFind == INVALID_HANDLE_VALUE)
        return f;

    do{
        // first 'file' found is always "." and ".."
        if (strcmp(".", fdFile.cFileName) != 0 && strcmp("..", fdFile.cFileName) != 0){
            if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){ // if folder
                sprintf(path, "%s\\%s", dir, fdFile.cFileName);
                f = file_GetList(f, size_of_files, path); // recursive through subfolders
            }else{ // if file
                f = file_Add(f, size_of_files, fdFile.cFileName, dir);
                f[*size_of_files - 1].file_size = ((uint64_t)fdFile.nFileSizeHigh << 32) + (uint64_t)fdFile.nFileSizeLow;

                // check if file size is bigger than max data size
                if (f[*size_of_files - 1].file_size > FILE_MAX_DATA_SIZE){
                    f[*size_of_files - 1].data_size = FILE_MAX_DATA_SIZE; // set data_size to max data size
                    f[*size_of_files - 1].split = 1; // set the split flag
                // else if smaller than max data size
                }else{
                    f[*size_of_files - 1].data_size = f[*size_of_files - 1].file_size; // set data_size to file size
                }
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); // find next file
    FindClose(hFind); // close handle
    return f;
}
#else
s_File* file_GetList(s_File *f, int *size_of_files, char *dir){
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    char path[2048];
    
    if ((dp = opendir(dir)) == NULL) {
        return f;
    }
    
    while ((entry = readdir(dp)) != NULL) {
        // Skip "." and ".."
        if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            continue;
            
        sprintf(path, "%s/%s", dir, entry->d_name);
        
        if (stat(path, &statbuf) == -1) {
            continue;
        }
        
        if (S_ISDIR(statbuf.st_mode)) { // if directory
            f = file_GetList(f, size_of_files, path); // recursive through subfolders
        } else { // if regular file
            f = file_Add(f, size_of_files, entry->d_name, dir);
            f[*size_of_files - 1].file_size = (uint64_t)statbuf.st_size;
            
            // check if file size is bigger than max data size
            if (f[*size_of_files - 1].file_size > FILE_MAX_DATA_SIZE){
                f[*size_of_files - 1].data_size = FILE_MAX_DATA_SIZE;
                f[*size_of_files - 1].split = 1;
            } else {
                f[*size_of_files - 1].data_size = f[*size_of_files - 1].file_size;
            }
        }
    }
    closedir(dp);
    return f;
}
#endif

void file_PrintList(s_File *f, int size){
    if (f){
        int i;
        for (i = 0; i < size; i++)
            printf("\t%d) %s / %s - %s\n", i, f[i].path, f[i].name, f[i].ext);
    }
}

void file_Open(s_File *f){
    char file_path[512];
    strcpy(file_path, f->path);
    
    #ifdef _WIN32
    strcat(file_path, "\\");
    #else
    strcat(file_path, "/");
    #endif
    
    strcat(file_path, f->name);

    // open file as read only binary
    f->fp = fopen(file_path, "rb");
}

void file_Read(s_File *f){
    if (f->fp){
        // if data available then reset it all
        if (f->data){
            free(f->data); // free file data
            f->data = NULL; // reset pointer
            f->data_size = 0; // set size to 0
        }

        // if it's a split file (ie bigger than max data size)
        if (f->split){
            // check if current position + max data size <= total file size
            if ((f->cursor + FILE_MAX_DATA_SIZE) <= f->file_size)
                f->data_size = FILE_MAX_DATA_SIZE;
            else
                f->data_size = f->file_size - f->cursor;
        }

        // if data pointer is null
        if (!f->data){
            f->data = (uint8_t*)malloc(sizeof(uint8_t) * f->data_size); // malloc file data
            if (!f->data)
                return;
        }
        // read data
        size_t count = fread(f->data, sizeof(uint8_t), f->data_size, f->fp); // read data
        f->cursor = count; // ftell(f->fp); // get cursor position after read
        if (f->cursor == f->file_size)
            f->eof = 1;
    }
}

void file_Close(s_File *f){
    if (f->fp) {
        if (f->data){
            free(f->data); // free file data
            f->data = NULL; // reset pointer
            f->data_size = 0; // set size to 0
        }
        fclose(f->fp); // close file
        f->fp = NULL;
    }
}

void file_Free(s_File *f, int size_of_files){
    int i;
    for (i = 0; i < size_of_files; i++){
        if (f[i].path)
            free(f[i].path);
        if (f[i].name)
            free(f[i].name);
        if (f[i].ext)
            free(f[i].ext);
        if (f[i].data)
            free(f[i].data);
    }
}
