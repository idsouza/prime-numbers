#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

/*
 * Author: Igor Duarte de Souza
 * E-mail: igorduarte.s@gmail.com
 * RA: 1931480
 */

#define MAX_NUMBER 5000000

/*
 * Weights responsible for determining the load of ranges 
 * allocated to a specific thread. This distribution aims to
 * balance the algorithm complexity.
 */
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

    printf("\nThread finish with %fs | Max number: %d", timediff(t1, t2), pi->max_number);
}

int calc_max_number(int initial_number, int max_number, float weight) {
    return ((int) (max_number * weight)) + initial_number;
}

int calc_max_number_closed(int initial_number, int max_number, float weight, int thredhold) {
    int max_number_aux = calc_max_number(initial_number, max_number, weight);
    return max_number_aux - (max_number_aux - MAX_NUMBER);
}

void main(void) {
    prime_interval pi1, pi2, pi3, pi4;
    int max_number_aux = 0;
    
    pi1.current_number = 2;
    pi1.max_number = calc_max_number(2, MAX_NUMBER, THREAD_A_WEIGHT);

    pi2.current_number = pi1.max_number + 1;
    pi2.max_number = calc_max_number(pi2.current_number, MAX_NUMBER, THREAD_B_WEIGHT);

    pi3.current_number = pi2.max_number + 1;
    pi3.max_number = calc_max_number(pi3.current_number, MAX_NUMBER, THREAD_C_WEIGHT);

    pi4.current_number = pi3.max_number + 1;;
    pi4.max_number = calc_max_number_closed(pi4.current_number, MAX_NUMBER, THREAD_D_WEIGHT, MAX_NUMBER);

    
    printf("\n### Thread range distribution");
    printf("\nThread A: Current: %d | Max: %d", pi1.current_number, pi1.max_number);
    printf("\n\nThread B: Current: %d | Max: %d", pi2.current_number, pi2.max_number);
    printf("\n\nThread C: Current: %d | Max: %d", pi3.current_number, pi3.max_number);
    printf("\n\nThread D: Current: %d | Max: %d\n\n", pi4.current_number, pi4.max_number);

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