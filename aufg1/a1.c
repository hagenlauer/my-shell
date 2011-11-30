#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
  //printf("Hello World");
  pid_t kind = fork();
  pid_t current = getpid(); 
  pid_t father = getppid();
  printf("Father: %d Process: %d Child: %d \n", father, current, kind);
}



