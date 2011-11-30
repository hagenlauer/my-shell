#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdlib.h>



int main(int argc, char *argv[]){

  int i;
  //char *arg1 = argv[1]; //programm to be called
  //printf("%s\n", arg1);
  char *arguments[argc-1]; //programm parameters

  for(i = 1; i < argc; i++){
    arguments[i-1]=argv[i];
    //printf("arg %d: %s\n", i, arguments[i-2]);
  }
  //arguments[argc-1] = "$PATH";
  arguments[argc-1] = NULL;
  

  //printf("%s\n",arg1);
  for (i = 0; i < argc; ++i)
  {
    //printf("arg %d: %s\n", i, arguments[i]);
  }

  printf("Initial Process: %d \n", getpid());
  //pid_t root = getpid(); //root ist festgeklopft
  //int safeinit = 2;
  
  pid_t kind1 = fork();
  pid_t kind2;
  if(kind1 == 0){ //child
  	printf("child1 %d\n", getpid());

  	
  	if(execlp("xterm", "xterm", "-bg", "red", NULL) == -1){
  		perror("An Error occured!");
  	  exit(-1);
    }


  }else if (kind1 < 0){ //Err in fork
  	perror("error in fork1!");
    exit(-1);
  }else{ //parent       
	   //printf("parent %d\n", getpid());
	   kind2 = fork();
	   if(kind2 == 0){ //child
		  printf("child2 %d\n", getpid());

		 if(execvp(argv[1], argv+2) == -1){ //mit den argv zugriffen performanter und auch sicher mit 0 terminiert!
  			perror("An Error occured!");
        exit(-1);
  	 }
	   }else if(kind2<0){
		   perror("error in fork2!");
	   }else{ //parent
     
     // evtl. place wait here

		 //printf("parent %d\n", getpid());
	   }      
  }
  if(kind1 && kind2 != 0){ //only childs have a 0 value in their variables 
    
    int deadchild = waitpid(-1, NULL, 0); //alle kinder tot
    
    if(deadchild == kind1){
      puts("mein rotes kind ist tot!\n");
      waitpid(-1, NULL, 0);
      puts("mein gruenes kind ist tot!\n");
    }else{
      puts("mein gruenes kind ist tot!\n");
      waitpid(-1, NULL, 0);
      puts("mein rotes kind ist tot!\n");
    }
                // evtl. 2 x waitpid() mit -1 für any child? wär cool wenn waitpid den gerade gestorbenen Prozess zurückliefert (pid)

  }
}



