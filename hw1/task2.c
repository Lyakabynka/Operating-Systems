// Text segment:

// Contains executable instructions of the given following program.
// It is one of the sections of a program in an object file or in memory, which contains executable instructions.

// Data segment described in the program

// Stack segment described in the program

// Heap segment described in the program

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* strndup(const char* s, size_t n) { // Stack: const char* s, size_t n function paramethers
    char* p = NULL; // Data: NULL global variable | Stack: p local variable
    
    if (s) {
        size_t len = strlen(s); // Stack: len local variable, strlen(s) function call
        if (n < len) {
            len = n;
        }
        
        p = (char*)malloc(len + 1); // Heap: malloc(...) dynamic memory allocatin ( may be considered also as stack function call )
        if (p) {
            strncpy(p, s, len); // Stack: strncpy(...) function call
        }
    }
    
    return p; // Stack: function result
}

int main(void) {
    static char m[] = "HelloWorld!"; // Data: char[] static variable
    size_t len = strlen(m); // Stack: len local variable, strlen(m) function call
    
    for (size_t n = 1; n <= len; n++) { // Stack: n local variable
        char* p = strndup(m, n); // Stack: p local variable, strndup(...) function call
        if (!p) {
            perror("strndup"); // Data: "strndup" constant string | Stack: perror(...) function call
            return EXIT_FAILURE; // Data: EXIT_FAILURE global variable
        }
        
        if (puts(p) == EOF) { // Data: EOF global variable | Stack: puts(p) function call
            perror("puts"); // Data: "puts" constant string | Stack: perror(...) function call
            return EXIT_FAILURE; // Data: EXIT_FAILURE global variable
        }
        
        free(p); // Stack: free(p) function call
    }
    
    if (fflush(stdout) == EOF) { // Data: EOF, stdout global variables | Stack: fflush(...) function call
        perror("fflush"); // Data: "puts" constant string | Stack: perror(...) function call
        return EXIT_FAILURE; // Data: EXIT_FAILURE global variable
    }
    
    return EXIT_SUCCESS; // Data: EXIT_SUCCESS global variable
}