#ifndef _THREAD_H
#define _THREAD_H

#include <windows.h>
#include <process.h>

#include "file.h"
#include "sha-256.h"


#define MAX_THREADS 16


typedef struct{
    s_File *f;
    s_SHA256_Digest *d;
}s_Thread;


void thread(void *p);


#endif // _THREAD_H
