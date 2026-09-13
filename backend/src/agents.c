#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agents.h"

// Return a small JSON describing the mini LangChain graph
const char *get_graph_json(void) {
    return "{ \"nodes\": ["
           "{\"id\":\"retriever\",\"label\":\"DocumentRetriever\"},"
           "{\"id\":\"qa\",\"label\":\"QA Agent\"},"
           "{\"id\":\"bio\",\"label\":\"Biometric Agent\"},"
           "{\"id\":\"decision\",\"label\":\"Decision Node\"},"
           "{\"id\":\"logger\",\"label\":\"Logger\"}"
           "], \"edges\": ["
           "{\"from\":\"retriever\",\"to\":\"qa\"},"
           "{\"from\":\"qa\",\"to\":\"decision\"},"
           "{\"from\":\"bio\",\"to\":\"decision\"},"
           "{\"from\":\"decision\",\"to\":\"logger\"}"
           "] }";
}

const char *scan_face_response(void) {
    // Simulated response
    return "{ \"status\": \"ok\", \"match\": true, \"id\": \"P007\", \"score\": 0.87 }";
}

void register_biometric(const char *payload) {
    // In demo mode, we simply write a log or ignore payload
    (void)payload;
}

const char *register_response(void) {
    return "{ \"status\": \"ok\", \"message\": \"registered (demo)\" }";
}
