/*
 * CSE4502 Operating Systems Lab (Bash Scripting 2A) - Part B
 * Concurrent Multi-Account Transfer System
 *
 * Input: Amount1 Amount2 Amount3
 * Each Amounti satisfies: Amounti * Repeati = 1000, 1000 % Amounti == 0
 *
 * Usage: ./bank_transfer
 * Then enter: 100 50 25
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int A = 1000, B = 1000, C = 1000;

pthread_mutex_t mutex_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_B = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_C = PTHREAD_MUTEX_INITIALIZER;

int amount1, amount2, amount3;
int repeat1, repeat2, repeat3;

void transfer(int *src, int *dst,
              pthread_mutex_t *ms, pthread_mutex_t *md,
              int amount, const char *src_name, const char *dst_name, int tid) {

    /* Lock in pointer order to prevent deadlock */
    if (ms < md) { pthread_mutex_lock(ms); pthread_mutex_lock(md); }
    else         { pthread_mutex_lock(md); pthread_mutex_lock(ms); }

    if (*src >= amount) {
        *src -= amount;
        *dst += amount;
        printf("[T%d] Transferred %d from %s to %s\n", tid, amount, src_name, dst_name);
    }

    pthread_mutex_unlock(ms);
    pthread_mutex_unlock(md);
}

void *thread_T1(void *arg) {
    for (int i = 0; i < repeat1; i++)
        transfer(&A, &B, &mutex_A, &mutex_B, amount1, "A", "B", 1);
    printf("After T1 completes: A = %d, B = %d, C = %d\n", A, B, C);
    return NULL;
}

void *thread_T2(void *arg) {
    for (int i = 0; i < repeat2; i++)
        transfer(&B, &C, &mutex_B, &mutex_C, amount2, "B", "C", 2);
    printf("After T2 completes: A = %d, B = %d, C = %d\n", A, B, C);
    return NULL;
}

void *thread_T3(void *arg) {
    for (int i = 0; i < repeat3; i++)
        transfer(&C, &A, &mutex_C, &mutex_A, amount3, "C", "A", 3);
    printf("After T3 completes: A = %d, B = %d, C = %d\n", A, B, C);
    return NULL;
}

int main() {
    printf("Enter Amount1 Amount2 Amount3: ");
    scanf("%d %d %d", &amount1, &amount2, &amount3);

    if (1000 % amount1 != 0 || 1000 % amount2 != 0 || 1000 % amount3 != 0) {
        printf("Error: 1000 must be divisible by each amount.\n");
        return 1;
    }

    repeat1 = 1000 / amount1;
    repeat2 = 1000 / amount2;
    repeat3 = 1000 / amount3;

    printf("T1: Transfer %d from A->B, %d times\n", amount1, repeat1);
    printf("T2: Transfer %d from B->C, %d times\n", amount2, repeat2);
    printf("T3: Transfer %d from C->A, %d times\n", amount3, repeat3);
    printf("\n");

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, thread_T1, NULL);
    pthread_create(&t2, NULL, thread_T2, NULL);
    pthread_create(&t3, NULL, thread_T3, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("\nFinal Balances:\nA = %d\nB = %d\nC = %d\n", A, B, C);
    printf("Sum = %d (invariant %s)\n", A + B + C, A + B + C == 3000 ? "HOLDS" : "VIOLATED");

    return 0;
}
