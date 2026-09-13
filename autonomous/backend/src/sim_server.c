#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#include "agent_core.h"
#include "sensors.h"
#include "scheduler.h"

#define PORT 9292
#define BACKLOG 4

static int server_sock = -1;

static void handle_client(int client){
    char buf[1024]; ssize_t r = recv(client, buf, sizeof(buf)-1, 0);
    if(r <= 0) { close(client); return; }
    buf[r] = '\0';
    // simple protocol: "GET /agents" or "CMD <id> <cmd>"
    if(strncmp(buf, "GET /agents", 11) == 0){
        char json[4096]; agents_get_json(json, sizeof(json));
        send(client, json, strlen(json), 0);
    } else if(strncmp(buf, "CMD", 3) == 0){
        int id; char cmd[128]; if(sscanf(buf+3, "%d %127[\n\r\t ]%*s", &id, cmd) >= 1){
            // fallback parsing: try parse id and rest of line
            // simpler: parse with strtok
            char *p = strchr(buf, ' '); if(p){ p++; if(sscanf(p, "%d %127[^"]", &id, cmd) >= 1){ scheduler_send_command(id, p + (int)strcspn(p, " ")); send(client, "OK\n", 3, 0); } else send(client, "ERR\n", 4,0);} else send(client, "ERR\n", 4,0);
        } else {
            send(client, "ERR\n", 4, 0);
        }
    } else {
        send(client, "ERR\n", 4, 0);
    }
    close(client);
}

int start_server(){
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) { perror("socket"); return -1; }
    int opt = 1; setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr; memset(&addr,0,sizeof(addr)); addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(PORT);
    if(bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){ perror("bind"); close(server_sock); return -1; }
    if(listen(server_sock, BACKLOG) < 0){ perror("listen"); close(server_sock); return -1; }
    return 0;
}

int accept_one_client(){
    struct sockaddr_in cli; socklen_t clilen = sizeof(cli);
    int c = accept(server_sock, (struct sockaddr*)&cli, &clilen);
    return c;
}

int main(int argc, char **argv){
    printf("Autonomous agents server starting...\n");
    agents_init(); sensors_init(); scheduler_init();
    if(start_server() != 0){ fprintf(stderr, "Failed to start server\n"); return 1; }
    fd_set readfds;
    struct timeval tv;
    int maxfd = server_sock;
    int tick_ms = 100;
    while(1){
        // update simulation
        sensors_update(tick_ms);
        agents_update(tick_ms);
        scheduler_update(tick_ms);
        // use select to accept any clients without blocking main loop
        FD_ZERO(&readfds); FD_SET(server_sock, &readfds);
        tv.tv_sec = 0; tv.tv_usec = tick_ms * 1000;
        int rv = select(server_sock+1, &readfds, NULL, NULL, &tv);
        if(rv > 0 && FD_ISSET(server_sock, &readfds)){
            int client = accept_one_client();
            if(client >= 0){
                handle_client(client);
            }
        }
        // continue loop
    }
    return 0;
}
