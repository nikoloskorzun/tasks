#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#define BUF_SIZE 1024
#define USERS_DIR "users"

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void create_user_dir(const char *username) {
    char user_path[BUF_SIZE];
    sprintf(user_path, "%s/%s", USERS_DIR, username);

    struct stat st = {0};
    if (stat(USERS_DIR, &st) == -1) {
        mkdir(USERS_DIR, 0700);
    }

    if (stat(user_path, &st) == -1) {
        mkdir(user_path, 0700);
    }
}

void send_user_message(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
}

void receive_client_message(int client_socket, char *buffer) {
    ssize_t bytes_received = recv(client_socket, buffer, BUF_SIZE, 0);
    if (bytes_received <= 0) {
        error("Error receiving data from client");
    }
    buffer[bytes_received] = '\0';
}

void handle_client(int client_socket) {
    char buff_time[80];
    char buffer[BUF_SIZE];
    char username[BUF_SIZE];
    char user_path[BUF_SIZE];

    // Получаем имя пользователя
    ssize_t bytes_received = recv(client_socket, username, sizeof(username), 0);
    username[bytes_received] = '\0';

    // Создаем директорию пользователя, если ее нет
    create_user_dir(username);

    // Отправляем приветственное сообщение
    const char *welcome_message = "Welcome to the Mail Server!\r\n";
    send_user_message(client_socket, welcome_message);

    while (1) {



	    
        // Получаем команду от клиента
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            
            break;
        }
        
        buffer[bytes_received] = '\0';
        wait(2);
	printf("Получена команда %s\n", buffer);
        // Обработка команды
        if (strncmp(buffer, "SEND", 4) == 0) {
        
       	char sender[BUF_SIZE], recipient[BUF_SIZE], subject[BUF_SIZE], content[BUF_SIZE];
	    printf("SENDED\n");
	    strcpy(sender, username);
            recv(client_socket, recipient, sizeof(recipient), 0);
            recv(client_socket, subject, sizeof(subject), 0);
            recv(client_socket, content, sizeof(content), 0);
            printf("Получены данные\n");

	    // Создаем файл для письма
	    char mail_path[BUF_SIZE];
	    
	    
	    time_t rawtime;
	    struct tm *timeinfo;

	    // Получение текущего времени
	    time(&rawtime);
	    timeinfo = localtime(&rawtime);

	    // Форматирование времени в строку

	    strftime(buff_time, sizeof(buff_time), "%H:%M", timeinfo);


	    sprintf(mail_path, "%s/%s/%s.txt", USERS_DIR, username, buff_time);
	    FILE *mail_file = fopen(mail_path, "w");
	    fprintf(mail_file, "Sender: %s\nRecipient: %s\nSubject: %s\nContent:\n%s", sender, recipient, subject, content);
	    fclose(mail_file);
	    	    create_user_dir(recipient);
	    sprintf(mail_path, "%s/%s/%s.txt", USERS_DIR, recipient, buff_time);
	    FILE *mail_file1 = fopen(mail_path, "w");
	    fprintf(mail_file1, "Sender: %s\nRecipient: %s\nSubject: %s\nContent:\n%s", sender, 	recipient, subject, content);
	    fclose(mail_file1);

	    send_user_message(client_socket, "Mail sent successfully!\r\n");
	    sleep(1);
        } 
        else if (strncmp(buffer, "LIST", 4) == 0){
        	DIR *dir;
	    struct dirent *ent;
		send_user_message(client_socket, "Yors messages:\n");
	    // Открываем директорию пользователя
	    char user_path[BUF_SIZE];
	    sprintf(user_path, "%s/%s", USERS_DIR, username);
	    if ((dir = opendir(user_path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
		    if (ent->d_type == DT_REG) {
		        send_user_message(client_socket, ent->d_name);
		        send_user_message(client_socket, "\r\n");
		    }
		}

		closedir(dir);
	    } else {
		send_user_message(client_socket, "Error listing messages.\r\n");
	    }
	    sleep(1);
        }
        else if (strncmp(buffer, "DELETE", 6) == 0) {
            // Команда для удаления письма
            char filename[BUF_SIZE];

            // Получаем имя файла для удаления
            recv(client_socket, filename, sizeof(filename), 0);

            // Полный путь к файлу
            sprintf(user_path, "%s/%s/%s", USERS_DIR, username, filename);

            // Пытаемся удалить файл
            if (remove(user_path) == 0) {
                send_user_message(client_socket, "Mail deleted successfully!\r\n");
            } else {
                send_user_message(client_socket, "Error deleting mail.\r\n");
            }
            sleep(1);
         }
        else if (strncmp(buffer, "VIEW", 4) == 0) {
	    // Команда для просмотра содержимого сообщения
	    char filename[BUF_SIZE];

	    // Получаем имя файла для просмотра
	    recv(client_socket, filename, sizeof(filename), 0);

	    // Полный путь к файлу
	    char mail_path[BUF_SIZE];
	    sprintf(mail_path, "%s/%s/%s", USERS_DIR, username, filename);

	    FILE *mail_file = fopen(mail_path, "r");
	    if (mail_file != NULL) {
		char line[BUF_SIZE];
		send_user_message(client_socket, "Message content:\r\n");
		while (fgets(line, sizeof(line), mail_file) != NULL) {
		    send_user_message(client_socket, line);
		}
		fclose(mail_file);
		send_user_message(client_socket, "\n");
		wait(2);
	    } else {
		send_user_message(client_socket, "Error viewing message.\r\n");
	    }
	    sleep(1);
	}else {
            // Неизвестная команда
            send_user_message(client_socket, "Unknown command. Try again.\r\n");
        }
	
    }

    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    // Создаем сокет
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        error("Error opening socket");
    }

    // Задаем параметры сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Привязываем сокет к адресу и порту
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        error("Error binding socket");
    }

    // Переводим сокет в режим прослушивания
    if (listen(server_socket, 10) == -1) {
        error("Error listening on socket");
    }

    printf("Mail server listening on port %d...\n", port);

    while (1) {
        // Принимаем соединение от клиента
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(struct sockaddr_in);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            error("Error accepting connection");
        }

        // Обработка соединения в отдельном процессе или потоке
        handle_client(client_socket);
    }

    close(server_socket);

    return 0;
}
