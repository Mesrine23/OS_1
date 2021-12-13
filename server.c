#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h> 
#include <sys/wait.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <semaphore.h>
#include <pthread.h>

#include "memory.h"

#define BLOCK_SIZE 100

#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

int main(int argc, char *argv[])
{

    if(strcmp(argv[1],"./client")!=0)
    {
        printf("\nWrong program input!\nMaybe try './' before executable.\nex: ./client\n\n");
        return -1;
    }
    else
    {
        printf("\nChild Process Program will be: %s\n",argv[1]);
    }

    printf("Number of children-clients created: %d\n",atoi(argv[2]));
    printf("Number of requests for each child-client: %d\n",atoi(argv[3]));

    printf("\n~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    sleep(1);    

    char* block = attach_memory_block(FILENAME,BLOCK_SIZE);
    if(block == NULL)
    {
        printf("ERROR: couldn't get block\n");
        return -1;
    }

    FILE *file;

    sem_unlink(SEM_SERVER_FNAME);
    sem_unlink(SEM_CLIENT_FNAME);
    sem_unlink(SEM_READ_FNAME);

    sem_t* sem_server = sem_open(SEM_SERVER_FNAME, O_CREAT | O_EXCL, SEM_PERMS, 0);
    if(sem_server == SEM_FAILED)
    {
        perror("SERVER:sem_open/server/server_semaphore\n");
        exit(EXIT_FAILURE);
    }

    sem_t* sem_child = sem_open(SEM_CLIENT_FNAME, O_CREAT | O_EXCL, SEM_PERMS, 0);
    if(sem_child == SEM_FAILED)
    {
        perror("SERVER:sem_open/child/child_semaphore\n");
        exit(EXIT_FAILURE);
    }

    /*sem_t* sem_reader = sem_open(SEM_READ_FNAME, O_CREAT | O_EXCL, SEM_PERMS, 0);
    if(sem_reader == SEM_FAILED)
    {
        perror("SERVER:sem_open/reader/reader_semaphore\n");
        exit(EXIT_FAILURE);
    }*/



    int status;
    int K = atoi(argv[2]);  // number of children
    int N = atoi(argv[3]);  // number of requests
     
    for (int i=0 ; i<K ; ++i)
    {
        int f;
        if((f = fork()) < 0)
        {
            perror("fork");
            abort();
        }
        else if(f==0)
        {
            //printf("Client Number: %d\n\n",i);
            char argInt[10];
            sprintf(argInt, "%d", i);
            char *arg_list[] = {argv[1], "50", argv[3], argInt, NULL};
            execvp(argv[1],arg_list);
            exit(0);
        }
    }

    
    for(int i=0 ; i<K*N ; ++i)
    {
        sem_post(sem_child);
        sem_wait(sem_server);
        int line;
        if(strlen(block)>0)
        {
            line = atoi(block);
            printf("SERVER -> Delivering line: %d\n", line);
        }
        file = fopen("text.txt","r");
        if(!file)
        {
            printf("ERROR: Can't open the .txt file.\n");
            return -1;
        }
        char buff[1000];
        int count=0;
        while (fgets(buff,1000, file)!=NULL)
        {   
            if(++count == line)
            {
                if(strlen(buff)>99)
                {
                    buff[98] = '\0';
                    buff[99] = '\n';
                }
                strncpy(block,buff,BLOCK_SIZE);
                //printf("Server-> Reading shared memory:\n");
                //printf("SHARED MEMORY: \"%s\"\n", block);
                break;
            }

        }
        //printf("\nSLE SLE SLE SLE\n");
        sem_t* sem_reader = sem_open(SEM_READ_FNAME, SEM_PERMS);
        if(sem_reader == SEM_FAILED)
        {
            perror("SERVER:sem_open/reader/reader_semaphore\n");
            exit(EXIT_FAILURE);
        }
        sem_post(sem_reader);
        //sem_unlink(SEM_READ_FNAME);
        sem_wait(sem_server);
    }

    for (int i=0 ; i<K ; ++i)
    {
        int ch = wait(&status);
        printf("Child->%d, ended successfully!\n\n",ch);
        //printf("\n~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    }

    //sem_close(sem_reader);
    sem_close(sem_server);
    sem_close(sem_child);
    detach_memory_block(block);

    return 0;
}