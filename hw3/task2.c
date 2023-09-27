#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <pthread.h>

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

void *check_isperfect_with_bounds(void *arg)
{
    for (int i = lower_bound; i <= higher_bound; i++)
    {
        if (is_perfect(i))
        {
            printf("%d\n", i);
        }
    }
}

int main(int argc, char *argv[])
{
    int opt;
    int lower_bound = 1;
    int higher_bound = 10000;
    bool verbose = false;
    int thread_count = 1;

    while ((opt = getopt(argc, argv, "s:e:t:v:")) != -1)
    {
        switch (opt)
        {
        case 's':
            lower_bound = atoi(optarg);
            printf("Lower bound: %d \n", lower_bound);
            break;
        case 'e':
            higher_bound = atoi(optarg);
            printf("Higher bound: %d \n", higher_bound);
            break;
        case 't':
            thread_count = atoi(optarg);

            break;
        default:
            fprintf(stderr, "Usage: %s [-s VAR] [-e VAR]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < thread_count; i++)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, check_isperfect_with_bounds, NULL);

        pthread_join(thread);
    }

    return EXIT_SUCCESS;
}