#ifndef NODE_MANAGER_H
#define NODE_MANAGER_H

typedef struct {
    int port;
    const char *file_part;
    int response;
} NodeArguments;


void *send_parts(void *arg);
void create_nodes();

#endif // NODE_MANAGER_H