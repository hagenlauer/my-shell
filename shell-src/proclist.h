typedef struct pl{
	int pid;
	int status;
	pl *next;
} *Proclist;


extern Proclist ProcListNeuLeer(void);
extern void elementEinfuegen(Proclist pl, int pid, int status);
extern void	elementEntfernen(Proclist pl, int pid);
extern void printList(Proclist pl);

