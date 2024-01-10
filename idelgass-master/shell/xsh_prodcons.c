#include<xinu.h>
#include<stdio.h>
#include<stdlib.h>
#include<prodcons.h>
#include<run.h>

int n;
//Semaphores
int prod, cons, main;

shellcmd xsh_prodcons(int nargs, char** args){
	int count;
	
	// If parameter is passed, use to initialize count
	if(nargs == 2){ 
		count = atoi(*(args + 1));
		// Making sure input is valid
		if(count < 0){
			fprintf(stderr, "Invalid value passed from cmd line: %d\n", count);
		       	signal(run_cmd);
			return 1;
		}	
	}
	// Otherwise initialize to default value of 200
	else if(nargs == 1) count = 200;
	// Guard condition for nargs
	else {
		fprintf(stderr, "Incorrect number of args. Expected: 1 (optional), Given: %d\n", nargs - 1);
		signal(run_cmd);
		return 1;
	}

	//Initializing semaphores
	//Don't want consume to run before produce, so initialize prod to 1 and cons to 0
	prod = semcreate(1); //Waited on at start of producer, signaled at end of consumer
	cons = semcreate(0); //Waited on at start of consumer, signaled at end of producer
	
	main = semcreate(0);

	resume( create(producer, 1000, 20, "producer", 1, count));		
	resume( create(consumer, 1000, 20, "consumer", 1, count));	

	//Signaled at end of idelgass/apps/consumer.c	
	wait(main);
	
	//Process has finished, can signal the run cmd wrapper now
	signal(run_cmd);
	return 0;
}

