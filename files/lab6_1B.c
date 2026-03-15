/*
 * Lab 6 - 1B
 * Part A: Sequential Launch of Space Systems (Semaphore)
 * Part B: Readers-Writers Synchronization Problem (Semaphore)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* ==================================================================
 * PART A: Sequential Launch - Fuel -> Navigation -> Launch
 * sem_nav    : signals that Fuel is ready   (Navigation waits on it)
 * sem_launch : signals that Navigation ready (Control waits on it)
 * ================================================================== */

sem_t sem_nav;
sem_t sem_launch;

void *fuel_thread(void *arg) {
    usleep(rand() % 100000);
    printf("[Fuel Thread] Fuel system activated. Signal sent.\n");
    sem_post(&sem_nav);
    return NULL;
}

void *navigation_thread(void *arg) {
    usleep(rand() % 100000);
    printf("[Navigation Thread] Waiting for fuel system...\n");
    sem_wait(&sem_nav);
    printf("[Navigation Thread] Navigation system online. Signal sent.\n");
    sem_post(&sem_launch);
    return NULL;
}

void *launch_thread(void *arg) {
    usleep(rand() % 100000);
    printf("[Launch Thread] Waiting for navigation system...\n");
    sem_wait(&sem_launch);
    printf("[Launch Thread] Launch sequence initiated!\n");
    return NULL;
}

/* ==================================================================
 * PART B: Readers-Writers Problem
 * mutex      : protects read_count
 * write_block: blocks writers when readers active (and vice versa)
 * ================================================================== */

int shared_data = 0;
int read_count  = 0;

sem_t mutex;       /* protects read_count */
sem_t write_block; /* ensures exclusive write access */

void *reader(void *arg) {
    int id = *(int *)arg;
    printf("[Reader %d] Waiting to read...\n", id);

    sem_wait(&mutex);
    read_count++;
    if (read_count == 1)
        sem_wait(&write_block); /* First reader blocks writers */
    sem_post(&mutex);

    /* --- Critical Section (Read) --- */
    printf("[Reader %d] Reading data: %d\n", id, shared_data);
    usleep(100000);
    /* --- End Critical Section --- */

    sem_wait(&mutex);
    read_count--;
    if (read_count == 0)
        sem_post(&write_block); /* Last reader unblocks writers */
    sem_post(&mutex);

    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;
    printf("[Writer %d] Waiting to write...\n", id);

    sem_wait(&write_block);

    /* --- Critical Section (Write) --- */
    printf("[Writer %d] Writing data...\n", id);
    shared_data++;
    usleep(150000);
    printf("[Writer %d] Updated data to %d\n", id, shared_data);
    /* --- End Critical Section --- */

    sem_post(&write_block);

    return NULL;
}

/* ==================================================================
 * MAIN
 * ================================================================== */

int main() {
    /* ---- Part A ---- */
    printf("===== PART A: Sequential Space System Launch =====\n");
    sem_init(&sem_nav,    0, 0);
    sem_init(&sem_launch, 0, 0);

    pthread_t pt, nt, lt;
    /* Threads started in a "wrong" order to demonstrate synchronization */
    pthread_create(&lt, NULL, launch_thread,     NULL);
    usleep(5000);
    pthread_create(&nt, NULL, navigation_thread, NULL);
    usleep(5000);
    pthread_create(&pt, NULL, fuel_thread,       NULL);

    pthread_join(pt, NULL);
    pthread_join(nt, NULL);
    pthread_join(lt, NULL);

    sem_destroy(&sem_nav);
    sem_destroy(&sem_launch);

    /* ---- Part B ---- */
    printf("\n===== PART B: Readers-Writers Problem =====\n");
    sem_init(&mutex,       0, 1);
    sem_init(&write_block, 0, 1);

    int reader_ids[3] = {1, 2, 3};
    int writer_ids[2] = {1, 2};
    pthread_t readers[3], writers[2];

    /* Start readers and writers concurrently */
    pthread_create(&readers[0], NULL, reader, &reader_ids[0]);
    pthread_create(&readers[1], NULL, reader, &reader_ids[1]);
    pthread_create(&writers[0], NULL, writer, &writer_ids[0]);
    usleep(50000);
    pthread_create(&readers[2], NULL, reader, &reader_ids[2]);
    pthread_create(&writers[1], NULL, writer, &writer_ids[1]);

    for (int i = 0; i < 3; i++) pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(writers[i], NULL);

    sem_destroy(&mutex);
    sem_destroy(&write_block);

    return 0;
}
