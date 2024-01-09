#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define KEY 1234

struct message {
    long mtype;
    char mtext[1024];
};

int msqid;

void cleanup()
{
    // Удаление очереди сообщений
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("Ошибка при удалении очереди сообщений");
        exit(1);
    }
    
    printf("Очередь сообщений удалена\n");
    
    // Вывод информации об очереди сообщений из ядра
    system("ipcs -q");
    
    exit(0);
}

int main()
{
    struct message msg;
    
    // Создание или получение идентификатора очереди сообщений
    msqid = msgget(KEY, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("Ошибка при создании/получении очереди сообщений");
        exit(1);
    }
    
    signal(SIGINT, cleanup);
    
    printf("Очередь сообщений создана / получена с ключом %d\n", KEY);
    
    pid_t server_pid = getpid();
    
    while (1) {
        // Получение сообщений от клиента
        int result_m = msgrcv(msqid, &msg, sizeof(msg.mtext), -30000, 0);
        if (result_m == -1) {
            perror("Ошибка при чтении сообщений");
            exit(1);
        }
      
        printf("Получено сообщение от клиента с ID %ld: %s\n", msg.mtype, msg.mtext);
        
        int num = atoi(msg.mtext);
        int result = num * 2; // пример операции над числом
        
        // Отправка результата клиенту
        sprintf(msg.mtext, "%d", result);
        msg.mtype = msg.mtype + 1+30000;
        if (msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("Ошибка при отправке сообщений");
            exit(1);
        }
        
        printf("Результат отправлен клиенту\n");
    }
    
    return 0;
}

