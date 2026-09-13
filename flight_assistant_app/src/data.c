/* data.c - minimal data store and simulated feed */

#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define MAX_FLIGHTS 64
static Flight flights[MAX_FLIGHTS];
static int flights_n = 0;
static pthread_t sim_thread;
static int sim_running = 0;

int load_demo_data(const char *path) {
    // For demo, we'll populate with a few made-up Mauritius flights
    flights_n = 0;
    Flight f;
    memset(&f,0,sizeof(f));
    strcpy(f.id, "MK101"); strcpy(f.origin, "MRU"); strcpy(f.dest, "JFK"); strcpy(f.status,"Scheduled"); strcpy(f.scheduled_time, "2026-09-14T12:30:00Z"); strcpy(f.gate, "A3"); f.baggage_progress=10; f.passengers=180; flights[flights_n++]=f;
    memset(&f,0,sizeof(f));
    strcpy(f.id, "MK202"); strcpy(f.origin, "MRU"); strcpy(f.dest, "LHR"); strcpy(f.status,"Boarding"); strcpy(f.scheduled_time, "2026-09-14T13:15:00Z"); strcpy(f.gate, "B1"); f.baggage_progress=60; f.passengers=220; flights[flights_n++]=f;
    memset(&f,0,sizeof(f));
    strcpy(f.id, "MK303"); strcpy(f.origin, "MRU"); strcpy(f.dest, "CDG"); strcpy(f.status,"Delayed"); strcpy(f.scheduled_time, "2026-09-14T14:00:00Z"); strcpy(f.gate, "C2"); f.baggage_progress=25; f.passengers=150; flights[flights_n++]=f;
    return 0;
}

int flight_count(void) { return flights_n; }
Flight *get_flight_by_index(int idx) { if (idx<0 || idx>=flights_n) return NULL; return &flights[idx]; }
Flight *get_flight_by_id(const char *id) { for (int i=0;i<flights_n;i++) if (strcmp(flights[i].id,id)==0) return &flights[i]; return NULL; }

void reschedule_flight(const char *id, int delta_minutes) {
    Flight *f = get_flight_by_id(id);
    if (!f) return;
    // naive string manipulation for demo (not timezone-aware)
    append_log("operator","reschedule", id);
    // change scheduled_time to indicate delta (demo)
    strcat(f->scheduled_time, " +" ); // visual marker only
}

void append_log(const char *user, const char *action, const char *details) {
    FILE *f = fopen("flight_assistant_logs.txt","a");
    if (!f) return; time_t t=time(NULL);
    fprintf(f, "%ld|%s|%s|%s\n", (long)t, user, action, details);
    fclose(f);
}

int save_logs(const char *path) {
    // for demo, copy existing log file to given path
    FILE *src = fopen("flight_assistant_logs.txt","r");
    if (!src) return -1;
    FILE *dst = fopen(path, "w");
    if (!dst) { fclose(src); return -1; }
    char buf[1024]; size_t r;
    while ((r=fread(buf,1,sizeof(buf),src))>0) fwrite(buf,1,r,dst);
    fclose(src); fclose(dst); return 0;
}

static void *sim_thread_fn(void *arg) {
    (void)arg;
    sim_running = 1;
    while (sim_running) {
        // simple simulation: slightly change baggage progress randomly
        for (int i=0;i<flights_n;i++) {
            Flight *f = &flights[i];
            if (f->baggage_progress < 100) {
                f->baggage_progress += (rand()%5);
                if (f->baggage_progress > 100) f->baggage_progress = 100;
                f->last_update_ts = time(NULL);
            }
        }
        sleep(2);
    }
    return NULL;
}

void start_simulation_feed(void) {
    pthread_create(&sim_thread, NULL, sim_thread_fn, NULL);
}
void stop_simulation_feed(void) { sim_running = 0; pthread_join(sim_thread, NULL); }
