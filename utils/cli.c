// Argument parsing for ciphers with a "[-d] <key> <message>" or
// "[-d] <message>" CLI.
#include <stdio.h>
#include <string.h>

// Parses a leading optional -d flag. Sets *decrypt and returns the index of
// the first positional argument, or -1 if an unrecognized flag was given.
static int parse_flag(int argc, char *argv[], int *decrypt) {
    int argi = 1;
    *decrypt = 0;
    if (argi < argc && strcmp(argv[argi], "-d") == 0) {
        *decrypt = 1;
        argi++;
    } else if (argi < argc && argv[argi][0] == '-') {
        return -1;
    }
    return argi;
}

// Parses an optional -d flag followed by a key and a message. On success sets
// the out params and returns 0; on bad usage prints the usage line and returns 1.
// *message points into argv, so it stays writable for in-place transforms.
int parse_args(int argc, char *argv[], int *decrypt, char **key, char **message) {
    int argi = parse_flag(argc, argv, decrypt);
    if (argi < 0 || argc - argi != 2) {
        fprintf(stderr, "Usage: %s [-d] <key> <message>\n", argv[0]);
        return 1;
    }

    *key = argv[argi];
    *message = argv[argi + 1];
    return 0;
}

// Same as parse_args, but for ciphers that take no key (e.g. Atbash).
int parse_args_no_key(int argc, char *argv[], int *decrypt, char **message) {
    int argi = parse_flag(argc, argv, decrypt);
    if (argi < 0 || argc - argi != 1) {
        fprintf(stderr, "Usage: %s [-d] <message>\n", argv[0]);
        return 1;
    }

    *message = argv[argi];
    return 0;
}
