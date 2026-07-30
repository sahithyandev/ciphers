char to_lower(char c) {
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

char to_upper(char c) {
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}
