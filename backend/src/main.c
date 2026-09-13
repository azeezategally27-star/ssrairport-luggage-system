#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "agents.h"
#include "storage.h"

#define PORT 42042
#define BUFSIZE 4096

int handle_client(int client_fd) {
    char buf[BUFSIZE];
    ssize_t n = recv(client_fd, buf, sizeof(buf)-1, 0);
    if (n <= 0) return -1;
    buf[n] = '\0';

    // Trim trailing newlines
    while (n>0 && (buf[n-1]=='\n' || buf[n-1]=='\r')) { buf[n-1]='\0'; n--; }

    if (strcmp(buf, "GRAPH") == 0) {
        const char *resp = get_graph_json();
        send(client_fd, resp, strlen(resp), 0);
    } else if (strcmp(buf, "SCAN_FACE") == 0) {
        const char *resp = scan_face_response();
        send(client_fd, resp, strlen(resp), 0);
        append_log("SCAN_FACE -> returned simulated match\n");
    } else if (strncmp(buf, "REGISTER ", 9) == 0) {
        const char *payload = buf + 9;
        register_biometric(payload);
        const char *resp = register_response();
        send(client_fd, resp, strlen(resp), 0);
    } else if (strcmp(buf, "LOGS") == 0) {
        char *logs = read_logs();
        if (logs) {
            send(client_fd, logs, strlen(logs), 0);
            free(logs);
        } else {
            const char *empty = "[]";
            send(client_fd, empty, strlen(empty), 0);
        }
    } else {
        const char *unknown = "{ \"status\": \"error\", \"message\": \"unknown command\" }";
        send(client_fd, unknown, strlen(unknown), 0);
    }

    return 0;
}

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }
    if (listen(server_fd, 5) < 0) { perror("listen"); return 1; }

    printf("ssrairportd demo daemon listening on 127.0.0.1:%d\n", PORT);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) { perror("accept"); continue; }
        handle_client(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
