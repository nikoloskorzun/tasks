#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


typedef struct {
    int sem_id;
    int shm_id;
} shared_mem;

struct sembuf sem_request = {0, -1, 0};
//struct sembuf sem_off = {0, 1, 0};

void dostup_cl(shared_mem* mem)
{
    int err = semop(mem->sem_id, &sem_request, 1);
    if (err == -1)
    {
        perror("dostup");
        exit(-1);
    }
}


int init_shared(shared_mem* mem, char* file, int size_of_shm)
{
    key_t sem_tok = ftok(file, 's');
    if (sem_tok == -1) return sem_tok;
    
    key_t shm_tok = ftok(file, 'm');
    if (shm_tok == -1) return shm_tok;

    mem->sem_id = semget(sem_tok, 3, IPC_CREAT | 0666);
    if (mem->sem_id == -1) return mem->sem_id;
    mem->shm_id = shmget(shm_tok, size_of_shm, IPC_CREAT | 0666);
    if (mem->shm_id == -1) return mem->shm_id;
    
    return 0;
}

void kill_all(shared_mem* mem)
{
    semctl(mem->sem_id, 3, IPC_RMID);
    shmctl(mem->shm_id, IPC_RMID, 0);
}