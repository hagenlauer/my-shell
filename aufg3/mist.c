#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>


int main(int argc, char *argv[]){

	if (argc <= 2) {perror("insufficent args given, are you dumb?\n"); exit(-1);}

	int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0640); /* neuer fd hier, auch fehlerbehandeln!*/
	//write(2, "new fd: %d\n", fd);
	int newfd;

	pid_t pid;

	switch(pid = fork()){
		case -1:
			perror("fork klappt nicht\n");
			exit(1);
		case 0: /*child*/
			
			newfd = dup2(fd, 1); /*add error handling here! SOON.*/
			printf("dup worked quite well!\n");
			
			if(execv(argv[2], argv+2)){
  				perror("An Error occured!");
  	  			exit(1);
    		}
			//close(newfd); /*is never reached!*/ /*SAME AS ABOVE!*/
			break;
		default: /*parent*/
			pid = waitpid(-1, NULL, 0);
			printf("pidfin: %d\n", pid);
			break;
	}
	if(close(fd) < 0){perror("could not close!\n");exit(1);} /*Thats the way to do it.*/
	//write(2, "successfully closed descriptor %d!\n",fd);
}
