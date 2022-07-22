#include <stdio.h>
#include <stdlib.h>

#include "thread.h"


void thread(void *p){
    s_Thread *t;
    s_File *f;
    s_SHA256_Digest *d;
    // transform void *p pointer to the thread structure
    t = (s_Thread*)p;
    f = t->f;
    d = t->d;

    file_Open(f); // open file
    while (f->eof == 0){
        file_Read(f); // read file to data array
        if (f->data){ // if data available
            if (f->eof) // only terminate data when eof found
                f->data = sha256_PrepareData(f->data, &f->data_size, f->file_size); // prepare data buffer to be a multiple of 512 bits
            d->digest = sha256_Transform(f->data, f->data_size, d->digest); // execute SHA256 hash on the data
        }
    }
    file_Close(f); // close file

    // exit and end thread
    _endthread();
}



HANDLE* thread_FindUnusedThread(HANDLE *t){
    int i;
    for (i = 0; i < MAX_THREADS; i++){ // loop through all threads
        switch (WaitForSingleObject(t[i], 0)){ // check if thread is busy
            case WAIT_TIMEOUT: // if busy do nothing
                break;
            case WAIT_OBJECT_0: // if free or finished
            default:
                return &t[i];
                break;
        }
    }

    return NULL;
}


int thread_WaitUntilFinished(HANDLE *t){
    int i, threads_done = 0;
    for (i = 0; i < MAX_THREADS; i++){
        switch (WaitForSingleObject(t[i], 0)){
            case WAIT_TIMEOUT:
                threads_done = 0;
                break;
            case WAIT_OBJECT_0:
            default:
                t[i] = (HANDLE)0;
                threads_done++;
                break;
        }
    }

    return threads_done == MAX_THREADS;
}
