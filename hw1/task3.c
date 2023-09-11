#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

extern char **environ;

void print_environment() {
    char **env = environ;
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int verbose = 0;
    int unset_var = 0;
    char *new_env = NULL;
    
    while ((opt = getopt(argc, argv, "vu:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'u':
                unset_var = 1;
                new_env = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] [-u VAR] [NAME=VALUE]... [COMMAND [ARG]...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        // No arguments provided, print current environment
        if (unset_var) {
            unsetenv(new_env);
        }
        if (verbose) {
            fprintf(stderr, "Printing current environment:\n");
        }
        print_environment();
    } else {
        // Handle environment variable assignments
        while (optind < argc && strchr(argv[optind], '=')) {
            char *env_assignment = argv[optind];
            if (putenv(env_assignment) != 0) {
                perror("putenv");
                exit(EXIT_FAILURE);
            }
            optind++;
        }

        // Print the modified environment if -v is specified
        if (verbose) {
            fprintf(stderr, "Modified environment:\n");
            print_environment();
        }

        // Execute the command if there's one
        if (optind < argc) {
            execvp(argv[optind], &argv[optind]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}