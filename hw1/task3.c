#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

extern char **environ;

void print_environment()
{
    char **env = environ;
    while (*env)
    {
        printf("%s\n", *env);
        env++;
    }
}

int main(int argc, char *argv[])
{
    opterr = 0;
    int opt;
    int verbose = 0;
    int unset_var = 0;

    // Handles options until it encounters not an option
    while ((optind < argc && strchr(argv[optind], '-')) && (opt = getopt(argc, argv, "vu:")) != -1)
    {
        switch (opt)
        {
        case 'v':
            verbose = 1;
            break;
        case 'u':
            if (verbose)
            {
                fprintf(stderr, "Unsetting environment variable: %s.\n", optarg);
            }

            if (unsetenv(optarg) != 0)
            {
                fprintf(stderr, "Error on unsetting environment variable.\n");
                return EXIT_FAILURE;
            }

            if (verbose)
            {
                fprintf(stderr, "Environment variable %s was successfully unset.\n", optarg);
            }
            break;
        default:
            fprintf(stderr, "Usage: %s [-v] [-u VAR] [NAME=VALUE]... [COMMAND]\n", argv[0]);
            return EXIT_FAILURE;
        }
        
        // If next argument is not an option, stop handling options
    }

    // Handle environment variable assignments
    while (optind < argc && strchr(argv[optind], '='))
    {
        char *env_assignment = argv[optind];

        if (verbose)
        {
            fprintf(stderr, "Putting environment variable: %s.\n", env_assignment);
        }

        if (putenv(env_assignment) != 0)
        {
            fprintf(stderr, "Error on putting an environment variable.\n");
            return EXIT_FAILURE;
        }

        if (verbose)
        {
            fprintf(stderr, "Environment variable %s was successfuly put.\n", env_assignment);
        }

        optind++;
    }

    // Execute the command if there's one
    if (optind < argc)
    {
        if (verbose)
        {
            int funcargs = optind;
            fprintf(stderr, "Started executing %s command with following arguments:\n", argv[optind]);
            while (funcargs + 1 < argc)
            {
                fprintf(stderr, "%s ", argv[funcargs + 1]);
                funcargs++;
            }
            fprintf(stderr, "\n");
        }

        execvp(argv[optind], argv + optind);

        if (verbose)
        {
            fprintf(stderr, "Command execution failed.");
        }
        return EXIT_FAILURE;
    }

    if (verbose)
    {
        fprintf(stderr, "Printing current environment:\n");
    }

    print_environment();

    return EXIT_SUCCESS;
}