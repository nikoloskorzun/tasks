#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main() {
    FILE *fp;
    char buffer[1024];
    fp = popen("ls -l", "r"); // открыть процесс для чтения вывода команды ls -l
    if (fp == NULL) {
        printf("Ошибка при выполнении команды\n");
        exit(1);
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL) { // читать вывод команды>
        printf("%s", buffer); // вывести на экран
    }
    pclose(fp); // закрыть процесс
    return 0;
}