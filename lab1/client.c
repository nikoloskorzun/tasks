#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void receive_server_message(int server_socket) {
    char buffer[BUF_SIZE];
    ssize_t bytes_received = recv(server_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        error("Error receiving data from server");
    }
    buffer[bytes_received] = '\0';
    printf("Server: %s", buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    // Создаем сокет
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        error("Error opening socket");
    }

    // Задаем параметры сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    // Устанавливаем соединение с сервером
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error("Error connecting to server");
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_sent;

    printf("EHLO\n");
    // Вводим имя пользователя
    char username[BUF_SIZE];
    printf("MAIL FROM:: ");
    fgets(username, sizeof(username), stdin);
    username[strlen(username) - 1] = '\0';  // Удаляем символ новой строки

    // Отправляем имя пользователя на сервер
    send(client_socket, username, strlen(username), 0);

    // Получаем приветственное сообщение для пользователя
    receive_server_message(client_socket);
	
    // Основной цикл клиента
    while (1) {
    	
    	//receive_server_message(client_socket);
        // Вводим команду
        printf("Enter a command ('SEND', 'DELETE', 'EXIT'): ");
        fgets(buffer, sizeof(buffer), stdin);
        // Отправляем команду на сервер
        send(client_socket, buffer, strlen(buffer), 0);

        // Обработка команд
        if (strncmp(buffer, "SEND", 4) == 0) {
            // Команда для отправки письма
            char recipient[BUF_SIZE], subject[BUF_SIZE], content[BUF_SIZE];



            printf("Enter RCPT TO: ");
            fgets(recipient, sizeof(recipient), stdin);
            recipient[strlen(recipient) - 1] = '\0';  // Удаляем символ новой строки
            send(client_socket, recipient, strlen(recipient), 0);
	
            printf("Enter subject: ");
            fgets(subject, sizeof(subject), stdin);
            subject[strlen(subject) - 1] = '\0';  // Удаляем символ новой строки
            send(client_socket, subject, strlen(subject), 0);

            printf("Enter content (DATA): ");
            fgets(content, sizeof(content), stdin);
            content[strlen(content) - 1] = '\0';  // Удаляем символ новой строки
            send(client_socket, content, strlen(content), 0);
		printf("Отправка: ");
            // Отправляем данные письма на сервер
            

            // Получаем ответ от сервера
            receive_server_message(client_socket);
        } else if (strncmp(buffer, "DELETE", 6) == 0) {
            // Команда для удаления письма
            char filename[BUF_SIZE];

            // Вводим имя файла для удаления
            printf("Enter filename to delete: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strlen(filename) - 1] = '\0';  // Удаляем символ новой строки

            // Отправляем имя файла на сервер
            send(client_socket, filename, strlen(filename), 0);
            // Получаем ответ от сервера
            receive_server_message(client_socket);
        } else if (strncmp(buffer, "EXIT", 4) == 0) {
            // Команда для завершения сеанса
            break;
        
 	} else if (strncmp(buffer, "VIEW", 4) == 0) {
            // Команда для просмотра содержимого сообщения
	    char filename[BUF_SIZE];

	    // Вводим имя файла для просмотра
	    printf("Enter filename to view: ");
	    fgets(filename, sizeof(filename), stdin);
	    filename[strlen(filename) - 1] = '\0';

	    // Отправляем имя файла на сервер

	    send(client_socket, filename, strlen(filename),0);
	    printf("\n");
	    // Получаем ответ от сервера
	    receive_server_message(client_socket);
	    printf("\n");
        } 
        else {
            // Неизвестная команда
            receive_server_message(client_socket);
        }
    
    }
    // Закрываем соединение
    close(client_socket);

    return 0;
}
