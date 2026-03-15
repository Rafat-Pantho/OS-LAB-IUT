/*
 * Lab-5 (Mutexes and Semaphores)
 * Part 1A (Mutex):  Hot-Swappable Configuration System
 * Part 1B (Semaphore): Dining Philosophers (Limited Seating)
 * Part 2A (Mutex):  Thread-Safe Histogram (Sharded Updates)
 * Part 2B (Semaphore): H2O Bond Formation (Barrier)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

/* ==================================================================
 * PART 1A: Hot-Swappable Configuration System (Mutex)
 * Invariant: active_config != NULL at all times
 * ================================================================== */

int *active_config = NULL;
pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int config_running = 1;

void *updater_thread(void *arg) {
    int update_num = 0;
    while (update_num < 3) {
        sleep(1);
        int *new_config = (int *)malloc(sizeof(int));
        *new_config = update_num * 100 + 42;

        pthread_mutex_lock(&config_mutex);
        int *old_config = active_config;
        active_config = new_config;  /* Atomic swap under lock */
        free(old_config);
        printf("[Updater] Config updated to: %d\n", *active_config);
        pthread_mutex_unlock(&config_mutex);

        update_num++;
    }
    config_running = 0;
    return NULL;
}

void *reader_thread(void *arg) {
    int id = *(int *)arg;
    while (config_running) {
        pthread_mutex_lock(&config_mutex);
        if (active_config != NULL)
            printf("[Reader %d] Reading config: %d\n", id, *active_config);
        pthread_mutex_unlock(&config_mutex);
        usleep(300000);
    }
    return NULL;
}

/* ==================================================================
 * PART 1B: Dining Philosophers - Limited Seating (Semaphore)
 * Strategy: Allow at most 4 out of 5 philosophers to sit
 * ================================================================== */

#define NUM_PHILOSOPHERS 5

sem_t room_capacity;   /* Counting semaphore initialized to 4 */
pthread_mutex_t forks[NUM_PHILOSOPHERS];

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left  = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int meal = 0; meal < 3; meal++) {
        printf("[Philosopher %d] Thinking...\n", id);
        usleep(100000);

        sem_wait(&room_capacity);          /* Wait for a seat */
        printf("[Philosopher %d] Seated, waiting for forks.\n", id);

        /* Pick up forks in consistent order to avoid deadlock */
        int first  = left < right ? left  : right;
        int second = left < right ? right : left;
        pthread_mutex_lock(&forks[first]);
        pthread_mutex_lock(&forks[second]);

        printf("[Philosopher %d] Eating (meal %d).\n", id, meal + 1);
        usleep(100000);

        pthread_mutex_unlock(&forks[second]);
        pthread_mutex_unlock(&forks[first]);

        sem_post(&room_capacity);          /* Leave the table */
        printf("[Philosopher %d] Finished eating, left table.\n", id);
    }
    return NULL;
}

/* ==================================================================
 * PART 2A: Thread-Safe Histogram (Sharded Updates, Mutex)
 * ================================================================== */

#define HIST_SIZE 10
#define DATA_SIZE 9
#define NUM_HIST_THREADS 2

