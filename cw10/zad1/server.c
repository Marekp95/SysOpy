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
fd_set socket_set;


struct sockaddr_un unix_address;
struct sockaddr_in inet_address;
struct sockaddr_un client_unix_address;
struct sockaddr_in client_inet_address;

socklen_t unix_address_size = sizeof(struct sockaddr_un);
socklen_t inet_address_size = sizeof(struct sockaddr_in);

//void send_to_all(request request1);

//void send_to(int i, request request1);

//void unregister_clients();

client clients[MAX_CLIENTS];

void handler(int sig) {
    exit(0);
}

void clean() {
    close(inet_socket);
    close(unix_socket);
    remove(path);
}

int register_client(request request1, struct sockaddr *sockaddr1, message_type message_type1) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state != INACTIVE) {
            if (strcmp(clients[i].name, request1.sender) == 0) {
                clients[i].time = time(NULL);
                return 0;
            }
        }
    }
    int free_client_slots;
    for (free_client_slots = 0; free_client_slots < MAX_CLIENTS && clients[free_client_slots].state != INACTIVE; free_client_slots++);
    if (free_client_slots == MAX_CLIENTS)
        return 1;
    strcpy(clients[free_client_slots].name, request1.sender);
    clients[free_client_slots].time = time(NULL);
    if (message_type1 == UNIX) {
        clients[free_client_slots].state = LOCAL;
        clients[free_client_slots].unix_address = *((struct sockaddr_un *) sockaddr1);
    } else if (message_type1 == INET) {
        clients[free_client_slots].state = GLOBAL;
        clients[free_client_slots].inet_address = *((struct sockaddr_in *) sockaddr1);
    }
    printf("%s zarejestrowany\n", request1.sender);
    return 0;
}

void unregister_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state != INACTIVE && time(NULL) - clients[i].time > 30) {
            clients[i].state = INACTIVE;
            printf("Klient %s wyrejestrowany\n", clients[i].name);
        }
    }
}

void send_to(int i, request request1) {
    struct sockaddr * sockaddr1;
    int socket;
    socklen_t address_len;
    if (clients[i].state == LOCAL) {
        sockaddr1 = (struct sockaddr *) &(clients[i].unix_address);
        socket = unix_socket;
        address_len = unix_address_size;
    } else {
        sockaddr1 = (struct sockaddr *) &(clients[i].inet_address);
        socket = inet_socket;
        address_len = inet_address_size;
        char ip[20];
        inet_ntop(AF_INET, &(clients[i].inet_address.sin_addr.s_addr), ip, 20);
    }
    if (sendto(socket, (void *) &request1, sizeof(request1), 0, sockaddr1, address_len) == -1) {
        perror(NULL);
    }
}

void send_to_all(request request1) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].state != INACTIVE && strcmp(request1.sender, clients[i].name) != 0) {
            send_to(i, request1);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Błędne argumenty\n");
        exit(-1);
    }
    atexit(clean);
    signal(SIGINT, handler);
    port_number = strtol(argv[1], NULL, 10);
    path = argv[2];

    unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    inet_socket = socket(AF_INET, SOCK_DGRAM, 0);

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

    message_type message_type1;
    request request1;

    fd_set sockset1;
    FD_ZERO(&socket_set);
    FD_SET(unix_socket, &socket_set);
    FD_SET(inet_socket, &socket_set);
    int flag = 1;
    while (flag) {
        sockset1 = socket_set;
        message_type1 = NO_MESSAGE;
        struct timeval timeval1;
        timeval1.tv_sec = 1;
        timeval1.tv_usec = 0;
        if (select(unix_socket > inet_socket ? unix_socket + 1 : inet_socket + 1, &sockset1, NULL, NULL, &timeval1) > 0) {
            if (FD_ISSET(unix_socket, &sockset1)) {
                ssize_t tmp = recvfrom(unix_socket, &request1, sizeof(request1), MSG_DONTWAIT, (struct sockaddr *) &client_unix_address, &unix_address_size);
                if (tmp > 0) {
                    message_type1 = UNIX;
                } else if (tmp == 0) {
                    printf("Rozłączony\n");
                    exit(0);
                }
            } else if (FD_ISSET(inet_socket, &sockset1)) {
                ssize_t tmp = recvfrom(inet_socket, &request1, sizeof(request1), MSG_DONTWAIT, (struct sockaddr *) &client_inet_address, &inet_address_size);
                if (tmp > 0) {
                    message_type1 = INET;
                } else if (tmp == 0) {
                    printf("Rozłączony\n");
                    exit(0);
                }
            }
            if (message_type1 != NO_MESSAGE) {
                if(message_type1 == UNIX)
                    register_client(request1, (struct sockaddr *) &client_unix_address, message_type1);
                else
                    register_client(request1, (struct sockaddr *) &client_inet_address, message_type1);
                send_to_all(request1);
            }
        }
        unregister_clients();
    }
}
