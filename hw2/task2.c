#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_RUNS 1000

int main(int argc, char *argv[])
{
    int opt;
    int duration = 5;
    int warmup_runs = 0;

    while ((optind < argc && strchr(argv[optind], '-')) && (opt = getopt(argc, argv, "d:w:")) != -1)
    {
        switch (opt)
        {
        case 'd':
            duration = atoi(optarg);
            printf("Duration: %d \n", duration);
            break;
        case 'w':
            warmup_runs = atoi(optarg);
            printf("Warmup runs: %d \n", warmup_runs);
            break;
        default:
            fprintf(stderr, "Usage: %s [-d VAR] [-w VAR] [COMMAND]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (optind >= argc)
    {
        fprintf(stderr, "Usage: %s [-d VAR] [-w VAR] [COMMAND]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // start

    pid_t pid;

    struct timespec start, end;
    double min_time = -1, max_time = -1, total_time = 0;
    int fails = 0, runs = 0;

    char *command = argv[optind];
    //redirecting output to /dev/null to prevent writing command outputs into console
    freopen("/dev/null", "w", stdout);

    for (int i = 0; i < warmup_runs; i++)
    {
        if ((pid = fork()) == 0)
        {
            execvp(command, &argv[optind]);

            exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            (void)waitpid(pid, &status, 0);

            i++;
        }
    }

    for (; runs < MAX_RUNS && total_time < duration;)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);

        if ((pid = fork()) == 0)
        {
            execvp(command, &argv[optind]);

            exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            int wpid = waitpid(pid, &status, 0);
            if (wpid == -1)
            {
                fails++;
                continue;
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

            if (min_time == -1 || elapsed_time < min_time)
            {
                min_time = elapsed_time;
            }

            if (max_time == -1 || elapsed_time > max_time)
            {
                max_time = elapsed_time;
            }

            total_time += elapsed_time;

            runs++;
        }
    }

    //redirecting back
    freopen("/dev/tty","w",stdout);

    printf("Summary:\n");
    printf("Min:   %.6lf seconds.\t", min_time);
    printf("Warmups: %d.\n", warmup_runs);
    printf("Avg:   %.6lf seconds.\t", total_time / runs);
    printf("Runs: %d.\n", runs);
    printf("Max:   %.6lf seconds.\t", max_time);
    printf("Fails: %d.\n", fails);
    printf("Total: %.6lf seconds.\n", total_time);

    return EXIT_SUCCESS;
}