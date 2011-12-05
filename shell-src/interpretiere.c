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
#include "variablen.h"

int interpretiere(Kommando k, int forkexec);

void do_execvp(int argc, char **args){
  execvp(*args, args);
  perror("exec-Fehler"); 
  fprintf(stderr, "bei Aufruf von \"%s\"\n", *args);
  exit(1);
}

int interpretiere_pipeline(Kommando k){
  /* NOT IMPLEMENTED */
  fputs("pipe erkannt!\n", stderr);
  
}

int umlenkungen(Kommando k){
  /* Umlenkungen bearbeiten */
  Liste ul = k->u.einfach.umlenkungen; //das klappt schonmal, jetzt hier die ganze open scheisse einbauen
  Umlenkung *u;
  while(ul){
      u = listeKopf(ul);
      fprintf(stderr, "%d%s %s\n", u->filedeskriptor, u->modus==READ ? "< " : u->modus==WRITE ? "> " : ">> ", u->pfad);
      ul = listeRest(ul);
  }

  fputs("umlenkung(Kommando k)!\n", stderr);
  return 0;
}

int aufruf(Kommando k, int forkexec){

  /* Programmaufruf im aktuellen Prozess (forkexec==0)
     oder Subprozess (forkexec==1)
  */
     
  if(forkexec){
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
    default:
      if(k->endeabwarten)
        /* So einfach geht das hier nicht! */ /*hier müssen tabellen einträge gemacht werden*/
	      waitpid(pid, NULL, 0);
      return 0;
    }
  }

  /* nur exec, kein fork */
  if(umlenkungen(k))
    exit(1);
  do_execvp(k->u.einfach.wortanzahl, k->u.einfach.worte);
  abbruch("interner Fehler 001"); /* sollte nie ausgeführt werden */
  exit(1);
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

int interpretiere(Kommando k, int forkexec){
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
      fputs("ohooo eine pipe!\n", stderr);
      status=interpretiere_pipeline(k);
    }
    return status;
  case K_UND:
    {
      fputs("bedingte ausfuehrung UND!\n", stderr); 
      Liste l = k->u.sequenz.liste;
      while(!listeIstleer(l) && status == 0){
         status=interpretiere ((Kommando)listeKopf(l), forkexec);
         l=listeRest(l);
      }
      return status;
    }
  case K_ODER:
    {
      fputs("bedingte ausfuehrung ODER!\n", stderr); 
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
    fputs("unbekannter Kommandotyp, Bearbeitung nicht implementiert\n", stderr);
    break;
  }
  return 0;
}

