/*
 * CSE4502 Operating Systems Lab (Bash Scripting 1B) - Part B
 * Multi-Level Priority-Based Concurrent Logging
 *
 * All P1 messages printed before P2, P2 before P3, etc.
 * No sorting. Uses chained semaphores as barriers between priority levels.
 *
 * Usage: ./priority_logging <k> <n>
 *   k = number of priority levels
 *   n = number of threads per priority level
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_K 10
#define MAX_N 40

int K, N;

/*
 * Strategy (no counter variables, no shared state to track done threads):
 * Use a binary semaphore per priority level as a "barrier gate".
 * - gate[i] starts at 0 (locked).
 * - gate[0] starts at 1 (P1 threads can go immediately).
 * - When a P_i thread finishes printing, it signals gate[i].
 * - Before printing, every P_i thread waits on gate[i-1], then re-signals it
 *   for the next P_i thread (turnstile pattern).
 * - After all P_i threads are done, the LAST P_i thread opens gate[i] (signals N times).
 *
 * To avoid shared counters: use a secondary semaphore per level counting completions
 * implemented as a turnstile that only the last thread breaks open.
 *
 * Simpler alternative actually used here:
 * - Each priority level i has a semaphore entry_gate[i] (turnstile).
 * - entry_gate[0] = N (P1 threads all pass immediately).
 * - entry_gate[i] = 0 for i > 0 (locked until P_{i-1} all done).
 * - P_i thread: wait(entry_gate[i]), print, post(entry_gate[i]),
 *   then for the LAST thread: post entry_gate[i+1] N times.
 *
 * Detecting "last thread without a counter": use a counting semaphore
 * completion[i] initialized to N. Each P_i thread does a try-wait (sem_trywait).
 * The thread that drains it to 0 is responsible for opening the next gate.
 *
 * Actually the simplest correct approach: use a mutex + counter per level,
 * but the problem says "can you do this without a counter variable or shared state".
 * We solve this using only semaphores in a producer-consumer chain:
 *
 * Use a pipeline: level 0 threads print, then post one token to level_done[0].
 * A dedicated "relay" thread for level i waits for N tokens from level_done[i-1],
 * then posts N tokens to entry_gate[i].
 *
 * This way there are NO shared integer variables to track done threads.
 */

sem_t entry_gate[MAX_K];   /* Turnstile for each priority level */
sem_t level_done[MAX_K];   /* Each P_i thread posts here when done */
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct { int level; int id; } ThreadArg;

void *log_thread(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    int level = ta->level;
    int id    = ta->id;

    /* Wait for entry gate (turnstile) */
    sem_wait(&entry_gate[level]);
    sem_post(&entry_gate[level]);  /* Re-open for next thread at same level */

    /* Print atomically */
    pthread_mutex_lock(&print_mutex);
    printf("[P%d] Message %d\n", level + 1, id + 1);
    pthread_mutex_unlock(&print_mutex);

    /* Signal completion */
    sem_post(&level_done[level]);

    return NULL;
}

/* Relay thread: waits for all N completions at level i, then opens level i+1 */
typedef struct { int level; } RelayArg;

void *relay_thread(void *arg) {
    RelayArg *ra = (RelayArg *)arg;
    int level = ra->level;

    /* Wait for all N threads at this level to finish */
    for (int i = 0; i < N; i++)
        sem_wait(&level_done[level]);

    /* Open the gate for the NEXT level (if it exists) */
    if (level + 1 < K) {
        for (int i = 0; i < N; i++)
            sem_post(&entry_gate[level + 1]);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <k> <n>\n", argv[0]);
        printf("  k = number of priority levels (2-%d)\n", MAX_K);
        printf("  n = threads per level (1-%d)\n", MAX_N);
        return 1;
    }

    K = atoi(argv[1]);
    N = atoi(argv[2]);

    if (K < 2 || K > MAX_K || N < 1 || N > MAX_N) {
        printf("Invalid parameters.\n");
        return 1;
    }

    /* Initialize semaphores */
    for (int i = 0; i < K; i++) {
        sem_init(&level_done[i], 0, 0);
        if (i == 0)
            sem_init(&entry_gate[i], 0, N); /* Level 0 open immediately */
        else
            sem_init(&entry_gate[i], 0, 0); /* Other levels locked */
    }

    /* Create log threads */
    pthread_t log_threads[MAX_K][MAX_N];
    ThreadArg log_args[MAX_K][MAX_N];
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            log_args[i][j].level = i;
            log_args[i][j].id    = j;
            pthread_create(&log_threads[i][j], NULL, log_thread, &log_args[i][j]);
        }
    }

    /* Create relay threads */
    pthread_t relays[MAX_K];
    RelayArg relay_args[MAX_K];
    for (int i = 0; i < K; i++) {
        relay_args[i].level = i;
        pthread_create(&relays[i], NULL, relay_thread, &relay_args[i]);
    }

    /* Join all */
    for (int i = 0; i < K; i++)
        for (int j = 0; j < N; j++)
            pthread_join(log_threads[i][j], NULL);
    for (int i = 0; i < K; i++)
        pthread_join(relays[i], NULL);

    /* Cleanup */
    for (int i = 0; i < K; i++) {
        sem_destroy(&entry_gate[i]);
        sem_destroy(&level_done[i]);
    }

    return 0;
}
