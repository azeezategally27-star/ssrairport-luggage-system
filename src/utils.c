#include "utils.h"
#include <stdio.h>

void hexdump(const void *data, int len) {
    const unsigned char *p = (const unsigned char*)data;
    for (int i=0;i<len;i++) {
        printf("%02x ", p[i]);
        if ((i&15)==15) printf("\n");
    }
    printf("\n");
}
