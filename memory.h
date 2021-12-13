#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>

char* attach_memory_block(char* filename, int size);
bool detach_memory_block(char* block);
bool destroy_memory_block(char* filename);

#define BLOCK_SIZE 100
#define FILENAME "client.c"
#define IPC_RESULT_ERROR (-1)

#define SEM_SERVER_FNAME "/myserver"
#define SEM_CLIENT_FNAME "/myclient"
#define SEM_READ_FNAME "/myreader"

#endif