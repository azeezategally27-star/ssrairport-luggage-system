#ifndef AGENT_CORE_H
#define AGENT_CORE_H

#include <stdbool.h>

#define MAX_AGENTS 32

typedef enum { AGENT_IDLE=0, AGENT_MOVING, AGENT_WORKING, AGENT_CHARGING } agent_state_t;

typedef struct {
    int id;
    char name[32];
    float x,y; // position
    float tx,ty; // target
    agent_state_t state;
    float battery; // 0..1
    char task[64];
} agent_t;

void agents_init();
void agents_update(int ms);
int agents_count();
// fill buf (size bufsz) with a JSON array of agents; returns bytes written
int agents_get_json(char *buf, int bufsz);
// send a simple command to an agent (e.g., "goto x y" or "charge")
void agents_send_command(int id, const char *cmd);

#endif
