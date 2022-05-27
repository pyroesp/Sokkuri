#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "file.h"
#include "sha-256.h"

#define SIZE_PATH 512


int main(int argc, char* argv[]){
    int size_of_file = 0;
    char *root_dir = NULL;
    s_File *f = NULL;
    clock_t t;

    root_dir = (char*)malloc(sizeof(char) * SIZE_PATH);
    memset(root_dir, 0, sizeof(char) * SIZE_PATH);
    strncpy(root_dir, "Z:\\Pictures", SIZE_PATH);


    printf("Get List:\n");
    f = file_GetList(NULL, &size_of_file, root_dir);
    printf("\tGet List done...\n");

    printf("Print List:\n");
    file_PrintList(f, size_of_file);
    printf("\tPrint List done...\n");

    printf("\n");
    printf("\n");

    s_SHA256_Digest *sha256;
    sha256 = (s_SHA256_Digest*)malloc(sizeof(s_SHA256_Digest) * size_of_file);
    memset(sha256, 0, sizeof(s_SHA256_Digest) * size_of_file);

    int i, j;
    clock_t time = 0;
    for (i = 0; i < size_of_file; i++){
        if (!strcmp(f[i].ext, "jpg") || !strcmp(f[i].ext, "JPG") || !strcmp(f[i].ext, "png") || !strcmp(f[i].ext, "PNG") || !strcmp(f[i].ext, "jpeg") || !strcmp(f[i].ext, "JPEG")){
            t = clock();
            file_Open(&f[i]);
            f[i].data = sha256_PrepareData(f[i].data, &f[i].data_size);
            sha256[i].digest = sha256_Transform(f[i].data, f[i].data_size);
            time += clock() - t;
            printf("Hash of file \"%s\" - %fs :\n\t", f[i].name, (float)(clock() - t)/(float)CLOCKS_PER_SEC);
            for (j = 0; j < SHA_HASH_SIZE; j++){
                if (j != 0 && (j % 8) == 0)
                    printf("\t\n");
                printf("0x%08X ", sha256[i].digest[j]);
            }
            printf("\n");
            file_Close(&f[i]);
        }
    }

    uint32_t s = 0;
    for (i = 0; i < size_of_file; i++)
        s += f[i].data_size;
    printf("\nFinished hashing %d files of total size %dB in %fs\n\n", size_of_file, s, (float)time/(float)CLOCKS_PER_SEC);

    printf("Cleaning up:\n");
    for (i = 0; i < size_of_file; i++){
        free(sha256[i].digest);
    }
    free(sha256);
    free(root_dir);
    file_Free(f, size_of_file);
    printf("\tCleanup done...\n");

    return 0;
}
