#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define COINS_COUNT 20 // number of coins on the table

char coins[COINS_COUNT] = "XXXXXXXXXX0000000000";
int persons = 100; // number of persons
int N = 10000;     // number of times person flips all 20 coins;

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_3[COINS_COUNT];

static void run_threads(int thread_count, void *(*proc)(void *))
{
    pthread_t *threads = calloc(thread_count, sizeof(pthread_t));

    for (int i = 0; i < thread_count; i++)
    {
        (void)pthread_create(&threads[i], NULL, proc, NULL);
    }

    for (int i = 0; i < thread_count; i++)
    {
        (void)pthread_join(threads[i], NULL);
    }

    free(threads);
}

static double timeit(int n, void *(*proc)(void *))
{
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc);
    t2 = clock();
    return ((double)t2 - (double)t1) / CLOCKS_PER_SEC * 1000;
}

static void flip(int pos)
{
    switch (rand() % 2)
    {
    case 0:
        coins[pos] = 'X';
        break;
    case 1:
        coins[pos] = 'O';
        break;
    }
}

static void *flip_the_coins_1(void *arg)
{
    (void)pthread_mutex_lock(&mutex_1);
    for (int i = 0; i < 10000; i++)
    {
        for (int j = 0; j < COINS_COUNT; j++)
        {
            flip(j);
        }
    }
    (void)pthread_mutex_unlock(&mutex_1);

    return NULL;
}

static void *flip_the_coins_2(void *arg)
{
    for (int i = 0; i < 10000; i++)
    {
        (void)pthread_mutex_lock(&mutex_1);
        for (int j = 0; j < COINS_COUNT; j++)
        {
            flip(j);
        }
        (void)pthread_mutex_unlock(&mutex_1);
    }

    return NULL;
}

static void *flip_the_coins_3(void *arg)
{
    for (int i = 0; i < 10000; i++)
    {
        for (int j = 0; j < COINS_COUNT; j++)
        {
            (void)pthread_mutex_lock(&mutex_3[j]);
            flip(j);
            (void)pthread_mutex_unlock(&mutex_3[j]);
        }
    }

    return NULL;
}

void print_coins(char* to_append)
{
    fprintf(stderr, "coins: ");
    for(int i = 0; i < COINS_COUNT; i++)
    {
        fprintf(stderr, "%c", coins[i]);
    }
    fprintf(stderr, " (%s) \n", to_append);
}

void print_info(double elapsed_ms)
{
    fprintf(stderr, "%d threads x %d flips:  %.3lf ms \n", persons, N, elapsed_ms);
}

int main(int argc, char *argv[])
{
    int opt;
    srand(time(0));

    while ((opt = getopt(argc, argv, "p:n:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            persons = atoi(optarg);
            fprintf(stdout, "Persons: %d \n", persons);
            break;
        case 'n':
            N = atoi(optarg);
            fprintf(stdout, "Number of times each person flips all 20 coins: %d \n", N);
            break;
        default:
            fprintf(stderr, "Usage: %s [-p VAR] [-n VAR] \n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // mutex initialization for case 3
    for (int i = 0; i < COINS_COUNT; i++)
    {
        pthread_mutex_init(&mutex_3[i], NULL);
    }

    double elapsed_ms;

    print_coins("start - glabal lock");
    elapsed_ms = timeit(persons, flip_the_coins_1);
    print_coins("end - global lock");
    print_info(elapsed_ms);
    fprintf(stderr,"\n");

    print_coins("start - iteration lock");
    elapsed_ms = timeit(persons, flip_the_coins_2);
    print_coins("end - table lock");
    print_info(elapsed_ms);
    fprintf(stderr,"\n");

    print_coins("start - coin lock");
    elapsed_ms = timeit(persons, flip_the_coins_3);
    print_coins("end - coin lock");
    print_info(elapsed_ms);
    fprintf(stderr,"\n");

    return EXIT_SUCCESS;
}