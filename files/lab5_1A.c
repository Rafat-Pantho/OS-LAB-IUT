/*
 * Lab-5 1A
 * Part A (Mutex): Multi-Account Transfer System
 * Part B (Semaphore): Limited-Capacity Transaction Server
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* ============================================================
 * PART A: Multi-Account Transfer System (Mutex)
 * ============================================================ */

int A = 1000, B = 1000, C = 1000;

pthread_mutex_t mutex_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_B = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_C = PTHREAD_MUTEX_INITIALIZER;

/* Helper: transfer amount from src to dst with ordered mutex locking */
void transfer(int *src, int *dst, pthread_mutex_t *m_src, pthread_mutex_t *m_dst,
              int amount, const char *src_name, const char *dst_name, int thread_id) {

    /* Lock in a consistent global order to avoid deadlock */
    if (m_src < m_dst) {
        pthread_mutex_lock(m_src);
        pthread_mutex_lock(m_dst);
    } else {
        pthread_mutex_lock(m_dst);
        pthread_mutex_lock(m_src);
    }

    printf("[T%d] Transfer START: %d from %s -> %s\n", thread_id, amount, src_name, dst_name);

    if (*src >= amount) {
        *src -= amount;
        *dst += amount;
        printf("[T%d] Transfer END:   %d from %s -> %s  (A=%d B=%d C=%d)\n",
               thread_id, amount, src_name, dst_name, A, B, C);
    } else {
        printf("[T%d] Transfer SKIPPED: insufficient funds in %s\n", thread_id, src_name);
    }

    pthread_mutex_unlock(m_src);
    pthread_mutex_unlock(m_dst);
}

void *thread_T1(void *arg) {
    /* Transfer 100 from A -> B, 10 times */
    for (int i = 0; i < 10; i++) {
        transfer(&A, &B, &mutex_A, &mutex_B, 100, "A", "B", 1);
    }
    printf("[T1] Done. A=%d B=%d C=%d\n", A, B, C);
    return NULL;
}

void *thread_T2(void *arg) {
    /* Transfer 50 from B -> C, 20 times */
    for (int i = 0; i < 20; i++) {
        transfer(&B, &C, &mutex_B, &mutex_C, 50, "B", "C", 2);
    }
    printf("[T2] Done. A=%d B=%d C=%d\n", A, B, C);
    return NULL;
}

void *thread_T3(void *arg) {
    /* Transfer 25 from C -> A, 40 times */
    for (int i = 0; i < 40; i++) {
        transfer(&C, &A, &mutex_C, &mutex_A, 25, "C", "A", 3);
    }
    printf("[T3] Done. A=%d B=%d C=%d\n", A, B, C);
    return NULL;
}

/* ============================================================
 * PART B: Limited-Capacity Transaction Server (Semaphore)
 * ============================================================ */

#define NUM_CLIENTS 6
#define MAX_CONCURRENT 3

sem_t server_sem;
pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;
int being_served[NUM_CLIENTS + 1]; /* 1-indexed */
int active_count = 0;

void print_status() {
    pthread_mutex_lock(&status_mutex);
    for (int i = 1; i <= NUM_CLIENTS; i++) {
        printf("C%d=%s", i, being_served[i] ? "True" : "False");
        if (i < NUM_CLIENTS) printf(", ");
    }
    printf("\n");
    pthread_mutex_unlock(&status_mutex);
}

void *client_thread(void *arg) {
    int id = *(int *)arg;

    sem_wait(&server_sem);

    pthread_mutex_lock(&status_mutex);
    being_served[id] = 1;
    active_count++;
    pthread_mutex_unlock(&status_mutex);

    print_status();
    sleep(1); /* Simulate service */

    pthread_mutex_lock(&status_mutex);
    being_served[id] = 0;
    active_count--;
    pthread_mutex_unlock(&status_mutex);

    print_status();
    sem_post(&server_sem);

    return NULL;
}

/* ============================================================
 * MAIN
 * ============================================================ */

int main() {
    printf("===== PART A: Multi-Account Transfer System =====\n");
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, thread_T1, NULL);
    pthread_create(&t2, NULL, thread_T2, NULL);
    pthread_create(&t3, NULL, thread_T3, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    printf("Final: A=%d, B=%d, C=%d (Sum=%d)\n\n", A, B, C, A + B + C);

    printf("===== PART B: Limited-Capacity Transaction Server =====\n");
    sem_init(&server_sem, 0, MAX_CONCURRENT);

    pthread_t clients[NUM_CLIENTS];
    int ids[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; i++) {
        ids[i] = i + 1;
        being_served[i + 1] = 0;
    }
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_create(&clients[i], NULL, client_thread, &ids[i]);
        usleep(50000); /* slight stagger so output is readable */
    }
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }
    sem_destroy(&server_sem);

    return 0;
}
