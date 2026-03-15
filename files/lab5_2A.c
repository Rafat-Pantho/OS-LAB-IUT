/*
 * Lab-5 2A
 * Part A (Mutex): Priority Logging System
 * Part B (Semaphore): Ordered Startup System
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

/* ============================================================
 * PART A: Priority Logging System (Mutex)
 * ============================================================ */

#define MAX_MESSAGES 16
#define MSG_LEN 64

typedef struct {
    int priority;       /* 0 = HIGH, 1 = LOW */
    char message[MSG_LEN];
} LogEntry;

LogEntry log_buffer[MAX_MESSAGES];
int log_count = 0;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_message(int priority, const char *msg) {
    pthread_mutex_lock(&log_mutex);
    log_buffer[log_count].priority = priority;
    strncpy(log_buffer[log_count].message, msg, MSG_LEN - 1);
    log_count++;
    pthread_mutex_unlock(&log_mutex);
}

/* Thread functions for Part A */
void *low_network(void *arg) {
    usleep(10000);
    log_message(1, "Network OK");
    return NULL;
}
void *high_disk(void *arg) {
    usleep(10000);
    log_message(0, "Disk Failure");
    return NULL;
}
void *low_login(void *arg) {
    usleep(10000);
    log_message(1, "User Login");
    return NULL;
}
void *high_power(void *arg) {
    usleep(10000);
    log_message(0, "Power Surge");
    return NULL;
}

/* ============================================================
 * PART B: Ordered Startup System (Semaphore)
 * ============================================================ */

sem_t sem_nav;    /* Signals: Power is ready -> Comm can proceed */
sem_t sem_launch; /* Signals: Comm is ready  -> Control can proceed */

void *power_thread(void *arg) {
    usleep(rand() % 100000); /* random start delay */
    printf("[Power Thread] Power Initialized. Signal sent.\n");
    sem_post(&sem_nav);
    return NULL;
}

void *comm_thread(void *arg) {
    usleep(rand() % 100000);
    printf("[Comm Thread] Waiting for Signal...\n");
    sem_wait(&sem_nav);
    printf("[Comm Thread] Communication Initialized. Signal sent.\n");
    sem_post(&sem_launch);
    return NULL;
}

void *control_thread(void *arg) {
    usleep(rand() % 100000);
    printf("[Control Thread] Waiting for Signal...\n");
    sem_wait(&sem_launch);
    printf("[Control Thread] Control System Initialized.\n");
    return NULL;
}

/* ============================================================
 * MAIN
 * ============================================================ */

int main() {
    printf("===== PART A: Priority Logging System =====\n");

    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, low_network, NULL);
    pthread_create(&t2, NULL, high_disk,   NULL);
    pthread_create(&t3, NULL, low_login,   NULL);
    pthread_create(&t4, NULL, high_power,  NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    /* Sort buffer: HIGH (0) before LOW (1) */
    for (int i = 0; i < log_count - 1; i++) {
        for (int j = i + 1; j < log_count; j++) {
            if (log_buffer[i].priority > log_buffer[j].priority) {
                LogEntry tmp = log_buffer[i];
                log_buffer[i] = log_buffer[j];
                log_buffer[j] = tmp;
            }
        }
    }

    printf("\n=== Buffer Dump ===\n");
    for (int i = 0; i < log_count; i++) {
        printf("[%s] %s\n",
               log_buffer[i].priority == 0 ? "HIGH" : "LOW",
               log_buffer[i].message);
    }

    printf("\n===== PART B: Ordered Startup System =====\n");

    sem_init(&sem_nav,    0, 0);
    sem_init(&sem_launch, 0, 0);

    pthread_t pt, ct, ctl;
    /* Start in random order: Control first, then Comm, then Power */
    pthread_create(&ctl, NULL, control_thread, NULL);
    usleep(10000);
    pthread_create(&ct,  NULL, comm_thread,    NULL);
    usleep(10000);
    pthread_create(&pt,  NULL, power_thread,   NULL);

    pthread_join(pt,  NULL);
    pthread_join(ct,  NULL);
    pthread_join(ctl, NULL);

    sem_destroy(&sem_nav);
    sem_destroy(&sem_launch);

    return 0;
}
