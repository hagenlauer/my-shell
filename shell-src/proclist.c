#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include "utils.h"
#include "proclist.h"



Prozess *neueProcListe(){
	Prozess *neu = malloc(sizeof (struct prozess));
	neu->pid = getpid();
	neu->status = -1;
	neu->next = NULL;
	neu->name = "shell";
	return neu;
}


Prozess *neuerProzess(int pid, char * string){
	Prozess *neu = malloc(sizeof (struct prozess));
	neu->pid = pid;
	neu->status = -1;
	neu->next = NULL;
	neu->name = string;
	return neu;
}

void append(Prozess *head, Prozess *p){
	p->next = head->next; 
	head->next = p;
	
}

Prozess *lookup(Prozess *head, int pid){
	Prozess *p = head;
	while(p->next != NULL){
		if(p->pid == pid){
			return p;
		}
		p = p->next;
	}
	return p;
}

Prozess *next(Prozess *p){
	return p->next;
}

void show(Prozess *head){
	Prozess *p = head;
	fputs("PID      STATUS       KOMMANDO\n", stderr);
	while(p != NULL){
		switch (p->status){
			case -1:
				printf("%d \t running      %s\n", p->pid, p->name);
				break;
			case 0:
				printf("%d \t exit(0)      %s\n", p->pid, p->name);
				break;
			case 1:
				printf("%d \t exit(1)      %s\n", p->pid, p->name);
				break;
			case 2:
				printf("%d \t exit(2)      %s\n", p->pid, p->name);
				break;
			case 3:
				printf("%d \t exit(3)      %s\n", p->pid, p->name);
				break;	
			default:
				printf("%d \t signal(%02d)   %s\n", p->pid, p->status,p->name);
				break;
		}
		
		p = p->next;
	}
}

void show2(Prozess p){
}

void removeProzess(Prozess *head, Prozess *pro){
	Prozess *p = head;
	while(p->next != pro){
		p = p->next;
	}
	p->next = p->next->next;
}
void cleanList(Prozess *head){
	Prozess *p = head;
	while(p != NULL){
		if(p->next != NULL && p->next->status != -1){ /*0 is running, 1 natural kill, 2 is killed by sig*/
			p->next = p->next->next;
		}else{
			p = p->next;
		}
	}
}


/*
int main(){

	Prozess *procliste = neueProcListe();
	//printf("%d %d %p\n", head->pid, head->status, head->next);
	append(procliste, neuerProzess(1, "xterm"));
	//printf("%d %d %p\n", head->pid, head->status, head->next);
	append(procliste, neuerProzess(2, "ls"));
	cleanList(procliste);
	show(procliste);
	Prozess *p = lookup(procliste,2);
	p->status = 0;
	p = lookup(procliste, 1);
	p->status = 0;
	show(procliste);
	cleanList(procliste);
	show(procliste);
	
	return 0;

}
*/