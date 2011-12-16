#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "utils.h"
#include "listen.h"
#include "wortspeicher.h"
#include "kommandos.h"
#include "frontend.h"
#include "parser.h"
#include "proclist.h"
#include "variablen.h"

int interpretiere(Kommando k, int forkexec);
int assemble_pipeline2(Liste l, int descr);
int assemble_pipeline3(Liste l);

Prozess *head; /*wird von shell.c aus initialisiert*/
Prozess *p;

void mach(Prozess *p){
  head = p;
  fprintf(stderr, "%s\n", head->name);
}

void do_execvp(int argc, char **args){
  execvp(*args, args);
  perror("exec-Fehler"); 
  fprintf(stderr, "bei Aufruf von \"%s\"\n", *args);
  exit(1);
}
int assemble_pipeline3(Liste l){ /*iterative loesung*/
  return 0;
}

int assemble_pipeline2(Liste l, int descr){
  Kommando k = (Kommando)listeKopf(l);
  int fds[2];
  pid_t pid;
  if(!listeIstleer(listeRest(l))){ /*if not is end of pipe*/
    pipe(fds);
  }
  pid=fork();
  switch(pid){
    case -1:
      perror("Fehler bei fork @pipe"); 
      return(-1);
    case 0: /* child */
      if(dup2(descr,0) == -1){perror("Error dup2 descr -> 0 failed");fprintf(stderr, "descr: %d\n", fds[1]);exit(1);} /*read from given Descriptor*/
      if(close(descr) == -1){perror("Error close descr");exit(1);}
      if(!listeIstleer(listeRest(l))){ /* !=END else do nothing to stdout*/
        if(dup2(fds[1],1) == -1){perror("Error dup2 fds[1]->1");fprintf(stderr, "descr: %d\n", fds[1]);exit(1);}
        if(close(fds[1]) == -1){perror("Error close fds[1]");exit(1);}
      }
      interpretiere(k, 0); /* exit on status here */
      return 0;
    default:
      if(!listeIstleer(listeRest(l))){
        if(close(fds[1]) == -1){perror("Parent couldn't close the pipe!"); exit(1);} /*close unused descriptor*/
        assemble_pipeline2(listeRest(l),fds[0]); /*return descr of new pipe*/
      } 
      /*Processlisten operationen ADD*/
      waitpid(pid, NULL, 0);
      /*Processlisten operationen STATUS*/
      return 0;
  }
}


int interpretiere_pipeline(Kommando k){
  Liste l = k->u.sequenz.liste;
  assemble_pipeline2(l,0);
  return 0;
}


int umlenkungen(Kommando k){
  /* Umlenkungen bearbeiten */
  Liste ul = k->u.einfach.umlenkungen; 
  Umlenkung *u;
  int fd;
  while(ul){
      u = listeKopf(ul);
      fd = -1; /*default setting it to error*/
      if(u->modus==READ){
        fd = open(u->pfad, O_RDONLY, 0640); /*not tested, is the last arg necessary?*/
      }
      if(u->modus==WRITE){
        fd = open(u->pfad, O_WRONLY | O_CREAT | O_TRUNC, 0640);
      }
      if(u->modus==APPEND){
        fd = open(u->pfad, O_WRONLY | O_CREAT | O_APPEND, 0640);
      }
      if(fd==-1){perror("Could not open file! Does it exist?\n");}
      if(dup2(fd,u->filedeskriptor)==-1){perror("Error in dup2()!\n");};
      u = listeKopf(ul);
      fprintf(stderr, "%d%s %s\n", u->filedeskriptor, u->modus==READ ? "< " : u->modus==WRITE ? "> " : ">> ", u->pfad);
      ul = listeRest(ul);
  }

  if(ul)fputs("umlenkung(Kommando k)!\n", stderr);
  return 0;
}

