#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <string.h>
#include <wait.h>


int main(int argc, char *argv[]){
  //char *program = argv[1]; //programm to be called 
  //printf("%s\n", arg1);
  
  char *env[argc]; // malloc verwenden
  char *arguments[argc]; // malloc verwenden
  
  //printf("program: %s\n", program);
  int i,j,k = 0;
  for(i = 1; i < argc; i++){
    switch(inspect(argv[i])){
      case 0:
        arguments[j] = argv[i];
        printf("arg: %s\n", arguments[j]);
        j++;
        break; 
      case 1:
        env[k] = argv[i];
        printf("env: %s\n", env[k]);
        k++;
        break;
      default:
        puts("invalid argument passed!");
        break;
    }
  }  
  //arguments[argc-1] = "$PATH";
  arguments[j] = NULL; //array ende setzen! muy importante!
  env[k] = NULL;

  pid_t child = fork();

  if(child == 0){ //child will enter this
    if(execve(argv[1], arguments, env) == -1){
      perror("Error in execve\n");
      exit(1);
    }
  }else if(child < 0){
    perror("Error in fork!\n");
    exit(1);
  }else{ //parent
    waitpid(-1, NULL, 0);
    puts("Alles tot, es sind alle tooooot, oh neiiiiiiiiiiin!\n");
  }
}
/*
Reqires: -char *.
         -#include <string.h>
Returns: -Sum of '=' in the specified char * 
*/
int inspect(char *string){
  int i = 0;
  int count = 0;
  while(i < strlen(string)){
    if(string[i]=='='){count++;}
    i++;
  }
  return count; //count = 0 is arg, count = 1 envvar, count > 1 wrong arg
}



