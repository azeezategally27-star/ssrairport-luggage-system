/* api.c - minimal TCP server that accepts commands and returns simple responses
 * Commands (text-based demo):
 * LIST_EVENTS\n  -> returns simple list
 * GET_EVENT idx\n -> returns event id and score
 * HOLD id\n -> append log and return OK
 */

#include "api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "data.h"

static pthread_t api_thread;
static int api_running = 0;
static int api_port = 0;

static void append_log(const char *user, const char *action, const char *details) {
    FILE *f = fopen("flight_security_logs.txt","a"); if (!f) return; fprintf(f, "%ld|%s|%s|%s\n", time(NULL), user, action, details); fclose(f);
}

static void handle_client(int client_fd) {
    char buf[1024]; ssize_t r = recv(client_fd, buf, sizeof(buf)-1, 0);
    if (r<=0) { close(client_fd); return; }
    buf[r]='\0';
    if (strncmp(buf, "LIST_EVENTS", 11)==0) {
        int n = data_event_count();
        char out[2048]; int pos=0; pos += snprintf(out+pos, sizeof(out)-pos, "[\n");
        for (int i=0;i<n;i++) {
            Event ev; data_get_event(i, &ev);
            pos += snprintf(out+pos, sizeof(out)-pos, " { \"id\": \"%s\", \"flight\": \"%s\", \"score\": %.3f },\n", ev.id, ev.flight_id, ev.threat_score);
        }
        pos += snprintf(out+pos, sizeof(out)-pos, "]\n");
        send(client_fd, out, strlen(out), 0);
    } else if (strncmp(buf, "GET_EVENT ", 10)==0) {
        int idx = atoi(buf+10);
        Event ev; data_get_event(idx, &ev);
        char out[512]; snprintf(out, sizeof(out), "{ \"id\":\"%s\", \"score\": %.3f }\n", ev.id, ev.threat_score);
        send(client_fd, out, strlen(out), 0);
    } else if (strncmp(buf, "HOLD ",5)==0) {
        char *id = buf+5; // simple
        append_log("remote", "hold", id);
        char out[64]; snprintf(out, sizeof(out), "{ \"status\": \"ok\", \"action\": \"hold\", \"id\": \"%s\" }\n", id);
        send(client_fd, out, strlen(out), 0);
    } else {
        const char *u = "{ \"error\": \"unknown command\" }\n"; send(client_fd, u, strlen(u), 0);
    }
    close(client_fd);
}

static void *api_thread_fn(void *arg) {
    (void)arg; api_running = 1;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return NULL; }
    struct sockaddr_in addr; addr.sin_family = AF_INET; addr.sin_addr.s_addr = inet_addr("127.0.0.1"); addr.sin_port = htons(api_port);
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(sock); return NULL; }
    if (listen(sock, 5) < 0) { perror("listen"); close(sock); return NULL; }

    while (api_running) {
        int client = accept(sock, NULL, NULL);
        if (client < 0) continue;
        handle_client(client);
    }
    close(sock);
    return NULL;
}

int api_start(int port) {
    api_port = port;
    pthread_create(&api_thread, NULL, api_thread_fn, NULL);
    return 0;
}

void api_stop(void) {
    api_running = 0; pthread_join(api_thread, NULL);
}
