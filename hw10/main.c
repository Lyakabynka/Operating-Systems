#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int opt;

    char* port;
    char mode = 'b';
    while ((opt = getopt(argc, argv, "ftp:")) != -1)
    {
        switch (opt)
        {
        case 'f':
        case 't':
            mode = opt;
            break;
        case 'p':
            port = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-f] [-t] [-p port]\n", argv[0]);
            return 1;
        }
    }


}