#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX 5000000
#define THREAD_A_WEIGHT 0.50
#define THREAD_B_WEIGHT 0.20
#define THREAD_C_WEIGHT 0.15
#define THREAD_D_WEIGHT 0.15

struct {
    int current_number;
    int max_number;
} typedef prime_interval;

pthread_mutex_t mutex;
int total = 0;

double timediff(struct timeval t1, struct timeval t2) {
    return (double) t2.tv_sec - t1.tv_sec;
}

void *prime_number(void *param) {
    prime_interval *pi = (prime_interval*) param;
    struct timeval t1, t2;
    int eval;
    int multiples;

    gettimeofday(&t1, 0);   
    while (pi->current_number <= pi->max_number) {
        eval = 1;
        multiples = 0;

        while (eval <= pi->current_number && multiples <= 2) {
            multiples += (pi->current_number % eval) == 0;
            eval++;
        }

        pthread_mutex_lock(&mutex);
        total += multiples == 2;
        pthread_mutex_unlock(&mutex);

        pi->current_number++;
    }
    gettimeofday(&t2, 0);

    printf("\nThread finish with %fs | Max number: %d\n\n", timediff(t1, t2), pi->max_number);
}

int calc_range(int initial_number, int max_number, float weight) {
    return ((int) (max_number * weight)) + initial_number;
}

int calc_range_closed(int initial_number, int max_number, float weight, int thredhold) {
    int max_number_aux = calc_range(initial_number, max_number, weight);
    return max_number_aux - (max_number_aux - MAX);
}

void main(void) {
    prime_interval pi1, pi2, pi3, pi4;
    int max_number_aux = 0;
    
    pi1.current_number = 2;
    pi1.max_number = calc_range(2, MAX, THREAD_A_WEIGHT);

    pi2.current_number = pi1.max_number + 1;
    pi2.max_number = calc_range(pi2.current_number, MAX, THREAD_B_WEIGHT);

    pi3.current_number = pi2.max_number + 1;
    pi3.max_number = calc_range(pi3.current_number, MAX, THREAD_C_WEIGHT);

    pi4.current_number = pi3.max_number + 1;;
    pi4.max_number = calc_range_closed(pi4.current_number, MAX, THREAD_D_WEIGHT, MAX);

    printf("\n\nThread A: current: %d | Max: %d", pi1.current_number, pi1.max_number);
    printf("\n\nThread B: current: %d | Max: %d", pi2.current_number, pi2.max_number);
    printf("\n\nThread C: current: %d | Max: %d", pi3.current_number, pi3.max_number);
    printf("\n\nThread D: current: %d | Max: %d\n\n", pi4.current_number, pi4.max_number);

    struct timeval t1, t2;
    pthread_t th1, th2, th3, th4;

    pthread_mutex_init(&mutex, NULL);

    gettimeofday(&t1, 0);
    
    pthread_create(&th1, NULL, prime_number, (void*) &pi1);
    pthread_create(&th2, NULL, prime_number, (void*) &pi2);
    pthread_create(&th3, NULL, prime_number, (void*) &pi3);
    pthread_create(&th4, NULL, prime_number, (void*) &pi4);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);
    pthread_join(th4, NULL);
    
    gettimeofday(&t2, 0);

    printf("\n\nPrime numbers total: %d", total);
    printf("\nFinish with %.3fs\n\n", timediff(t1, t2));
    
    exit(EXIT_SUCCESS);
}