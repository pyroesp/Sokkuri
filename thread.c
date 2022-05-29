#include <stdio.h>
#include <stdlib.h>

#include "thread.h"



void thread(void *p){
    s_Thread *t;
    s_File *f;
    s_SHA256_Digest *d;
    t = (s_Thread*)p;
    f = t->f;
    d = t->d;

    if (!strcmp(f->ext, "jpg") || !strcmp(f->ext, "JPG") || !strcmp(f->ext, "png") || !strcmp(f->ext, "PNG") || !strcmp(f->ext, "jpeg") || !strcmp(f->ext, "JPEG")){
        file_Open(f);
        f->data = sha256_PrepareData(f->data, &f->data_size);
        d->digest = sha256_Transform(f->data, f->data_size);
        file_Close(f);
    }

    _endthread();
}
