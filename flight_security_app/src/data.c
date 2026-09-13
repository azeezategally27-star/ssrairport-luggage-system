/* data.c - demo store and simulation feed */

#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "ai_detector.h"

static Event events[MAX_EVENTS];
static int event_n = 0;
static pthread_t sim_thread;
static int sim_running = 0;

static void random_init_event(Event *e, int idx) {
    snprintf(e->id, sizeof(e->id), "EV%03d", idx+1);
    snprintf(e->flight_id, sizeof(e->flight_id), "MK%03d", 100 + idx*7);
    e->hotspot_x = -1; e->hotspot_y = -1;
    e->state = EV_STATE_NORMAL;
    e->xray_sig = (rand()%100)/100.0; e->metal_val = (rand()%120)/100.0; e->weight_anomaly = (rand()%80)/100.0;
    e->watchlist_score = (rand()%100)/100.0; e->behavior_score = (rand()%60)/100.0;
    e->ts = time(NULL);
    // compute detector score
    e->threat_score = ai_infer(e->xray_sig, e->metal_val, e->weight_anomaly, e->watchlist_score, e->behavior_score);
    // hotspot if xray_sig high
    if (e->xray_sig > 0.75) { e->hotspot_x = 60 + (rand()%40); e->hotspot_y = 30 + (rand()%40); }
}

static void *sim_fn(void *arg) {
    (void)arg; sim_running = 1;
    while (sim_running) {
        // maintain a rolling window of events (max MAX_EVENTS)
        if (event_n < MAX_EVENTS) {
            random_init_event(&events[event_n], event_n);
            event_n++;
        } else {
            // rotate
            for (int i=0;i<MAX_EVENTS-1;i++) events[i]=events[i+1];
            random_init_event(&events[MAX_EVENTS-1], MAX_EVENTS-1);
        }
        // apply simple rules: if score > 0.8 escalate
        for (int i=0;i<event_n;i++) {
            if (events[i].threat_score > 0.85) events[i].state = EV_THREAT_ESCALATE;
            else if (events[i].threat_score > 0.6) events[i].state = EV_THREAT_HOLD;
            else events[i].state = EV_STATE_NORMAL;
        }
        sleep(3);
    }
    return NULL;
}

void data_init(void) {
    srand((unsigned)time(NULL)); event_n = 0;
    pthread_create(&sim_thread, NULL, sim_fn, NULL);
}

int data_event_count(void) { return event_n; }
void data_get_event(int idx, Event *out) { if (idx<0 || idx>=event_n) { memset(out,0,sizeof(Event)); return; } *out = events[idx]; }
int data_get_top_event(Event *out) {
    if (event_n==0) return -1;
    int idx = 0; double best = events[0].threat_score;
    for (int i=1;i<event_n;i++) if (events[i].threat_score > best) { best = events[i].threat_score; idx = i; }
    if (out) *out = events[idx]; return idx;
}

void data_shutdown(void) { sim_running = 0; pthread_join(sim_thread, NULL); }

void data_get_last_log(char *buf, int len) {
    FILE *f = fopen("flight_security_logs.txt","r");
    if (!f) { strncpy(buf, "(no logs)", len); return; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); long start = sz - 512; if (start<0) start=0; fseek(f, start, SEEK_SET);
    char tmp[512+1]; size_t r = fread(tmp,1,512,f); tmp[r]='\0'; fclose(f);
    strncpy(buf, tmp, len); buf[len-1]='\0';
}
