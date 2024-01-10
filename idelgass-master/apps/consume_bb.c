#include <xinu.h>
#include <prodcons_bb.h>
#include <stdio.h>

int n;
void consumer_bb(int cons_id, int cons_valc){
        for(int i = 0; i < cons_valc; i++){ 
                wait(sem_fullc);// Decrease number of full slots by 1, if no full slots (fullc = 0) then wait
		wait(mutex); // Restrict shared access to buffer

                //Critical
                // ring buff
		n = arr_q[head++];
		head %= BUFFSIZE;
                fprintf(stdout, "name : consumer_%d, read : %d\n", cons_id, n);
		
                signal(mutex);
		signal(sem_emptyc); // Increase number of empty slots by 1
        }
	signal(sem_main);
}

