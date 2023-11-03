#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // if no input files provided, read from standard input
        char buffer[4096];
        int lines = 0;
        int words = 0;
        int bytes = 0;
        int is_word = 0;
        
        while (1) {
            int n = read(0, buffer, sizeof(buffer));
            if (n <= 0) break;

            for (int i = 0; i < n; i++) {
                bytes++;
                if (isspace(buffer[i])) {
                    if (is_word) {
                        words++;
                        is_word = 0;
                    }
                    if (buffer[i] == '\n') {
                        lines++;
                    }
                } else {
                    is_word = 1;
                }
            }
        }

        printf("%d %d %d\n", lines, words, bytes);
    } else {
        int total_lines = 0;
        int total_words = 0;
        int total_bytes = 0;

        for (int i = 1; i < argc; i++) {
            //file descriptor
            int fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                perror("open");
                continue;
            }

            //retrieve file attributes
            struct stat st;
            if (fstat(fd, &st) == -1) {
                perror("fstat");
                close(fd);
                continue;
            }

            char *file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (file_data == MAP_FAILED) {
                perror("mmap");
                close(fd);
                continue;
            }

            int lines = 0;
            int words = 0;
            int bytes = 0;
            int is_word = 0;

            for (off_t i = 0; i < st.st_size; i++) {
                bytes++;
                if (isspace(file_data[i])) {
                    if (is_word) {
                        words++;
                        is_word = 0;
                    }
                    if (file_data[i] == '\n') {
                        lines++;
                    }
                } else {
                    is_word = 1;
                }
            }

            printf("%d %d %d %s\n", lines, words, bytes, argv[i]);

            total_lines += lines;
            total_words += words;
            total_bytes += bytes;

            munmap(file_data, st.st_size);
            close(fd);
        }

        if (argc > 2) {
            printf("%d %d %d total\n", total_lines, total_words, total_bytes);
        }
    }

    return 0;
}
