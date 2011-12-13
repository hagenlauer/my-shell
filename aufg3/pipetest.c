#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct fdpaar{
  int zero;
  int one;
};

int main(int argc, char *argv[]){
   int pipefd[2];
   
   struct fdpaar fds; 
   pid_t cpid;
   char buf;

   if (argc != 2) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
   }

   if (pipe((int *)&fds) == -1) {perror("pipe failed!\n");exit(EXIT_FAILURE);}

   cpid = fork();
   if (cpid == -1) {perror("fork failed!\n");exit(EXIT_FAILURE);}

   if (cpid == 0) {    /* Child reads */
       close(fds.one);          /* unused write */

       while (read(fds.zero, &buf, 1) > 0)
           write(STDOUT_FILENO, &buf, 1);

       write(STDOUT_FILENO, "\n", 1);
       close(fds.zero);
       _exit(EXIT_SUCCESS);

   } else {            /* argv[1] to pipe */
       close(fds.zero);          /* unused read */
       write(fds.one, argv[1], strlen(argv[1]));
       close(fds.one);          /*EOF */
       waitpid(-1, NULL, 0);                
       exit(EXIT_SUCCESS);
   }
}
