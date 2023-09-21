#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

int main(char* argv[], int argc)
{
    int duration = 5;
    int warmup_runs = 0;

    while ((optind < argc && strchr(argv[optind], '-')) && (opt = getopt(argc, argv, "dw:")) != -1)
    {
        switch (opt)
        {
        case 'd':
            duration = optarg;
            printf("Duration: %d \n", duration);
            break;
        case 'w':
            warmup_runs = optarg;
            printf("Warmup runs: %d \n", warmup_runs);
            break;
        default:
            fprintf(stderr, "Usage: %s [-d VAR] [-w VAR] [COMMAND]\n", argv[0]);
            return EXIT_FAILURE;
        }
        // If next argument is not an option, stop handling options
    }

    argv[optind];

    printf(clock_gettime());

    prtinf(CLOCK_MONOTONIC);
}