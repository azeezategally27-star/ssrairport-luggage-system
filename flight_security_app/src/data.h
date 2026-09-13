/* data.h - structures for events and simple in-memory store */

#ifndef DATA_H
#define DATA_H

#define MAX_EVENTS 16

typedef enum { EV_STATE_NORMAL=0, EV_THREAT_HOLD=1, EV_THREAT_ESCALATE=2 } EvState;

typedef struct {
    char id[32];
    char flight_id[32];
    double threat_score; // 0..1
    int hotspot_x, hotspot_y; // -1 if none
    EvState state;
    // raw sensor features (for explainability)
    double xray_sig;
    double metal_val;
    double weight_anomaly;
    double watchlist_score;
    double behavior_score;
    long ts;
} Event;

void data_init(void);
int data_event_count(void);
void data_get_event(int idx, Event *out);
int data_get_top_event(Event *out);
void data_shutdown(void);
void data_get_last_log(char *buf, int len);

#endif
