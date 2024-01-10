/* Global variable for producer consumer */
extern int n; /* this is just declaration */

//Semaphores for synch
extern int  prod, cons, main;

/* Function Prototype */
void consumer(int count);
void producer(int count);
