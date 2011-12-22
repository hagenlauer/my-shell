/*
  Shell-Beispielimplementierung

  Die Implementierung verzichtet aus Gründen der Einfachheit
  des Parsers auf eine vernünftige Heap-Speicherverwaltung:

  Falls der Parser die Analyse eines Kommandos wegen eines
  Syntaxfehlers abbricht, werden die ggf. vorhandenen
  Syntaxbäume für erfolgreich analysierte Unterstrukturen des
  fehlerhaften Kommandos nicht gelöscht.

  Beispiel: if test ! -d /tmp; then mkdir /tmp; else echo "/tmp vorhanden" 

  Die Analyse wird mit Fehlermeldung abgebrochen, weil vor dem "fi" das 
  obligatorische Semikolon fehlt. Im Heap stehen zu diesem Zeitpunkt die
  Bäume für das test- und das mkdir-Kommando. Diese verbleiben als Müll
  im Heap, da die Verweise ausschließlich auf dem Parser-Stack stehen,
  der im Fehlerfall nicht mehr ohne weiteres zugänglich ist.

  Um dies zu beheben, müsste man 
  a) sich beim Parsen die Zeiger auf die Wurzeln aller
     konstruierten Substruktur-Bäume solange in einer globalen Liste merken, 
     bis die Analyse der kompletten Struktur ERFOLGREICH beendet ist
  oder
  b) die Grammatik mit Fehlerregeln anreichern, in denen die Freigabe
     im Fehlerfall explizit vorgenommen wird.

  Da beides die Grammatik aber stark aufbläht, wird darauf verzichtet.
  Genau.
*/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "utils.h"
#include "listen.h"
#include "wortspeicher.h"
#include "kommandos.h"
#include "frontend.h"
#include "parser.h"
#include "proclist.h"
#include "variablen.h"



extern int yydebug;
extern int yyparse(void);
extern int interpretiere(Kommando k, int forkexec);
extern Prozess *neueProcListe();
extern void mach(Prozess *p);


Prozess *headshell;



void signal_handler0(int signum){
  pid_t pid;
  int chld_state;
  fputs("sigcall\n", stderr);
  pid = waitpid(-1, &chld_state, WNOHANG);
  fprintf(stderr, "Sighandler: %d\n", pid);
  if(pid > 0){
    /*fprintf(stderr,"Caught signal %d from %d and state %d\n",signum, pid, chld_state);*/
    Prozess *p = lookup(headshell, pid);
    if(p == NULL){
      headshell->next->status = -1;
    }else{
      if(chld_state == 256){
        p->status = -2; /*exit(error)*/
      }else{
        p->status = -1; /*mark him as zombie*/
      }
    }
  }else{
    /*fputs("parent got the signal!\n", stderr);*/
  }
}
void signal_handler1(int signum){
  fputs("sigcall\n", stderr);
}

void endesubprozess (int sig){
  /*printf("Sighandler\n");*/
  pid_t pid;
  int chld_state;
  pid = waitpid(-1, &chld_state, WNOHANG);
  /*fprintf(stderr, "Sighandler: %d , %d , %d\n", pid, WEXITSTATUS(chld_state), WTERMSIG(chld_state));*/
  if(pid > 0){
    /*fprintf(stderr,"Caught signal %d from %d and state %d\n",signum, pid, chld_state);*/
    Prozess *p = lookup(headshell, pid);
    if(p == NULL){
      /*headshell->next->pid = pid;*/
      /*fputs("in headshell\n", stderr);*/
      headshell->next->status = WEXITSTATUS(chld_state) + WTERMSIG(chld_state);
    }else{
      p->status = WEXITSTATUS(chld_state) + WTERMSIG(chld_state); /*exit(error)*/
    }
  }else{
    /*fputs("parent got the signal!\n", stderr);*/
  }
}

void init_signalbehandlung(){
  /*signal(SIGCHLD, signal_handler1);*/
  struct sigaction action;
  action.sa_handler = endesubprozess;
  action.sa_flags = SA_RESTART | SA_NODEFER;
  action.sa_flags &= ~SA_RESETHAND;
  if(sigaction(SIGCHLD,&action,NULL)==-1){perror("Error in Sigaction");exit(1);}
}


int main(int argc, char *argv[]){
  int  zeigen=0, ausfuehren=1;
  int status, i;

  /*Prozessliste initialisieren*/
  headshell = neueProcListe();
  mach(headshell);
  
  /*fprintf(stderr, "%s\n", headshell->name);*/
  
  init_signalbehandlung();

  yydebug=0;

  for(i=1; i<argc; i++){
    if (!strcmp(argv[i],"--zeige"))
      zeigen=1;
    else if  (!strcmp(argv[i],"--noexec"))
      ausfuehren=0;
    else if  (!strcmp(argv[i],"--yydebug"))
      yydebug=1;
    else {
      fprintf(stderr, "Aufruf: %s [--zeige] [--noexec] [--yydebug]\n", argv[0]);
      exit(1);
    }
  }
  wsp=wortspeicherNeu();
  while(1){
    int res;
    fputs("github.com/Angusrocks>> ", stdout);
    fflush(stdout);
    res=yyparse();
    if(res==0){
      if(zeigen) 
        kommandoZeigen(k);
      if(ausfuehren) 
        status=interpretiere(k, 1);
      if(zeigen) 
        fprintf(stderr, "Status: %d\n", status);
      kommandoLoeschen(k);
    }
    else
      fputs("Fehlerhaftes Kommando shell\n", stderr);
    wortspeicherLeeren(wsp);
  }
}

