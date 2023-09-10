#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

void print_environment() {
    char **env = environ;
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
}

void add_environment(const char *name, const char *value) {
    char *env_entry = (char *)malloc(strlen(name) + strlen(value) + 2);
    if (env_entry == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sprintf(env_entry, "%s=%s", name, value);
    putenv(env_entry);
}

void remove_environment(const char *name) {
    unsetenv(name);
}

int main(int argc, char *argv[]) {
    int verbose = 0;
    int opt;

    // Parse command line options
    while ((opt = getopt(argc, argv, "vu:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'u':
                remove_environment(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] [-u name] [name=value]... [command [arg]...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Process name=value pairs
    for (int i = optind; i < argc; i++) {
        char *equal_sign = strchr(argv[i], '=');
        if (equal_sign) {
            *equal_sign = '\0';
            add_environment(argv[i], equal_sign + 1);
        } else {
            break;
        }
    }

    // Execute a command if provided
    if (optind < argc) {
        if (verbose) {
            fprintf(stderr, "Executing: ");
            for (int i = optind; i < argc; i++) {
                fprintf(stderr, "%s ", argv[i]);
            }
            fprintf(stderr, "\n");
        }
        execvp(argv[optind], &argv[optind]);
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // If no command is provided, print the environment
    if (optind == argc) {
        print_environment();
    }

    return 0;
}