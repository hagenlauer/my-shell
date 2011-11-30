#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
   int pipefd[2];
   pid_t cpid;
   char buf;

   if (argc != 2) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
   }

   if (pipe(pipefd) == -1) {perror("pipe failed!\n");exit(EXIT_FAILURE);}

   cpid = fork();
   if (cpid == -1) {perror("fork failed!\n");exit(EXIT_FAILURE);}

   if (cpid == 0) {    /* Child reads */
       close(pipefd[1]);          /* unused write */

       while (read(pipefd[0], &buf, 1) > 0)
           write(STDOUT_FILENO, &buf, 1);

       write(STDOUT_FILENO, "\n", 1);
       close(pipefd[0]);
       _exit(EXIT_SUCCESS);

   } else {            /* argv[1] to pipe */
       close(pipefd[0]);          /* unused read */
       write(pipefd[1], argv[1], strlen(argv[1]));
       close(pipefd[1]);          /*EOF */
       waitpid(-1, NULL, 0);                
       exit(EXIT_SUCCESS);
   }
}
