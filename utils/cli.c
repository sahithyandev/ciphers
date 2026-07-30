// Argument parsing for ciphers with a "[-d] <key> <message>" CLI.
#include <stdio.h>
#include <string.h>

// Parses an optional -d flag followed by a key and a message. On success sets
// the out params and returns 0; on bad usage prints the usage line and returns 1.
// *message points into argv, so it stays writable for in-place transforms.
int parse_args(int argc, char *argv[], int *decrypt, char **key, char **message) {
    int argi = 1;
    int bad_flag = 0;
    *decrypt = 0;
    if (argi < argc && strcmp(argv[argi], "-d") == 0) {
        *decrypt = 1;
        argi++;
    } else if (argi < argc && argv[argi][0] == '-') {
        bad_flag = 1;
    }

    if (bad_flag || argc - argi != 2) {
        fprintf(stderr, "Usage: %s [-d] <key> <message>\n", argv[0]);
        return 1;
    }

    *key = argv[argi];
    *message = argv[argi + 1];
    return 0;
}
