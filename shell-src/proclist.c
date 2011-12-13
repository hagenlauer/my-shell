#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "proclist.h"

/*
typedef struct prozess {
	int pid, status;
	struct prozess *next;
}*Prozess;
*/


Prozess neueProcListe(){
	Prozess neu = malloc(sizeof (struct prozess));
	neu->pid = -1;
	neu->status = -1;
	neu->next = NULL;
	return neu;
}

Prozess neuerProzess(int pid){
	Prozess neu = malloc(sizeof (struct prozess));
	neu->pid = pid;
	neu->status = 0;
	neu->next = NULL;
	return neu;
}

void append(Prozess head, Prozess p){
	p->next = head->next; /**/
	head->next = p;
}

Prozess lookup(Prozess head, int pid){
	Prozess p = head;
	while(p->next != NULL){
		if(p->pid == pid){
			return p;
		}
		p = p->next;
	}
	return p;
}

void show(Prozess head){
	Prozess p = head;
	while(p->next != NULL){
		printf("%d %d\n", p->pid, p->status);
		p = p->next;
	}
	printf("%d %d\n", p->pid, p->status);
}

void removeProzess(Prozess head, Prozess pro){
	Prozess p = head;
	while(p->next != pro){
		p = p->next;
	}
	
	p->next = p->next->next;

}
void cleanList(Prozess head){
	Prozess p = head;
	do{
		if(p->next->status==0){
			p->next= p->next->next;
		}
		p = p->next;
	}while(p->next != NULL);
}

int main(){

	Prozess head = neueProcListe();
	//printf("%d %d %p\n", head->pid, head->status, head->next);
	append(head, neuerProzess(1));
	//printf("%d %d %p\n", head->pid, head->status, head->next);
	append(head, neuerProzess(2));
	show(head);
	cleanList(head);
	show(head);
	//Prozess head = neueProcListe();
	return 0;
}