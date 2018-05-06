
#include <util.h>

int strncmp(const char *s, const char *t, int n) {
    while(n--) {
        if (*s++ != *t++) {
            return *(unsigned char*)(s - 1) - *(unsigned char*)(t - 1); 
        } 
    }
    return 0;
}

