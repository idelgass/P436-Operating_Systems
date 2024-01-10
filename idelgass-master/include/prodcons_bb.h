//Ring buffer for prodcons_bb
extern int arr_q[];
extern int head;
extern int tail;
#define BUFFSIZE 5

//Semaphores for synch
extern int mutex, sem_fullc, sem_emptyc, sem_main;

/* Function Prototype */
void producer_bb(int prod_id, int prod_valc);
void consumer_bb(int cons_id, int cons_valc);
