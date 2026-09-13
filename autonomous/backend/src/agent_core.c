#include "agent_core.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static agent_t g_agents[MAX_AGENTS];
static int g_agent_count = 0;

void agents_init(){
    srand((unsigned)time(NULL));
    // create 6 example agents
    g_agent_count = 6;
    for(int i=0;i<g_agent_count;i++){
        agent_t *a = &g_agents[i];
        a->id = i+1;
        snprintf(a->name, sizeof(a->name), "agent-%d", a->id);
        a->x = (float)(rand()%800 - 400) / 10.0f;
        a->y = (float)(rand()%600 - 300) / 10.0f;
        a->tx = a->x; a->ty = a->y;
        a->state = AGENT_IDLE;
        a->battery = 0.5f + (rand()%50)/100.0f;
        strcpy(a->task, "idle");
    }
}

static float approach(float cur, float target, float maxd){
    float d = target - cur; if(fabsf(d) <= maxd) return target; return cur + (d>0?maxd:-maxd);
}

void agents_update(int ms){
    float step = (ms/1000.0f) * 1.2f; // speed units per second
    for(int i=0;i<g_agent_count;i++){
        agent_t *a = &g_agents[i];
        // simple battery drain
        a->battery -= (ms/1000.0f) * 0.0015f; if(a->battery < 0) a->battery = 0;
        if(a->state == AGENT_MOVING){
            a->x = approach(a->x, a->tx, step);
            a->y = approach(a->y, a->ty, step);
            if(a->x == a->tx && a->y == a->ty){
                a->state = AGENT_WORKING;
                snprintf(a->task, sizeof(a->task), "performing");
            }
        } else if(a->state == AGENT_WORKING){
            // work for a while then idle
            if(rand()%100 < 4){ a->state = AGENT_IDLE; strcpy(a->task, "idle"); }
        } else if(a->state == AGENT_IDLE){
            if(rand()%200 < 3){
                // pick a random roam target
                a->tx = a->x + (float)(rand()%200 - 100)/10.0f;
                a->ty = a->y + (float)(rand()%200 - 100)/10.0f;
                a->state = AGENT_MOVING;
                snprintf(a->task, sizeof(a->task), "moving");
            }
        } else if(a->state == AGENT_CHARGING){
            a->battery += (ms/1000.0f) * 0.0025f; if(a->battery >= 1.0f){ a->battery = 1.0f; a->state = AGENT_IDLE; strcpy(a->task, "idle"); }
        }
    }
}

int agents_count(){ return g_agent_count; }

int agents_get_json(char *buf, int bufsz){
    int off = 0; off += snprintf(buf+off, bufsz-off, "[");
    for(int i=0;i<g_agent_count && off < bufsz-100;i++){
        agent_t *a = &g_agents[i];
        off += snprintf(buf+off, bufsz-off, "{\"id\":%d,\"name\":\"%s\",\"x\":%.2f,\"y\":%.2f,\"battery\":%.2f,\"state\":%d,\"task\":\"%s\"}",
                 a->id, a->name, a->x, a->y, a->battery, (int)a->state, a->task);
        if(i < g_agent_count-1) off += snprintf(buf+off, bufsz-off, ",");
    }
    off += snprintf(buf+off, bufsz-off, "]\n");
    return off;
}

void agents_send_command(int id, const char *cmd){
    for(int i=0;i<g_agent_count;i++){
        agent_t *a = &g_agents[i];
        if(a->id == id){
            if(strncmp(cmd, "goto", 4) == 0){
                float nx=0, ny=0; if(sscanf(cmd+4, "%f %f", &nx, &ny) >= 2){ a->tx = nx; a->ty = ny; a->state = AGENT_MOVING; strncpy(a->task, "manual", sizeof(a->task)-1);} }
            else if(strncmp(cmd, "charge", 6) == 0){ a->state = AGENT_CHARGING; strncpy(a->task, "charging", sizeof(a->task)-1); }
            else if(strncmp(cmd, "stop", 4) == 0){ a->state = AGENT_IDLE; strncpy(a->task, "stopped", sizeof(a->task)-1); }
            break;
        }
    }
}
