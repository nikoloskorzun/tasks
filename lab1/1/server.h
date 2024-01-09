#include "proto.h"

void server_read(shared_mem* mem, int size_of_shm)
{
    struct sembuf server_wait = {2, -size_of_shm, 0}; //сем с свободными
    struct sembuf sops[2] = {server_wait, sem_request};  //массив операций
    int err = semop(mem->sem_id, sops, 2);

    if (err == -1)
    {
        perror("read request");
        kill_all(mem);
        exit(-1);
    }
}

void first_setup(shared_mem* mem, int size_of_shm)
{
    struct sembuf init[2] = { {0, 1, 0},
                              {1, size_of_shm, 0}
                            };
    int err = semop(mem->sem_id, init, 2);
    if (err == -1)
    {
        perror("Semaphores init!!!");
        kill_all(mem);
        exit(-1);
    }
}

