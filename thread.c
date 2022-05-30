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
        f->data = sha256_PrepareData(f->data, &f->data_size); // prepare data buffer to be a multiple of 512 bits
        d->digest = sha256_Transform(f->data, f->data_size); // execute SHA256 hash on the data
        file_Close(f); // free data buffer
    }

    // exit and end thread
    _endthread();
}
