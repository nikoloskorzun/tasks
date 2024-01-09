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
    if(access(".", R_OK) == -1) {
        perror("access");
        exit(EXIT_FAILURE);
    }

    // открываем FIFO для чтения
    fd = open(FIFO_NAME, O_RDONLY);

    printf("Waiting for data...\n");

    while(read(fd, buf, sizeof(buf)) > 0) {
        // выводим данные на экран
        printf("%s", buf);
    }

    // закрываем дескриптор FIFO
    close(fd);

    exit(EXIT_SUCCESS);
}