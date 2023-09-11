#define _DEFAULT_SOUCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Count the number of characters in a string that are an element of the
 * character set delim. Returns 0 if none of the characters in the string
 * is in the character set delim or the string is empty.
 */
size_t strcnt(const char* string, const char* delim) {
    size_t cnt = 0;
    
    for (const char* s = string; *s; s++) { // here it should be *s != '\0'
        for (const char* d = delim; *d; d++) { // here it also should be *s != '\0'
            if (*s == *d) {
                cnt++;
                break;
            }
        }
    }
    
    return cnt;
}

/*
 * Split the string whenever a character appears that is in the
 * character set delim. Return a NULL-terminated vector of pointers to
 * the sub-strings.
 */
char** strsplit(char** string, const char* delim) {
    char* token;
    size_t cnt = strcnt(*string, delim);

    char** splitv = calloc(cnt + 1, sizeof(char)); // there should be < calloc(cnt+1, sizeof(char*)) > instead of simply char
    
    if (splitv) {
        for (int i = 0; (token = strsep(string, delim)); i++) {
            splitv[i] = token;
        }
    }
    
    return splitv;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        char** splitv = strsplit(&argv[i], " ");
        if (splitv) {
            for (int j = 0; splitv[j]; j++) {
                (void)puts(splitv[j]); // the memory should be deallocated after each iteration ( because strsep placed several '\0' )
                // here: (void)free(splitv[j])
            }
            (void)free(splitv);
        }
    }
    
    return EXIT_SUCCESS;
}
