
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME "myfifo"

int main()
{
    int fd;
    char buf[100];

    // проверяем права доступа к текущей директории
    if(access(".", W_OK) == -1) {
        perror("access");
        exit(EXIT_FAILURE);
    }

    // создаем FIFO
    if(mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for readers...\n");

    // открываем FIFO для записи
    fd = open(FIFO_NAME, O_WRONLY);

    printf("Got a reader - enter some text\n");

    while(fgets(buf, sizeof(buf), stdin) != NULL) {
        // пишем данные в FIFO
        if(write(fd, buf, sizeof(buf)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    // закрываем дескриптор FIFO
      close(fd);
      unlink(FIFO_NAME);
    // удаляем kanal
    exit(EXIT_SUCCESS);
}


