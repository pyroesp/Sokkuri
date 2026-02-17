#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"

#ifdef _WIN32
void thread(void *p){
#else
void* thread(void *p){
#endif
    ThreadObject *to;
    ThreadData *data;
    s_File *file;
    s_SHA256_Digest *sha256;

    // cast void pointer to thread object
    to = (ThreadObject*)p;
    data = to->data;
    file = data->f;
    sha256 = data->d;

    file_Open(file); // open file
    while (file->eof == 0){
        file_Read(file); // read file to data array
        if (file->data){ // if data available
            if (file->eof) // only terminate data when eof found
                file->data = sha256_PrepareData(file->data, &file->data_size, file->file_size); // prepare data buffer to be a multiple of 512 bits
            sha256->digest = sha256_Transform(file->data, file->data_size, sha256->digest); // execute SHA256 hash on the data
        }
    }
    file_Close(file); // close file

    // exit and end thread
    to->finished = 1;
#ifdef _WIN32
    _endthread();
#else
    pthread_exit(NULL);
#endif
}


int thread_FindUnusedThread(ThreadObject *to){
    int index = 0;
    int waiting = 1;

    // wait for object that is not running or is finished
    do {
        int running = to[index].running;
        int finished = to[index].finished;

        if (finished || !running){
            waiting = 0;
        }else if (running){
            index++;
        }

        if (index >= MAX_THREADS)
            index = 0;
    }while(waiting == 1);
    return index;
}

int thread_WaitUntilFinished(ThreadObject *to){
    int i, threads_done = 0;

    for (i = 0; i < MAX_THREADS; i++){
        if (to[i].finished)
            threads_done++;
    }

    return threads_done == MAX_THREADS;
}
