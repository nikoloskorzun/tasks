#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "server.h"

#define SHM_SIZE 100

int main() 
{
    shared_mem mem = {0}; 
    init_shared(&mem, "proto.h", SHM_SIZE); //создание сем и разд

     //отобразить разделяемую память в адресное пространство текущего процесса
    char *shm_data = shmat(mem.shm_id, NULL, 0);  //явное преобразование указателя на целое
    if (shm_data == -1) {
        perror("shmat");
        exit(1);
    }

    printf("Введите символ, который хотите записать\n");
    char lit; //1 byte
    scanf("%c", &lit);

    printf("Введите количество символов, которые хотите ввести\n");
    int amount;
    scanf("%d", &amount);

   
    dostup_cl(&mem); //ожидание положительного значения, чтобы клиент мог писать
    int sem1 = semctl(mem.sem_id, 1, GETVAL); //получение значения кол-ва свободных ячеек
    int sem2 = semctl(mem.sem_id, 2, GETVAL);//получение значения кол-ва занятых

    //проверка- хватает ли места для записи
    amount = (amount <= sem1) ? amount : sem2;
    //заполнение
    for(int i = 0; i< amount; i++)
    {
        shm_data[i+sem2] = lit; 
    }

    //обновление данных в семафоре
    struct sembuf zanyat = {2, amount, 0}; //занятых
    struct sembuf svobod = {1, -amount, 0}; //свободных
    struct sembuf dostup = {0,1, 0}; //доступ
    struct sembuf sops[3] = {zanyat, svobod, dostup}; //массив операций
    int err = semop(mem.sem_id, sops, 3); //выполннеие
    if (err == -1)
    {
        perror("semop upd");
        exit(-1);
    }

    //удаление отображения
    shmdt(&mem);

    return 0;
}
