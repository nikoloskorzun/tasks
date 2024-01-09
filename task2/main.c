#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#include <string.h>


void executer( int *pipe_d)
{
	
	dup2(pipe_d[0], STDIN_FILENO);
	close(pipe_d[0]);

	char s[100]; 
	scanf("%s", s);
	execlp(s, s, NULL);	


//	wait(0);
}

void sender(int *pipe_d)
{

	dup2(pipe_d[1],STDOUT_FILENO);
	close(pipe_d[1]);



	printf("/bin/tree\n");
}




int main(void)
{
	int pipe_fd[2];
	if(pipe(pipe_fd) == -1)
		return -2;
	pid_t pid = fork();
	if(pid < 0)
	{
		return -1;
	}
	if(!pid)
	{
		//child
		sender(pipe_fd);
	}
	else
	{
		//parent
		char s[] = "temp";
		executer(pipe_fd);
	}

	return 0;
}