int global_histogram[HIST_SIZE] = {0};
int data[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
pthread_mutex_t hist_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct { int start; int end; } HistRange;

void *histogram_thread(void *arg) {
    HistRange *range = (HistRange *)arg;
    int local_hist[HIST_SIZE] = {0};

    for (int i = range->start; i <= range->end; i++) {
        local_hist[data[i]]++;
    }

    /* Merge local into global under mutex */
    pthread_mutex_lock(&hist_mutex);
    for (int i = 0; i < HIST_SIZE; i++) {
        global_histogram[i] += local_hist[i];
    }
    pthread_mutex_unlock(&hist_mutex);
    return NULL;
}

/* ==================================================================
 * PART 2B: H2O Bond Formation (Barrier, Semaphores only)
 * ================================================================== */

sem_t h_ready;    /* Counts waiting H atoms */
sem_t o_ready;    /* Counts waiting O atoms */
sem_t bond_mutex; /* Binary semaphore for bond printing */
sem_t h_gate;     /* Controls when H atoms can print */
sem_t o_gate;     /* Controls when O atoms can print */

int h_count = 0;
sem_t h_count_mutex;

void *hydrogen_atom(void *arg) {
    int id = *(int *)arg;
    printf("[H%d] H Ready\n", id);
    sem_post(&h_ready);

    sem_wait(&h_gate);  /* Wait until bond is ready */
    printf("[H%d] Bonding!\n", id);
    return NULL;
}

void *oxygen_atom(void *arg) {
    int id = *(int *)arg;
    printf("[O%d] O Ready\n", id);
    sem_post(&o_ready);

    sem_wait(&o_gate);  /* Wait until two H atoms are ready */

    /* Collect two H atoms and form bond */
    sem_wait(&h_ready);
    sem_wait(&h_ready);

    sem_post(&h_gate);
    sem_post(&h_gate);
    sem_post(&o_gate);  /* Allow this O to proceed (itself) */

    printf("[O%d] Bond Formed! (H-H-O)\n", id);
    return NULL;
}

/* ==================================================================
 * MAIN
 * ================================================================== */

int main() {
    /* ---- Part 1A ---- */
    printf("\n===== PART 1A: Hot-Swappable Configuration =====\n");
    active_config = (int *)malloc(sizeof(int));
    *active_config = 0;

    int reader_ids[3] = {1, 2, 3};
    pthread_t updater, readers[3];
    pthread_create(&updater, NULL, updater_thread, NULL);
    for (int i = 0; i < 3; i++)
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    pthread_join(updater, NULL);
    for (int i = 0; i < 3; i++)
        pthread_join(readers[i], NULL);
    free(active_config);
    printf("Part 1A complete.\n");

    /* ---- Part 1B ---- */
    printf("\n===== PART 1B: Dining Philosophers =====\n");
    sem_init(&room_capacity, 0, NUM_PHILOSOPHERS - 1);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i], NULL);

    pthread_t philosophers[NUM_PHILOSOPHERS];
    int phil_ids[NUM_PHILOSOPHERS];
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        phil_ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &phil_ids[i]);
    }
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_join(philosophers[i], NULL);
    sem_destroy(&room_capacity);
    printf("All philosophers have eaten.\n");

    /* ---- Part 2A ---- */
    printf("\n===== PART 2A: Thread-Safe Histogram =====\n");
    HistRange ranges[NUM_HIST_THREADS] = {{0, 4}, {5, 8}};
    pthread_t hist_threads[NUM_HIST_THREADS];
    for (int i = 0; i < NUM_HIST_THREADS; i++)
        pthread_create(&hist_threads[i], NULL, histogram_thread, &ranges[i]);
    for (int i = 0; i < NUM_HIST_THREADS; i++)
        pthread_join(hist_threads[i], NULL);

    int total = 0;
    printf("Histogram:\n");
    for (int i = 0; i < HIST_SIZE; i++) {
        printf("  [%d]: %d\n", i, global_histogram[i]);
        total += global_histogram[i];
    }
    printf("Total count: %d (expected %d) - %s\n", total, DATA_SIZE,
           total == DATA_SIZE ? "INVARIANT HOLDS" : "INVARIANT VIOLATED");

    /* ---- Part 2B ---- */
    printf("\n===== PART 2B: H2O Bond Formation =====\n");
    sem_init(&h_ready, 0, 0);
    sem_init(&o_ready, 0, 0);
    sem_init(&h_gate,  0, 0);
    sem_init(&o_gate,  0, 0);

    /* Create 4 H atoms and 2 O atoms (forms 2 water molecules) */
    int h_ids[4] = {1, 2, 3, 4};
    int o_ids[2] = {1, 2};
    pthread_t h_threads[4], o_threads[2];

    for (int i = 0; i < 2; i++)
        pthread_create(&o_threads[i], NULL, oxygen_atom, &o_ids[i]);
    for (int i = 0; i < 4; i++)
        pthread_create(&h_threads[i], NULL, hydrogen_atom, &h_ids[i]);

    for (int i = 0; i < 4; i++) pthread_join(h_threads[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(o_threads[i], NULL);

    sem_destroy(&h_ready);
    sem_destroy(&o_ready);
    sem_destroy(&h_gate);
    sem_destroy(&o_gate);

    return 0;
}
