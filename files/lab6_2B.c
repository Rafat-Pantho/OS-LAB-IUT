/*
 * Lab 6 - 2B
 * Part 1: Parallel Histogram Computation Using Pthreads
 * Part 2: H2O Bond Formation (Barrier, Semaphores Only)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* ==================================================================
 * PART 1: Parallel Histogram Computation
 * ================================================================== */

#define SUB_ARRAY_COUNT 4
#define SUB_ARRAY_DIM   3
#define RANGE           10

int data_array[SUB_ARRAY_COUNT][SUB_ARRAY_DIM][SUB_ARRAY_DIM][SUB_ARRAY_DIM];
int global_histogram[RANGE] = {0};
pthread_mutex_t hist_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Provided initialization function - do NOT modify */
void init_data_array() {
    srand(0);
    for (int i = 0; i < SUB_ARRAY_COUNT; i++)
        for (int j = 0; j < SUB_ARRAY_DIM; j++)
            for (int k = 0; k < SUB_ARRAY_DIM; k++)
                for (int l = 0; l < SUB_ARRAY_DIM; l++)
                    data_array[i][j][k][l] = rand() % RANGE;
}

void *compute_histogram(void *arg) {
    int sub_id = *(int *)arg;
    int local_hist[RANGE] = {0};

    /* Compute local histogram for this sub-array */
    for (int j = 0; j < SUB_ARRAY_DIM; j++)
        for (int k = 0; k < SUB_ARRAY_DIM; k++)
            for (int l = 0; l < SUB_ARRAY_DIM; l++)
                local_hist[data_array[sub_id][j][k][l]]++;

    /* Merge into global histogram under the single allowed mutex */
    pthread_mutex_lock(&hist_mutex);
    for (int i = 0; i < RANGE; i++)
        global_histogram[i] += local_hist[i];
    pthread_mutex_unlock(&hist_mutex);

    return NULL;
}

/* ==================================================================
 * PART 2: H2O Bond Formation (Semaphores Only)
 *
 * Strategy:
 *  - h_sem: H atoms post here to signal they're ready (count of waiting H)
 *  - o_sem: O atoms post here to signal they're ready (count of waiting O)
 *  - bond_trigger: O waits until 2 H atoms are available, then fires
 *  - h_proceed / o_proceed: gates to release exactly 2 H + 1 O per bond
 * ================================================================== */

sem_t h_sem;       /* H atoms post when ready        */
sem_t h_proceed;   /* Gate: H waits here until bond   */
sem_t o_proceed;   /* Gate: O waits here until bond   */

void *hydrogen(void *arg) {
    int id = *(int *)arg;
    printf("H%d Ready\n", id);
    sem_post(&h_sem);          /* Signal: one more H available */

    sem_wait(&h_proceed);      /* Wait for bonding signal */
    printf("H%d Bonded!\n", id);
    return NULL;
}

void *oxygen(void *arg) {
    int id = *(int *)arg;
    printf("O%d Ready\n", id);

    /* Wait for 2 H atoms */
    sem_wait(&h_sem);
    sem_wait(&h_sem);

    printf("O%d Ready -> Bond Formed!\n", id);

    /* Release 2 H atoms and self */
    sem_post(&h_proceed);
    sem_post(&h_proceed);
    sem_post(&o_proceed);

    sem_wait(&o_proceed);  /* O waits for its own release */
    printf("O%d Bonded!\n", id);
    return NULL;
}

/* ==================================================================
 * MAIN
 * ================================================================== */

int main() {
    /* ---- Part 1: Histogram ---- */
    printf("===== PART 1: Parallel Histogram Computation =====\n");
    init_data_array();

    int expected = SUB_ARRAY_COUNT * SUB_ARRAY_DIM * SUB_ARRAY_DIM * SUB_ARRAY_DIM;
    pthread_t threads[SUB_ARRAY_COUNT];
    int sub_ids[SUB_ARRAY_COUNT];

    for (int i = 0; i < SUB_ARRAY_COUNT; i++) {
        sub_ids[i] = i;
        pthread_create(&threads[i], NULL, compute_histogram, &sub_ids[i]);
    }
    for (int i = 0; i < SUB_ARRAY_COUNT; i++)
        pthread_join(threads[i], NULL);

    printf("Global Histogram:\n");
    int total = 0;
    for (int i = 0; i < RANGE; i++) {
        printf("  [%d]: %d\n", i, global_histogram[i]);
        total += global_histogram[i];
    }
    printf("Total: %d, Expected: %d -> %s\n\n", total, expected,
           total == expected ? "VERIFIED" : "MISMATCH");

    /* ---- Part 2: H2O ---- */
    printf("===== PART 2: H2O Bond Formation =====\n");
    sem_init(&h_sem,     0, 0);
    sem_init(&h_proceed, 0, 0);
    sem_init(&o_proceed, 0, 0);

    /* 4 H atoms + 2 O atoms = 2 water molecules */
    int h_ids[4] = {1, 2, 3, 4};
    int o_ids[2] = {1, 2};
    pthread_t h_threads[4], o_threads[2];

    /* Create O threads first (they wait for H) */
    for (int i = 0; i < 2; i++)
        pthread_create(&o_threads[i], NULL, oxygen, &o_ids[i]);
    usleep(10000);
    for (int i = 0; i < 4; i++)
        pthread_create(&h_threads[i], NULL, hydrogen, &h_ids[i]);

    for (int i = 0; i < 4; i++) pthread_join(h_threads[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(o_threads[i], NULL);

    sem_destroy(&h_sem);
    sem_destroy(&h_proceed);
    sem_destroy(&o_proceed);

    printf("All bonds formed!\n");
    return 0;
}
