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

    // TODO: Allow all files to be processed.
    //       To do this limit the data buffer so you don't try to load a 20GB movie to RAM,
    //       and do multiple hash passes to get the final SHA256 digest.

    // for now, only allow jpg/jpeg/png files to be hashed
    if (!strcmp(f->ext, "jpg") || !strcmp(f->ext, "JPG") || !strcmp(f->ext, "png") || !strcmp(f->ext, "PNG") || !strcmp(f->ext, "jpeg") || !strcmp(f->ext, "JPEG")){
        file_Open(f); // read file to data buffer
        if (f->data){
            f->data = sha256_PrepareData(f->data, &f->data_size); // prepare data buffer to be a multiple of 512 bits
            d->digest = sha256_Transform(f->data, f->data_size); // execute SHA256 hash on the data
            file_Close(f); // free data buffer
        }
    }

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


void thread_WaitUntilFinished(HANDLE *t){
    int i, threads_done = 0;
    while (threads_done < MAX_THREADS){
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
    }
}