int aufruf(Kommando k, int forkexec){

  /* Programmaufruf im aktuellen Prozess (forkexec==0)
     oder Subprozess (forkexec==1)
  */
  char  * worte = malloc(100 * sizeof(char)); /* 20 zeichen sollten genügen */
  strcpy(worte, k->u.einfach.worte[0]);
  fprintf(stderr, "asdlos %s\n", worte);
  int chld_state;
     
  if(forkexec==1){

    p = neuerProzess(0, worte);
    append(head,p);
    fputs("prozess angelegt! \n",stderr);
    show(head);
    int pid=fork();

    switch (pid){
    case -1:
      perror("Fehler bei fork"); 
      return(-1);
    case 0:
      if(umlenkungen(k))
	    exit(1);
      do_execvp(k->u.einfach.wortanzahl, k->u.einfach.worte);
      abbruch("interner Fehler 001"); /* sollte nie ausgeführt werden */
      return(-1);
    default:
      p->pid = pid; /*set the aquired pid*/
      show(head);
      if(k->endeabwarten){
        /* So einfach geht das hier nicht! */  
        waitpid(pid, &chld_state, 0);
        fputs("setze status!\n", stderr);
        p->status = 1; /*mark him as zombie*/
        if(chld_state == 256){
          return 1; /*error*/
        }
      }
      return 0;
    }
  }else if (forkexec == 2){ /*testausgabe*/
      /*fputs("lolkaese\n",stderr);*/
      kommandoZeigen(k);
      exit(1);
      return 0;
  }else if(forkexec == 0){

    /* nur exec, kein fork */
    if(umlenkungen(k))
    exit(1);
    do_execvp(k->u.einfach.wortanzahl, k->u.einfach.worte);
    abbruch("interner Fehler 001"); /* sollte nie ausgeführt werden */
    exit(1);
    return 1;
  }
}


int interpretiere_einfach(Kommando k, int forkexec){

  char **worte = k->u.einfach.worte;
  int anzahl=k->u.einfach.wortanzahl;

  if (strcmp(worte[0], "exit")==0) {
    switch(anzahl){
    case 1:
      exit(0);
    case 2:
      exit(atoi(worte[1]));
    default:
      fputs( "Aufruf: exit [ ZAHL ]\n", stderr);
      return -1;
    }
  }

  if (strcmp(worte[0], "status")==0) {
    fputs("STATUS soll ausgegeben werden!\n", stderr);
    show(head);

    return 0;
  }
  if (strcmp(worte[0], "clean")==0) {
    fputs("CLEAN!\n", stderr);
    cleanList(head);

    return 0;
  }
  if (strcmp(worte[0], "cd")==0) {
    switch(anzahl){
    case 1:
      return chdir(getenv("HOME"));
    case 2:
      return chdir(worte[1]);
    default:
      fputs("Aufruf: cd [ PFAD ]\n", stderr);
      return -1;
    }
  }

  return aufruf(k, forkexec);
}

int interpretiere(Kommando k, int forkexec){ /*evtl forkexec manipulieren f. exec ohne fork*/
  int status;

  switch(k->typ){
  case K_LEER: 
    return 0;
  case K_EINFACH:
    return interpretiere_einfach(k, forkexec);
  case K_SEQUENZ:
    {
      Liste l = k->u.sequenz.liste;
      while(!listeIstleer(l)){
	       status=interpretiere ((Kommando)listeKopf(l), forkexec);
	       l=listeRest(l);
      }
    }
    return status;
  case K_PIPE:
    {
      status=interpretiere_pipeline(k);
    }
    return status;
  case K_UND:
    { 
      Liste l = k->u.sequenz.liste;
      while(!listeIstleer(l) && status == 0){
         status=interpretiere ((Kommando)listeKopf(l), forkexec);
         l=listeRest(l);
      }
      return status;
    }
  case K_ODER:
    {
      Liste l = k->u.sequenz.liste;
      while(!listeIstleer(l)){
         status=interpretiere ((Kommando)listeKopf(l), forkexec);
         l=listeRest(l);
         if(status == 0){
           /*manuelles break*/
           return status;
         }
      }
      return status;
    }
  default:
    fputs("unbekannter Kommandotyp\n", stderr);
    break;
  }
  return 0;
}

