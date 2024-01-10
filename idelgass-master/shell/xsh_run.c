#include<xinu.h>
#include<stdio.h>
#include<string.h>
#include<shprototypes.h>
#include<run.h> //CAREFUL, WAS WORKING BEFORE

//Semaphore used for synchronization with suboutines. Waited on here, signaled at end of whichever subroutine is being run
int run_cmd;

//Initialize list of available functions
const char* list[] = {
	"hello"
	"list",
	"prodcons",
	"prodcons_bb",
	"futest",
	"memtest",
	"fstest"
};
#define NLIST (sizeof(list) / sizeof(const char *))

shellcmd xsh_run(int nargs, char* args[]){	
	// Print list of available functions
	if ((nargs == 1) || (strncmp(args[1], "list", 4) == 0)) {
  		for(int i = 0; i < NLIST; i++) fprintf(stdout, "%s\n", list[i]);
		return OK;
	}
	
	run_cmd = semcreate(0);

	if(strncmp(args[1], "hello", sizeof("hello")/sizeof(char)) == 0) {
 		/* create a process with the function as an entry point. */
		resume (create((void *) xsh_hello, 4096, 20, "hello", 2, nargs - 1, &(args[1])));
		wait(run_cmd);
	}
	else if(strncmp(args[1], "prodcons", sizeof("prodcons")/sizeof(char)) == 0){
		resume (create((void *) xsh_prodcons, 4096, 20, "prodcons", 2, nargs -1, &(args[1])));
		wait(run_cmd);
	}
	else if(strncmp(args[1], "prodcons_bb", sizeof("prodcons_bb")/sizeof(char)) == 0){
		resume (create((void *) xsh_prodcons_bb, 4096, 20, "prodcons_bb", 2, nargs - 1, &(args[1])));
		wait(run_cmd);
	}
	else if(strncmp(args[1], "futest", sizeof("futest")/sizeof(char)) == 0){
		resume (create((void*) xsh_futest, 4096, 20, "futest", 2, nargs - 1, &(args[1])));
		wait(run_cmd);
	}
	else if(strncmp(args[1], "memtest", sizeof("memtest")/sizeof(char)) == 0){
		resume (create((void*) xsh_memtest, 4096, 20, "memtest", 2, nargs - 1, &(args[1])));
		wait(run_cmd);
	}
	else if(strncmp(args[1], "fstest", sizeof("fstest")/sizeof(char)) == 0){
                resume (create((void*) xsh_fstest, 4096, 20, "fstest", 2, nargs - 1, &(args[1])));
                wait(run_cmd);
        }
	else{
		fprintf(stderr, "Command not recognized: %s\nList of recognized commands:\n", args[1]);
		for(int i = 0; i < NLIST; i++) fprintf(stdout, "\t%s\n", list[i]);
	}
	return 0;
}
