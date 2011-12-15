/* 
  proclist.h - definiert den Datentyp "Prozess" zur internen 
                Repräsentation eines Prozesses
*/

typedef struct prozess {
	int pid, status;
	char * name;
	struct prozess *next;
}Prozess;

Prozess* neueProcListe(void);
void show(Prozess *head);
