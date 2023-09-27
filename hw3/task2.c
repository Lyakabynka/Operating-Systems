#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t mutex;

struct thread_args
{
    int lower_bound;
    int higher_bound;
};

// Function to check if a number is perfect
static int is_perfect(uint64_t num)
{
    uint64_t i, sum;

    if (num < 2)
    {
        return 0;
    }

    for (i = 2, sum = 1; i * i <= num; i++)
    {
        if (num % i == 0)
        {
            sum += (i * i == num) ? i : (i + num / i);
        }
    }

    return (sum == num);
}

static void *check_isperfect_with_bounds_multithreads(void *arg)
{
    struct thread_args *args = (struct thread_args *)arg;
    for (int i = args->lower_bound; i <= args->higher_bound; i++)
    {
        if (is_perfect(i))
        {
            sem_wait(&mutex);
            
            fprintf(stdout,"%d\n", i);

            sem_post(&mutex);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int opt;
    int lower_bound = 1;
    int higher_bound = 10000;
    int verbose = 0;
    int thread_count = 1;

    while ((opt = getopt(argc, argv, "s:e:t:v")) != -1)
    {
        switch (opt)
        {
        case 's':
            lower_bound = atoi(optarg);
            fprintf(stdout,"Lower bound: %d \n", lower_bound);
            break;
        case 'e':
            higher_bound = atoi(optarg);
            fprintf(stdout,"Higher bound: %d \n", higher_bound);
            break;
        case 't':
            thread_count = atoi(optarg);
            if (thread_count < 1)
            {
                fprintf(stderr, "Usage: %s [-s VAR] [-e VAR] [-t VAR] [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            break;
        case 'v':
            verbose = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-s VAR] [-e VAR] [-t VAR] [-v]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    pthread_t *threads = calloc(thread_count, sizeof(pthread_t));

    struct thread_args args[thread_count];

    int step = (higher_bound - lower_bound + 1) / thread_count;
    for (int i = 0; i < thread_count; i++)
    {
        args[i].lower_bound = lower_bound + i * step;
        args[i].higher_bound = i == thread_count - 1 ? higher_bound : lower_bound + (i+1) * step - 1;
    }

    sem_init(&mutex, 0, 1);

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(&threads[i], NULL, check_isperfect_with_bounds_multithreads, &args[i]);

        if (verbose)
        {
            fprintf(stderr, "%s: t%d searching [%d,%d]\n", argv[0], i, args[i].lower_bound, args[i].higher_bound);
        }
    }


    for (int i = 0; i < thread_count; i++)
    {
        if(pthread_join(threads[i], NULL) != 0){
            fprintf(stderr, "Error when joining thread #%d\n", i);
            exit(EXIT_FAILURE);
        }

        if (verbose)
        {
            fprintf(stderr, "%s: t%d finishing\n", argv[0], i);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fprintf(stdout,"Elapsed time: %lf\n", elapsed_time);

    free(threads);

    return EXIT_SUCCESS;
}