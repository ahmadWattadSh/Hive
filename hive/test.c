#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

void *thread_function(void *arg) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    // Do something in the thread
    printf("Thread is running\n");
    // Simulate some work
    sleep(5);

    return NULL;
}

int main() {
    pthread_t thread;
    int err;


    // Block SIGINT in the main thread


    // Create a thread
    err = pthread_create(&thread, NULL, &thread_function, NULL);
    if (err != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    // Simulate main thread doing some work
    printf("Main thread is running\n");
    sleep(10);

    // Cancel the thread
   /* pthread_cancel(thread);
    pthread_join(thread, NULL);*/

    return EXIT_SUCCESS;
}

