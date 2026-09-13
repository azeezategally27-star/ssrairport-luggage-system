/* data.h - simple in-memory flight store and persistence helpers */

#ifndef DATA_H
#define DATA_H

typedef struct {
    char id[32];
    char origin[64];
    char dest[64];
    char status[32];
    char scheduled_time[64];
    char estimated_time[64];
    char gate[8];
    double baggage_progress;
    int passengers;
    int priority_count;
    long last_update_ts;
} Flight;

int load_demo_data(const char *path);
int flight_count(void);
Flight *get_flight_by_index(int idx);
Flight *get_flight_by_id(const char *id);
void reschedule_flight(const char *id, int delta_minutes);
void append_log(const char *user, const char *action, const char *details);
int save_logs(const char *path);

// simulation feed
void start_simulation_feed(void);
void stop_simulation_feed(void);

#endif
