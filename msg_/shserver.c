#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEMKEYPATH "/dev/null"  
#define SEMKEYID 3              
#define SHMKEYPATH "/dev/null"  
#define SHMKEYID 3             

#define NUMSEMS 2               
#define SIZEOFSHMSEG 50         

#define NUMMSG 2              

int main(int argc, char *argv[])
{
    int rc, semid, shmid, i;
    key_t semkey, shmkey;
    void *shm_address;
    struct sembuf operations[2];
    struct shmid_ds shmid_struct;
    short  sarray[NUMSEMS];

   
    semkey = ftok(SEMKEYPATH,SEMKEYID);
    if ( semkey == (key_t)-1 )
      {
        printf("main: ftok() for sem failed\n");
        return -1;
      }
    shmkey = ftok(SHMKEYPATH,SHMKEYID);
    if ( shmkey == (key_t)-1 )
      {
        printf("main: ftok() for shm failed\n");
        return -1;
      }

    semid = semget( semkey, NUMSEMS, 0666 | IPC_CREAT | IPC_EXCL );
    if ( semid == -1 )
      {
        printf("main: semget() failed\n");
        return -1;
      }

    sarray[0] = 0;
    sarray[1] = 0;
                                                     
    rc = semctl( semid, 1, SETALL, sarray);
    if(rc == -1)
      {
        printf("main: semctl() initialization failed\n");
        return -1;
      }

   
    shmid = shmget(shmkey, SIZEOFSHMSEG, 0666 | IPC_CREAT | IPC_EXCL);
    if (shmid == -1)
      {
        printf("main: shmget() failed\n");
        return -1;
      }


    shm_address = shmat(shmid, NULL, 0);
    if ( shm_address==NULL )
      {
        printf("main: shmat() failed\n");
        return -1;
      }
    printf("Ready for client jobs\n");

    for (i=0; i < NUMMSG; i++)
      {
       
        operations[0].sem_num = 1;
                                 
        operations[0].sem_op = -1;
                                    
        operations[0].sem_flg = 0;
                               

        operations[1].sem_num = 0;
                                 
        operations[1].sem_op =  1;
                                
        operations[1].sem_flg = IPC_NOWAIT;
                                         

        rc = semop( semid, operations, 2 );
        if (rc == -1)
          {
            printf("main: semop() failed\n");
            return -1;
          }

        /* Print the shared memory contents.                         */
        printf("Server Received : \"%s\"\n", (char *) shm_address);

        /* Signal the first semaphore to free the shared memory.     */
        operations[0].sem_num = 0;
        operations[0].sem_op  = -1;
        operations[0].sem_flg = IPC_NOWAIT;

        rc = semop( semid, operations, 1 );
        if (rc == -1)
          {
            printf("main: semop() failed\n");
            return -1;
        }

      }

    rc = semctl( semid, 1, IPC_RMID );
    if (rc==-1)
      {
        printf("main: semctl() remove id failed\n");
        return -1;
      }
    rc = shmdt(shm_address);
    if (rc==-1)
      {
        printf("main: shmdt() failed\n");
        return -1;
      }
    rc = shmctl(shmid, IPC_RMID, &shmid_struct);
    if (rc==-1)
      {
        printf("main: shmctl() failed\n");
        return -1;
      }
return 0;
}

