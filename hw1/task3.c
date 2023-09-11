#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

extern char **environ;

void
print_environment() {
    char **env = environ;
    while (*env)
    {
        printf("%s\n", *env);
        env++;
    }
}

int 
main(int argc, char *argv[]) {
    int opt;
    int verbose = 0;
    int unset_var = 0;
    char *unset_var_name = NULL;

    while ((opt = getopt(argc, argv, "vu:")) != -1) {
        switch (opt)
        {
        case 'v':
            verbose = 1;
            break;
        case 'u':
            unset_var = 1;
            unset_var_name = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-v] [-u VAR] [NAME=VALUE]... [COMMAND]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (unset_var) {
        if (verbose) {
            fprintf(stderr, "Unsetting environment variable: %s.\n", unset_var_name);
        }
        
        if(unsetenv(unset_var_name) != 0){
            fprintf(stderr, "Error on unsetting environment variable.\n");
            return EXIT_FAILURE;
        }

        if(verbose){
            fprintf(stderr, "Environment variable %s was successfully unset.\n", unset_var_name);
        }
    }

    if (optind != argc)
    {
        // Handle environment variable assignments
        while (optind < argc && strchr(argv[optind], '='))
        {
            char *env_assignment = argv[optind];

            if(verbose) {
                fprintf(stderr, "Putting environment variable: %s.\n", env_assignment);
            }

            if (putenv(env_assignment) != 0)
            {
                fprintf(stderr, "Error on putting an environment variable.\n");
                return EXIT_FAILURE;
            }

            if(verbose) {
                fprintf(stderr, "Environment variable %s was successfuly put.\n", env_assignment);
            }

            optind++;
        }

        // Execute the command if there's one
        if (optind < argc) {
            if (verbose) {
                fprintf(stderr, "Started executing %s command.\n", argv[optind]);
            }

            execvp(argv[optind], &argv[optind]);

            if (verbose) {
                fprintf(stderr, "Command execution failed.");
            }
            return EXIT_FAILURE;
        }
    }

    if (verbose) {
        fprintf(stderr, "Printing current environment:\n");
    }

    print_environment();

    return EXIT_SUCCESS;
}