#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>

#include "memory.h"

#define BLOCK_SIZE 100

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int main(int argc, char* argv[])
{
    clock_t t;
    t = clock();
    //printf("Timer starts\n");
    char* block = attach_memory_block(FILENAME, BLOCK_SIZE);
    if(block == NULL)
    {
        printf("ERROR: couldn't get block\n");
        return -1;
    }

    sem_t* sem_server = sem_open(SEM_SERVER_FNAME, SEM_PERMS);
    if(sem_server == SEM_FAILED)
    {
        perror("sem_open/client/int_semaphore\n");
        exit(EXIT_FAILURE);
    }

    sem_t* sem_child = sem_open(SEM_CLIENT_FNAME, SEM_PERMS);
    if(sem_child == SEM_FAILED)
    {
        perror("sem_open/client/int_semaphore\n");
        exit(EXIT_FAILURE);
    }
    
    for(int i=0 ; i<atoi(argv[2]) ; ++i)
    {
        long now;
        now=time(NULL);														
        srand((unsigned int)now);

        int x = rand() % atoi(argv[1]);
        x += 1;

        printf("CLIENT -> Asking Server for line: %d\n",x);

        char value[100];

        sprintf(value, "%d", x);

        //sem_wait(sem_cons);
        strncpy(block,value,BLOCK_SIZE);


        sem_post(sem_server);   // we pass the flow to parent process - server
        sem_wait(sem_child);    // we let the child - client wait    

        printf("CLIENT -> Reading shared memory:\n");
        printf("SHARED MEMORY: %s\n", block);
        sleep(1);
    }
    
    detach_memory_block(block);
    
    //printf("Timer ends \n");
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time
    printf("The client-process took %f seconds to execute\n", time_taken);

    return 0;
}