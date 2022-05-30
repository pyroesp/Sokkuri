#ifndef _THREAD_H
#define _THREAD_H

#include <windows.h>
#include <process.h>

#include "file.h"
#include "sha-256.h"

// define max number of threads
#define MAX_THREADS 16

// thread structure to pass data to the thread
typedef struct{
    s_File *f;
    s_SHA256_Digest *d;
}s_Thread;

// thread function made as required by _beginthread
void thread(void *p);


#endif // _THREAD_H
