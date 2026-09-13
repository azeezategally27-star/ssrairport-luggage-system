/* ai_assistant.c - demo AI assistant heuristics */

#include "ai_assistant.h"
#include <stdio.h>
#include <string.h>

int ai_get_suggestion(const char *flight_id, char *out, size_t out_len) {
    // Demo heuristic: if flight contains '3' suggest +20 minutes; otherwise OK
    if (strstr(flight_id, "3")) {
        snprintf(out, out_len, "Suggest reschedule +20 minutes due to baggage lag and busy evening slot.");
    } else {
        snprintf(out, out_len, "No reschedule needed; on-track with low delay risk.");
    }
    return 0;
}
