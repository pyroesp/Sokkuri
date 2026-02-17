#ifndef _THREAD_H
#define _THREAD_H


#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #define _POSIX_C_SOURCE 200809L
    #include <pthread.h>
    #include <signal.h>
    #include <errno.h>
#endif

#include "file.h"
#include "sha-256.h"

// define max number of threads
#define MAX_THREADS 16

// platform-specific thread handle type
#ifdef _WIN32
    typedef HANDLE ThreadHandle;
#else
    typedef pthread_t ThreadHandle;
#endif

// thread structure to pass data to the thread
typedef struct{
    s_File *f;
    s_SHA256_Digest *d;
}ThreadData;

// thread object containing handle, data and flags
typedef struct{
    ThreadHandle *handle;
    ThreadData *data;
    int running;
    int finished;
}ThreadObject;

// thread function made as required by platform
#ifdef _WIN32
    void thread(void *p);
#else
    void* thread(void *p);
#endif

// return pointer to thread handle that is free
int thread_FindUnusedThread(ThreadObject *to);
// wait until all threads finished
int thread_WaitUntilFinished(ThreadObject *to);

#endif // _THREAD_H
