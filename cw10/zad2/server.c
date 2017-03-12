#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <time.h>
#include "main.h"

int inet_socket;
int unix_socket;
char * path;
long port_number;
int nsc;
fd_set socket_set;

struct sockaddr_un unix_address;
struct sockaddr_in inet_address;

client clients[MAX_CLIENTS];

void handler(int sig) {
    exit(0);
}
void clean() {
    close(inet_socket);
    close(unix_socket);
    remove(path);
}

int register_client(request request1, int client_socket) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state != INACTIVE) {
            if (strcmp(clients[i].name, request1.sender) == 0) {
                clients[i].time = time(NULL);
                clients[i].client_socket = client_socket;
                clients[i].tmp = 1;
                return 0;
            }
        }
    }
    int free_client_slot;
    for (free_client_slot = 0; free_client_slot < MAX_CLIENTS && clients[free_client_slot].state != INACTIVE; free_client_slot++);
    if (free_client_slot == MAX_CLIENTS)
        return 1;
    strcpy(clients[free_client_slot].name, request1.sender);
    clients[free_client_slot].time = time(NULL);
    clients[free_client_slot].client_socket = client_socket;
    clients[free_client_slot].state = ACTIVE;
    clients[free_client_slot].tmp = 0;
    if (client_socket > nsc)
        nsc = client_socket;
    FD_SET(client_socket, &socket_set);
    printf("%s zarejestrowany\n", request1.sender);
    return 0;
}

void unregister_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state != INACTIVE && clients[i].tmp == 0 && time(NULL) - clients[i].time > 30) {
            clients[i].state = INACTIVE;
            FD_CLR(clients[i].client_socket,&socket_set);
            close(clients[i].client_socket);
        }
    }
}

void send_to(int i, request request1) {
    int client_socket = clients[i].client_socket;
    send(client_socket, (void *) &request1, sizeof(request1), 0);

}

void send_to_all(request request1) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state != INACTIVE && strcmp(request1.sender, clients[i].name) != 0) {
            send_to(i, request1);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Błędne argumenty\n");
        exit(-1);
    }
    atexit(clean);
    signal(SIGINT, handler);
    signal(SIGPIPE,SIG_IGN);
    port_number = strtol(argv[1], NULL, 10);
    path = argv[2];

    unix_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    inet_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (unix_socket == -1 || inet_socket == -1) {
        printf("Error 001\n");
        exit(-1);
    }

    memset(&unix_address, 0, sizeof(unix_address));
    memset(&inet_address, 0, sizeof(inet_address));
    unix_address.sun_family = AF_UNIX;
    strcpy(unix_address.sun_path, path);

    inet_address.sin_family = AF_INET;
    inet_address.sin_port = htons((uint16_t) port_number);
    inet_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(unix_socket, (struct sockaddr *) &unix_address, sizeof(unix_address)) != 0) {
        perror(NULL);
        exit(-1);
    }
    if (bind(inet_socket, (struct sockaddr *) &inet_address, sizeof(inet_address)) != 0) {
        perror(NULL);
        exit(-1);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].state = INACTIVE;
    }

    if (listen(unix_socket, 16) == -1) {
        perror(NULL);
        exit(-1);
    }
    if (listen(inet_socket, 16) == -1) {
        perror(NULL);
        exit(-1);
    }

    message_type message_type1;
    request request1;
    int client_socket;

    fd_set sockset1;
    FD_ZERO(&socket_set);
    FD_SET(unix_socket, &socket_set);
    FD_SET(inet_socket, &socket_set);
    nsc = unix_socket > inet_socket ? unix_socket : inet_socket;
    while (1) {
        sockset1 = socket_set;
        message_type1 = NO_MESSAGE;
        struct timeval tim1;
        tim1.tv_sec = 1;
        tim1.tv_usec = 0;
        if (select(nsc + 1, &sockset1, NULL, NULL, &tim1) > 0) {
            if (FD_ISSET(unix_socket, &sockset1)) {
                client_socket = accept(unix_socket, NULL, NULL);
                if (client_socket != -1) {
                    message_type1 = MESSAGE;
                }
            } else if (FD_ISSET(inet_socket, &sockset1)) {
                client_socket = accept(inet_socket, NULL, NULL);
                if (client_socket != -1) {
                    message_type1 = MESSAGE;
                }
            } else {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (FD_ISSET(clients[i].client_socket, &sockset1)) {
                        client_socket = clients[i].client_socket;
                        message_type1 = MESSAGE;
                        break;
                    }
                }
            }
        }
        if (message_type1 != NO_MESSAGE) {
            ssize_t tmp = recv(client_socket, (void *) &request1, sizeof(request1), 0);
            if (tmp > 0) {
                register_client(request1, client_socket);
                send_to_all(request1);
            } else if (tmp == 0) {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].client_socket == client_socket) {
                        clients[i].state = INACTIVE;
                        break;
                    }
                }
                FD_CLR(client_socket, &socket_set);
                close(client_socket);
            }
        }
        unregister_clients();
    }
}