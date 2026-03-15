/*
 * CSE4502 Operating Systems Lab (Bash Scripting 2B) - Part B
 * Sequential Character Printing Using Semaphores
 *
 * n threads print A, B, C, ... in strict alphabetical order
 * despite starting in arbitrary order and random delays.
 *
 * Usage: ./seq_print <n>   (1 <= n <= 26)
 *
 * Strategy:
 *   sem[i] is initially 0. Thread i waits on sem[i], prints its character,
 *   then signals sem[i+1]. Thread 0 starts with sem[0] = 1.
 *   This creates a semaphore chain ensuring strict sequential output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_N 26

int N;
sem_t sems[MAX_N + 1]; /* sems[i] controls when thread i may print */

void delay_ms() {
    unsigned int milliseconds = (rand() * 100) % 500;
    usleep(milliseconds * 1000); /* Convert ms to microseconds */
}

typedef struct { int id; } ThreadArg;

void *print_char(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    int id = ta->id;

    delay_ms(); /* Random delay before printing */

    sem_wait(&sems[id]);                /* Wait for our turn */
    printf("%c ", 'A' + id);
    fflush(stdout);
    sem_post(&sems[id + 1]);            /* Signal next thread */

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <n>  (1 <= n <= 26)\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    if (N < 1 || N > MAX_N) {
        printf("n must be between 1 and 26.\n");
        return 1;
    }

    srand(42);

    /* Initialize all semaphores to 0; only thread 0's gate is open */
    for (int i = 0; i <= N; i++)
        sem_init(&sems[i], 0, 0);
    sem_post(&sems[0]); /* Thread 0 can go first */

    pthread_t threads[MAX_N];
    ThreadArg args[MAX_N];

    /* Shuffle thread creation order to demonstrate ordering is independent of start */
    int order[MAX_N];
    for (int i = 0; i < N; i++) order[i] = i;
    /* Simple Fisher-Yates shuffle */
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
    }

    for (int i = 0; i < N; i++) {
        args[order[i]].id = order[i];
        pthread_create(&threads[order[i]], NULL, print_char, &args[order[i]]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    printf("\n");

    for (int i = 0; i <= N; i++)
        sem_destroy(&sems[i]);

    return 0;
}
