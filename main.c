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
    FILE *out, *sokkuri;
    s_File *f = NULL;
    clock_t t;

    root_dir = (char*)malloc(sizeof(char) * SIZE_PATH);
    memset(root_dir, 0, sizeof(char) * SIZE_PATH);
    strncpy(root_dir, "Z:\\Pictures", SIZE_PATH);

    out = fopen("out.txt", "w+");
    sokkuri = fopen("sokkuri.txt", "w+");
    if(!out || !sokkuri)
        return -1;

    printf("Get list of files:\n");
    f = file_GetList(NULL, &size_of_file, root_dir);
    printf("\tFound %d files...\n", size_of_file);

    /*
    printf("Print List:\n");
    file_PrintList(f, size_of_file);
    printf("\tPrint List done...\n");
    */

    printf("\n");
    printf("\n");

    s_SHA256_Digest *sha256;
    sha256 = (s_SHA256_Digest*)malloc(sizeof(s_SHA256_Digest) * size_of_file);
    memset(sha256, 0, sizeof(s_SHA256_Digest) * size_of_file);

    int i, j, k;
    clock_t timeout = 5 * CLOCKS_PER_SEC;
    t = clock();
    printf("Hashing files: ");
    for (i = 0; i < size_of_file; i++){
        if (!strcmp(f[i].ext, "jpg") || !strcmp(f[i].ext, "JPG") || !strcmp(f[i].ext, "png") || !strcmp(f[i].ext, "PNG") || !strcmp(f[i].ext, "jpeg") || !strcmp(f[i].ext, "JPEG")){
            file_Open(&f[i]);
            f[i].data = sha256_PrepareData(f[i].data, &f[i].data_size);
            sha256[i].digest = sha256_Transform(f[i].data, f[i].data_size);
            file_Close(&f[i]);

            // print dot
            if (clock() - t > timeout){
                printf(".");
                t = clock();
            }
        }
    }
    printf("\n\tHashing done...\n");
    printf("Writing to output file: ");
    fprintf(out, "path, name, hash\n");
    for (i = 0; i < size_of_file; i++){
        if (!strcmp(f[i].ext, "jpg") || !strcmp(f[i].ext, "JPG") || !strcmp(f[i].ext, "png") || !strcmp(f[i].ext, "PNG") || !strcmp(f[i].ext, "jpeg") || !strcmp(f[i].ext, "JPEG")){
            fprintf(out, "%s, %s, ", f[i].path, f[i].name);
            for (j = 0; j < SHA_HASH_SIZE; j++)
                fprintf(out, "%08X", sha256[i].digest[j]);
            fprintf(out, "\n");
        }

        // print dot
        if (clock() - t > timeout){
            printf(".");
            t = clock();
        }
    }
    printf("\n\tOutput file written...\n");

    int *found;
    found = (int*)malloc(sizeof(int) * size_of_file);
    memset(found, 0, sizeof(int) * size_of_file);
    printf("Writing duplicates to file: ");
    fprintf(sokkuri, "path, name, hash\n");
    for (i = 0; i < size_of_file; i++){
        for (j = i + 1; j < size_of_file; j++){
            if (sha256[i].digest && sha256[j].digest && !found[j]){
                if (!memcmp(sha256[i].digest, sha256[j].digest, sizeof(uint32_t) * SHA_HASH_SIZE)){
                    if (!found[i]){
                        fprintf(sokkuri, "%s, %s, ", f[i].path, f[i].name);
                        for (k = 0; k < SHA_HASH_SIZE; k++)
                            fprintf(sokkuri, "%08X", sha256[i].digest[k]);
                        fprintf(sokkuri, "\n");
                        found[i] = 1;
                    }

                    fprintf(sokkuri, "%s, %s, ", f[j].path, f[j].name);
                    for (k = 0; k < SHA_HASH_SIZE; k++)
                        fprintf(sokkuri, "%08X", sha256[j].digest[k]);
                    fprintf(sokkuri, "\n");
                    found[j] = 1;
                }
            }
        }

        // print dot
        if (clock() - t > timeout){
            printf(".");
            t = clock();
        }
    }
    printf("\n\tDuplicate file written...\n");

    float s = 0;
    char u = 0;
    for (i = 0; i < size_of_file; i++)
        s += f[i].data_size;

    if (s > 1000000000.0){
        u = 'G';
        s = s / 1000000000.0;
    }
    else if (s > 1000000.0){
        u = 'M';
        s = s / 1000000.0;
    }else if (s > 1000.0){
        u = 'k';
        s = s / 1000.0;
    }
    printf("Finished hashing %d files of total size %0.2f%cB\n", size_of_file, s, u);

    printf("Cleaning up:\n");
    for (i = 0; i < size_of_file; i++){
        free(sha256[i].digest);
    }
    free(sha256);
    free(root_dir);
    file_Free(f, size_of_file);
    fclose(out);
    fclose(sokkuri);
    printf("\tCleanup done...\n");

    return 0;
}
