#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "file.h"
#include "sha-256.h"
#include "thread.h"


#define SIZE_PATH 512


int main(int argc, char* argv[]){
    int i, j, k;
    int size_of_file = 0;
    FILE *out, *sokkuri;
    s_File *f = NULL;
    clock_t start = clock(); // get value of clock at start of program

    // root directory needs to be set as program argument
    if (argc != 2)
        return -1;

    // create the output file that will contain the full file list
    out = fopen("out.txt", "w+");
    // create the duplicate file that will contain only duplicates found
    sokkuri = fopen("sokkuri.txt", "w+");
    if(!out || !sokkuri)
        return -1;

    printf("Get list of files:\n");
    f = file_GetList(NULL, &size_of_file, argv[1]); // Get list of files
    printf("\tFound %d files...\n", size_of_file);

    printf("Hashing files: ");
    s_SHA256_Digest *sha256; // create SHA256 digest
    sha256 = (s_SHA256_Digest*)malloc(sizeof(s_SHA256_Digest) * size_of_file);
    if (!sha256)
        return -1;
    memset(sha256, 0, sizeof(s_SHA256_Digest) * size_of_file);

    s_Thread *t; // create thread data struct
    t = (s_Thread*)malloc(sizeof(s_Thread) * size_of_file);
    if (!t)
        return -1;
    memset(t, 0, sizeof(s_Thread) * size_of_file);

    HANDLE *hThread; // create thread handle (windows thing)
    hThread = (HANDLE*)malloc(sizeof(HANDLE) * MAX_THREADS);
    if (!hThread)
        return -1;
    memset(hThread, 0, sizeof(HANDLE) * MAX_THREADS);

    // set the thread data
    for (i = 0; i < size_of_file; i++){
        t[i].f = &f[i];
        t[i].d = &sha256[i];
    }

    // use clock stuff to print dots during hashing so we know the program is still running
    clock_t timeout = 5 * CLOCKS_PER_SEC;
    clock_t ct = clock();
    int *hashing; // create a hashing list of size size_of_file to use as flag if the associated file index is being/has already been hashed
    hashing = (int*)malloc(sizeof(int) * size_of_file);
    memset(hashing, 0, sizeof(int) * size_of_file);
    for (i = 0; i < size_of_file; i++){ // go through all files
        for (j = i + 1; j < size_of_file; j++){ // go through all files starting at index i
            if (f[i].data_size == f[j].data_size){ // hash only if size is equal
                HANDLE *temp; // use temp handle to find free thread
                if (!hashing[i]){ // check if file at index i already is being/has been hashed
                    temp = NULL;
                    while (temp == NULL) // wait to find a free thread
                        temp = thread_FindUnusedThread(hThread);
                    *temp = (HANDLE)_beginthread(thread, 0, (void*)&t[i]); // start new thread
                    hashing[i] = 1; // set the hashing index to 1 so we can skip it next time
                }
                if (!hashing[j]){  // check if file at index j already is being/has been hashed
                    temp = NULL;
                    while (temp == NULL) // wait to find a free thread
                        temp = thread_FindUnusedThread(hThread);
                    *temp = (HANDLE)_beginthread(thread, 0, (void*)&t[j]); // start new thread
                    hashing[j] = 1; // set the hashing index to 1 so we can skip it next time
                }
            }
        }

        // print dot
        if (clock() - ct > timeout){
            printf(".");
            ct = clock();
        }
    }
    // wait for active threads to finish
    thread_WaitUntilFinished(hThread);
    printf("\n\tHashing done...\n");

    // Write file list to output text file, with their hash
    printf("Writing to output file: ");
    fprintf(out, "path, name, hash\n");
    for (i = 0; i < size_of_file; i++){
        if (!strcmp(f[i].ext, "jpg") || !strcmp(f[i].ext, "JPG") || !strcmp(f[i].ext, "png") || !strcmp(f[i].ext, "PNG") || !strcmp(f[i].ext, "jpeg") || !strcmp(f[i].ext, "JPEG")){
            fprintf(out, "%s, %s, ", f[i].path, f[i].name);
            for (j = 0; j < SHA_HASH_SIZE; j++){
                if (sha256[i].digest)
                    fprintf(out, "%08X", sha256[i].digest[j]);
                else
                    fprintf(out, "00000000");
            }
            fprintf(out, "\n");
        }

        // print dot
        if (clock() - ct > timeout){
            printf(".");
            ct = clock();
        }
    }
    fclose(out); // close file list output file
    printf("\n\tOutput file written...\n");

    // Writing duplicates to sokkuri file
    printf("Writing duplicates to file: ");
    int *found; // create a list of size size_of_file to use as flag if the associated file index has already been found
    found = (int*)malloc(sizeof(int) * size_of_file);
    memset(found, 0, sizeof(int) * size_of_file);
    fprintf(sokkuri, "path, name, hash\n");
    for (i = 0; i < size_of_file; i++){ // loop through all files
        for (j = i + 1; j < size_of_file; j++){ // loop through all files starting from i + 1
            if (sha256[i].digest && sha256[j].digest && !found[j]){ // if digest files exist and file at index j has not been found yet
                if (!memcmp(sha256[i].digest, sha256[j].digest, sizeof(uint32_t) * SHA_HASH_SIZE)){ // compare hashes between index i and j
                    if (!found[i]){ // if index i has not been found yet
                        fprintf(sokkuri, "%s, %s, ", f[i].path, f[i].name); // write path, name and hash of file at index i to the sokkuri txt file
                        for (k = 0; k < SHA_HASH_SIZE; k++)
                            fprintf(sokkuri, "%08X", sha256[i].digest[k]);
                        fprintf(sokkuri, "\n");
                        found[i] = 1; // set the flag to 1 so we can skip it next time
                    }

                    // write path, name and hash of file at index j to the sokkuri txt file
                    fprintf(sokkuri, "%s, %s, ", f[j].path, f[j].name);
                    for (k = 0; k < SHA_HASH_SIZE; k++)
                        fprintf(sokkuri, "%08X", sha256[j].digest[k]);
                    fprintf(sokkuri, "\n");
                    found[j] = 1; // set the flag to 1 so we can skip it next time
                }
            }
        }

        // print dot
        if (clock() - ct > timeout){
            printf(".");
            ct = clock();
        }
    }
    fclose(sokkuri); // close file
    printf("\n\tDuplicate file written...\n");

    // get size of processed files to display as end message
    float s = 0;
    char u = 0;
    for (i = 0; i < size_of_file; i++)
        s += f[i].data_size;

    if (s > 1e9){
        u = 'G';
        s = s / 1e9;
    }
    else if (s > 1e6){
        u = 'M';
        s = s / 1e6;
    }else if (s > 1e3){
        u = 'k';
        s = s / 1e3;
    }
    // print statistics: number of files processed, total size, total time to process
    printf("Finished hashing %d files of total size %0.2f%cB in %0.2fs.\n", size_of_file, s, u, (float)(clock() - start)/CLOCKS_PER_SEC);

    // start cleanup of allocated memory
    printf("Cleaning up:\n");
    for (i = 0; i < size_of_file; i++){
        free(sha256[i].digest);
    }
    free(sha256);
    free(hashing);
    free(t);
    free(hThread);
    file_Free(f, size_of_file);
    free(f);
    printf("\tCleanup done...\n");

    // exit success
    return 0;
}
