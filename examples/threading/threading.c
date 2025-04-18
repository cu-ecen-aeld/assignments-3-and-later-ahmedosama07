#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    int rc = usleep(thread_func_args->wait_to_obtain_ms * 1000);
    if (rc != 0) {
        ERROR_LOG("usleep failed: %d", rc);
        thread_func_args->thread_complete_success = false;
        pthread_exit(thread_param); // Ensure proper cleanup
    }

    rc = pthread_mutex_lock(thread_func_args->mutex);
    if (rc != 0) {
        ERROR_LOG("pthread_mutex_lock failed: %d", rc);
        thread_func_args->thread_complete_success = false;
        pthread_exit(thread_param); // Ensure proper cleanup
    }

    rc = usleep(thread_func_args->wait_to_release_ms * 1000);
    if (rc != 0) {
        ERROR_LOG("usleep failed: %d", rc);
        pthread_mutex_unlock(thread_func_args->mutex); // Unlock mutex before exiting
        thread_func_args->thread_complete_success = false;
        pthread_exit(thread_param); // Ensure proper cleanup
    }

    rc = pthread_mutex_unlock(thread_func_args->mutex);
    if (rc != 0) {
        ERROR_LOG("pthread_mutex_unlock failed: %d", rc);
        thread_func_args->thread_complete_success = false;
        pthread_exit(thread_param); // Ensure proper cleanup
    }

    thread_func_args->thread_complete_success = true;
    pthread_exit(thread_param); // Ensure proper cleanup
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    struct thread_data* thread_func_args = malloc(sizeof(struct thread_data));
    if (thread_func_args == NULL) {
        ERROR_LOG("Failed to allocate memory for thread data");
        return false;
    }

    thread_func_args->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_func_args->wait_to_release_ms = wait_to_release_ms;
    thread_func_args->mutex = mutex;
    thread_func_args->thread_complete_success = false;

    int rc = pthread_create(thread, NULL, threadfunc, (void*)thread_func_args);
    if (rc != 0) {
        ERROR_LOG("pthread_create failed: %d", rc);
        free(thread_func_args); // Free allocated memory on failure
        return false;
    }

    return true;
}

