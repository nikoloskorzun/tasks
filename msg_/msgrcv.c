#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 1234

struct message {
    long mtype;
    char mtext[1024];
};

int main()
{
    struct message msg;
    int msqid;
    long client_id; // Уникальный идентификатор клиента
    
    // Получение идентификатора очереди сообщений
    msqid = msgget(KEY, 0666);
    if (msqid == -1) {
        perror("Ошибка при получении очереди сообщений");
        exit(1);
    }
    
    // Генерация уникального идентификатора клиента (можно использовать произвольный метод)
    client_id = getpid(); // В данном случае используется PID процесса
    
    // Ввод числа
    while(1){
    printf("Введите число: ");
    fgets(msg.mtext, sizeof(msg.mtext), stdin);
    msg.mtext[strcspn(msg.mtext, "\n")] = 0; // удаление символа новой строки
    
    msg.mtype = client_id; // Используем PID в качестве идентификатора клиента
    
    // Отправка числа серверу
    if (msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("Ошибка при отправке сообщений");
        exit(1);
    }
    
    // Получение результата от сервера
    if (msgrcv(msqid, &msg, sizeof(msg.mtext), client_id + 1, 0) == -1) {
        perror("Ошибка при получении сообщений");
        exit(1);
    }
    
    printf("Получено: %s\n", msg.mtext);
    }
    return 0;
}

