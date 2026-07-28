#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char shift_cipher(char c, int shift) {
    char base = (c >= 'a' && c <= 'z') ? 'a' : 'A';
    shift = ((shift % 26) + 26) % 26;
    return base + (c - base + shift) % 26;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <key> <message>\n", argv[0]);
        return 1;
    }

    int key = atoi(argv[1]);
    char *message = argv[2];

    for (int i = 0; message[i] != '\0'; i++) {
        char ch = message[i];
        int is_alpha = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
        if (is_alpha) {
            message[i] = shift_cipher(ch, key);
        }
    }
    printf("%s\n", message);
    
    return 0;
}
