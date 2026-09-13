#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

#define LOGFILE "backend_logs.txt"

void append_log(const char *line) {
    FILE *f = fopen(LOGFILE, "a");
    if (!f) return;
    fprintf(f, "%s", line);
    fclose(f);
}

char *read_logs(void) {
    FILE *f = fopen(LOGFILE, "r");
    if (!f) {
        // Return empty JSON array
        char *empty = malloc(3);
        strcpy(empty, "[]");
        return empty;
    }

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1 + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t r = fread(buf, 1, sz, f);
    buf[r] = '\0';
    fclose(f);

    // For demo, wrap lines into a JSON array
    char *out = malloc(sz + 64);
    strcpy(out, "[");
    char *line = strtok(buf, "\n");
    int first = 1;
    while (line) {
        if (!first) strcat(out, ",");
        strcat(out, "\"");
        // escape quotes simple
        for (char *p = line; *p; ++p) {
            if (*p == '"') strcat(out, "\\\"");
            else {
                size_t l = strlen(out);
                out[l] = *p;
                out[l+1] = '\0';
            }
        }
        strcat(out, "\"");
        first = 0;
        line = strtok(NULL, "\n");
    }
    strcat(out, "]");
    free(buf);
    return out;
}
