#include <xinu.h>
#include <prodcons_bb.h>
#include <stdio.h>

void producer_bb(int prod_id, int prod_valc) {
	for(int i = 0; i < prod_valc; i++){ 
		wait(sem_emptyc);// Decrease number of empty slots by 1, if no empty slots (emptyc = 0) then wait
		wait(mutex); // Restrict shared access to buffer

                //Critical
   		//ring buff
		arr_q[tail++] = i;
	       	tail %= BUFFSIZE;	
                fprintf(stdout, "name : producer_%d, write : %d\n", prod_id, i);
 
		signal(mutex);
		signal(sem_fullc);// Increase number of full slots by 1
        }
	signal(sem_main);
}

