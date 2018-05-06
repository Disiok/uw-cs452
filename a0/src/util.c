
#include <util.h>

int strncmp(const char *s, const char *t, int n) {
    while(n--) {
        if (*s++ != *t++) {
            return *(unsigned char*)(s - 1) - *(unsigned char*)(t - 1); 
        } 
    }
    return 0;
}

char parse_char_arg(const char *s, int pos) {
    int n_spaces = pos + 1;
    while(n_spaces > 0) {
        while (*s != ' ') {
            s++; 
        } 
        n_spaces --;
        // Now we should be at the space before the next token
        s++;
        // Now we should be at the start of the next token
    }

    return *s;
}

int parse_int_arg(const char *s, int pos) {
    int n_spaces = pos + 1;
    while(n_spaces > 0) {
        while (*s != ' ') {
            s++; 
        } 
        n_spaces --;
        // Now we should be at the space before the next token
        s++;
        // Now we should be at the start of the next token
    }

    // Now we are at the number
    int n = 0;
    while (*s != ' ' && *s != '\0') {
        n = n * 10 + (*s - '0'); 
        s++;
    }

    return n;
}

