typedef struct pl{
	void* kopf; /*head ist der anfang, root was auch immer*/
	void* process; /*process zeugs*/
	struct pl *next;
	struct pl *previous;
} *Proclist;


extern Proclist  ProcListNeuLeer(void);
extern void elementEinfuegen(Proclist pl, void* elem);
extern void	elementEntfernen(Proclist pl, void* elem);
extern void printList(Proclist pl);