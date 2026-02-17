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
    s_File *files = NULL;
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
    files = file_GetList(NULL, &size_of_file, argv[1]); // Get list of files
    printf("\tFound %d files...\n", size_of_file);

    s_SHA256_Digest *sha256; // create SHA256 digest
    sha256 = (s_SHA256_Digest*)malloc(sizeof(s_SHA256_Digest) * size_of_file);
    if (!sha256)
        return -1;
    // init struct to 0
    memset(sha256, 0, sizeof(s_SHA256_Digest) * size_of_file);

    ThreadData *data; // create thread data struct
    data = (ThreadData*)malloc(sizeof(ThreadData) * size_of_file);
    if (!data)
        return -1;
    memset(data, 0, sizeof(ThreadData) * size_of_file);

    // set thread data
    for (i = 0; i < size_of_file; i++){
        data[i].f = &files[i];
        data[i].d = &sha256[i];
    }

    ThreadHandle *handle; // create thread handle
    handle = (ThreadHandle*)malloc(sizeof(ThreadHandle) * MAX_THREADS);
    if (!handle)
        return -1;
    memset(handle, 0, sizeof(ThreadHandle) * MAX_THREADS);

    ThreadObject *object; // create thread object
    object = (ThreadObject*)malloc(sizeof(ThreadObject) * MAX_THREADS);
    if (!object)
        return -1;
    memset(object, 0, sizeof(ThreadObject) * MAX_THREADS);

    // set object struct
    for (i = 0; i < MAX_THREADS; i++){
        object[i].handle = &handle[i];
        object[i].data = NULL;
        object[i].running = 0;
        object[i].finished = 0;
    }


    // use clock stuff to print dots during hashing so we know the program is still running
    clock_t timeout = 5 * CLOCKS_PER_SEC;
    clock_t ct = clock();

    int *hashing; // create a hashing list of size size_of_file to use as flag if the associated file index is being/has already been hashed
    hashing = (int*)malloc(sizeof(int) * size_of_file);
    memset(hashing, 0, sizeof(int) * size_of_file);

    printf("\tStart hashing:\n");
    for (i = 0; i < size_of_file - 1; i++){ // go through all files
        for (j = i + 1; j < size_of_file; j++){ // go through all files starting at index i

            // print dot
            if (clock() - ct > timeout){
                printf(".");
                ct = clock();
            }

            if (files[i].file_size == files[j].file_size){ // hash only if size is equal
                if (!hashing[i]){ // check if file at index i already is being/has been hashed
                    int index = thread_FindUnusedThread(object);
                    object[index].data = &data[i];
                    object[index].running = 1;
                    object[index].finished = 0;

                    // Start thread
                #ifdef _WIN32
                    *(object[index].handle) = (ThreadHandle)_beginthread(thread, 0, (void*)&object[index]);
                #else
                    pthread_create(object[index].handle, NULL, thread, (void*)&object[index]);
                    //pthread_detach(*(object[index].handle));
                #endif
                    hashing[i] = 1; // set the hashing index to 1 so we can skip it next time
                }

                if (!hashing[j]){ // check if file at index i already is being/has been hashed
                    int index = thread_FindUnusedThread(object);
                    object[index].data = &data[j];
                    object[index].running = 1;
                    object[index].finished = 0;

                    // Start thread
                #ifdef _WIN32
                    *(object[index].handle) = (ThreadHandle)_beginthread(thread, 0, (void*)&object[index]);
                #else
                    pthread_create(object[index].handle, NULL, thread, (void*)&object[index]);
                    //pthread_detach(*(object[index].handle));
                #endif
                    hashing[j] = 1; // set the hashing index to 1 so we can skip it next time
                }
            }
        }
    }
    // wait for active threads to finish
    while (thread_WaitUntilFinished(object) == 0){
        if (clock() - ct > timeout){
            printf(".");
            ct = clock();
        }
    }
    printf("\tHashing done...\n");

    // Write file list to output text file, with their hash
    printf("Writing to output file: ");
    fprintf(out, "path, name, hash\n");
    for (i = 0; i < size_of_file; i++){
        fprintf(out, "%s, %s, ", files[i].path, files[i].name);
        for (j = 0; j < SHA_HASH_SIZE; j++){
            if (sha256[i].digest)
                fprintf(out, "%08X", sha256[i].digest[j]);
            else
                fprintf(out, "00000000");
        }
        fprintf(out, "\n");

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
                        fprintf(sokkuri, "%s, %s, ", files[i].path, files[i].name); // write path, name and hash of file at index i to the sokkuri txt file
                        for (k = 0; k < SHA_HASH_SIZE; k++)
                            fprintf(sokkuri, "%08X", sha256[i].digest[k]);
                        fprintf(sokkuri, "\n");
                        found[i] = 1; // set the flag to 1 so we can skip it next time
                    }

                    // write path, name and hash of file at index j to the sokkuri txt file
                    fprintf(sokkuri, "%s, %s, ", files[j].path, files[j].name);
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
        s += files[i].file_size;

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
    free(data);
    free(handle);
    file_Free(files, size_of_file);
    free(files);
    free(object);
    printf("\tCleanup done...\n");

    // exit success
    return 0;
}
