#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[]){
	if (argc <= 1) {perror("no args given!\n"); exit(-1);}
	//char *out = (char *)malloc(1000 * sizeof(char));
	//printf("%s\n", argv[1]);
	if (argc > 2){printf("%s\n", argv[2]);}else{perror("No text given!\n");exit(-1);}

	//write
	int fd=open(argv[1], O_RDWR | O_CREAT, 0755); // mode ist eigentlich soderbit | userbit | groupbit | otherbit
	int written = write(fd, argv[2], strlen(argv[2]));
	
	//read
	int fd2 = open(argv[1], O_RDONLY);
	int re = read(fd2, 0, written); //bei unbekannter länge do while schleife byteweise einlesen
	
	if((close(fd) | close (fd2))!= 0){perror("Error in close!\n"); exit(-1);}

}


