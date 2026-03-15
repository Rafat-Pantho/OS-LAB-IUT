/*
 * CSE4502 Operating Systems Lab 9: 2B
 * Dining Philosophers - Phase 1: Deadlock Demo, Phase 2: Resolved
 *
 * Usage: ./dining <N> <P>
 *   N = number of philosophers
 *   P = 1 (deadlock demo) | 2 (resolved, terminates)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PHILOSOPHERS 20

int N;
pthread_mutex_t forks[MAX_PHILOSOPHERS];

/* ---- State tracking ---- */
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum { WAITING_2, WAITING_1, EATING } State;
State states[MAX_PHILOSOPHERS];

void print_states() {
    pthread_mutex_lock(&print_mutex);
    for (int i = 0; i < N; i++) {
        const char *s;
        switch (states[i]) {
            case WAITING_2: s = "Waiting(2 forks)"; break;
            case WAITING_1: s = "Waiting(1 fork) "; break;
            case EATING:    s = "Eating          "; break;
            default:        s = "Unknown         "; break;
        }
        printf("P%d:[%s]  ", i, s);
    }
    printf("\n");
    pthread_mutex_unlock(&print_mutex);
}

/* ============================================================
 * PHASE 1: Deadlock - every philosopher picks up LEFT fork first
 * ============================================================ */

void *philosopher_deadlock(void *arg) {
    int id = *(int *)arg;
    int left  = id;
    int right = (id + 1) % N;

    states[id] = WAITING_2;
    print_states();

    pthread_mutex_lock(&forks[left]);   /* Pick up left fork */
    states[id] = WAITING_1;
    print_states();
    usleep(100000);                      /* Simulate delay - leads to deadlock */

    pthread_mutex_lock(&forks[right]);  /* Try to pick up right - DEADLOCK */
    states[id] = EATING;
    print_states();
    usleep(200000);

    pthread_mutex_unlock(&forks[right]);
    pthread_mutex_unlock(&forks[left]);

    return NULL;
}

/* ============================================================
 * PHASE 2: Resolved - limit table capacity to N-1
 * ============================================================ */

sem_t room_sem;

void *philosopher_resolved(void *arg) {
    int id = *(int *)arg;

    for (int meal = 0; meal < 2; meal++) {
        states[id] = WAITING_2;
        print_states();

        sem_wait(&room_sem);            /* Limit concurrent diners */

        /* Pick forks in consistent order */
        int first  = id < (id + 1) % N ? id : (id + 1) % N;
        int second = id < (id + 1) % N ? (id + 1) % N : id;

        pthread_mutex_lock(&forks[first]);
        states[id] = WAITING_1;
        print_states();

        pthread_mutex_lock(&forks[second]);
        states[id] = EATING;
        print_states();

        usleep(200000);

        pthread_mutex_unlock(&forks[second]);
        pthread_mutex_unlock(&forks[first]);

        sem_post(&room_sem);
        usleep(100000);
    }
    return NULL;
}

/* ============================================================
 * MAIN
 * ============================================================ */

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <N_philosophers> <phase (1 or 2)>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    int phase = atoi(argv[2]);

    if (N < 2 || N > MAX_PHILOSOPHERS) {
        printf("N must be between 2 and %d\n", MAX_PHILOSOPHERS);
        return 1;
    }

    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
        states[i] = WAITING_2;
    }

    pthread_t philosophers[MAX_PHILOSOPHERS];
    int ids[MAX_PHILOSOPHERS];
    for (int i = 0; i < N; i++) ids[i] = i;

    if (phase == 1) {
        printf("=== PHASE 1: Deadlock Demonstration (will NOT terminate) ===\n");
        for (int i = 0; i < N; i++)
            pthread_create(&philosophers[i], NULL, philosopher_deadlock, &ids[i]);
        for (int i = 0; i < N; i++)
            pthread_join(philosophers[i], NULL);
    } else {
        printf("=== PHASE 2: Resolved Simulation (will terminate) ===\n");
        sem_init(&room_sem, 0, N - 1);
        for (int i = 0; i < N; i++)
            pthread_create(&philosophers[i], NULL, philosopher_resolved, &ids[i]);
        for (int i = 0; i < N; i++)
            pthread_join(philosophers[i], NULL);
        sem_destroy(&room_sem);
        printf("\nAll philosophers have eaten. System terminated.\n");
    }

    for (int i = 0; i < N; i++)
        pthread_mutex_destroy(&forks[i]);

    return 0;
}
