typedef struct prozess {
	int pid, status;
	struct prozess *next;
}*Prozess;



EXTERNCPP Prozess neueProcListe(void);
void removeProzess(Prozess head, Prozess pro);
