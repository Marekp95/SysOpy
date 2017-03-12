#ifndef MAIN_H
#define MAIN_H

#define MAX_CLIENTS 100
#define MAX_MESSAGE_LENGTH 256
#define MAX_CLIENT_NAME_LENGTH 32
#define MAX_RESPONSE_TIME 5
#define MAX_PATH_LENGTH 50

typedef enum {
    NO_MESSAGE,
    MESSAGE
} message_type;

typedef enum {
    INACTIVE = 0,
    ACTIVE
} client_state;

typedef struct {
    char sender[MAX_CLIENT_NAME_LENGTH];
    char message[MAX_MESSAGE_LENGTH];
} request;

typedef struct {
    char name[MAX_CLIENT_NAME_LENGTH];
    client_state state;
    clock_t time;
    int client_socket;
    int tmp;
} client;

#endif