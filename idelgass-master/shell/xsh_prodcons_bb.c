#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>
#include<prodcons_bb.h>
#include<run.h>

int arr_q[BUFFSIZE]; //BUFFSIZE in prodcons_bb header
int head = 0, tail = 0;

// Semaphores
// Use to protect accesss to arr_q between producer and consumer
int mutex;
// Count number of full slots in ring buff?
int sem_fullc;
// Count number of empty slots in ring buff?
int sem_emptyc;
// Synch created processes w main process
int sem_main;

shellcmd xsh_prodcons_bb(int nargs, char** args){
	int nprod, ncons; // nprod = no of producer processes, ncons = no of consumer processes
	int prod_valc; // no of values to be produced
	int cons_valc; // no of values to be consumed

        // Guard condition
        if(nargs != 5){
                fprintf(stderr, "Invalid number of arguments. Expected: 4, Given: %d\n", nargs - 1);
                signal(run_cmd);
		return 1;
        }

	// Set cmd line args
	if((nprod = atoi(*(args + 1))) == 0){
		fprintf(stderr, "Invalid input\n");
		signal(run_cmd);
		return 1;
	}
	if((ncons = atoi(*(args + 2))) == 0){   
                fprintf(stderr, "Invalid input\n");     
                signal(run_cmd);
                return 1;
        }
	if((prod_valc = atoi(*(args + 3))) == 0){
                fprintf(stderr, "Invalid input\n");
                signal(run_cmd);
                return 1;
        }
	if((cons_valc = atoi(*(args + 4))) == 0){
                fprintf(stderr, "Invalid input\n");
                signal(run_cmd);
                return 1;
        }

	// Need to check that prod_valc * n = cons_valc * m
	if(prod_valc * nprod != cons_valc * ncons){
		fprintf(stderr, "Producer consumer arg mismatch (all and only produced vales must be consumed)\n");
		signal(run_cmd);
		return 1;
	}


        //Initializing semaphores
	mutex = semcreate(1);
	sem_fullc = semcreate(0);
	sem_emptyc = semcreate(BUFFSIZE);
	sem_main = semcreate(0);

	//Create producer/consumer processes
	for(int i = 0; i < nprod; i++){
        	resume( create(producer_bb, 1000, 20, "producer_bb", 2, i, prod_valc));
	}
	for(int i = 0; i < ncons; i++){
		resume( create(consumer_bb, 1000, 20, "consumer_bb", 2, i, cons_valc));
	}
	
	//TODO need to fix synch here, xsh returns at end of consumer_0, but what about consumer_1
        //Signaled at end of each prod/cons app   
        for(int i = 0; i < nprod + ncons; i++){	
		wait(sem_main);
	}

        //Process has finished, can signal the run cmd wrapper now
        signal(run_cmd);
        return 0;
}

