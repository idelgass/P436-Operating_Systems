#include<xinu.h>
#include<stdio.h>
#include<run.h>

shellcmd xsh_hello(int argc, char** argv){
	if(argc != 2){
		fprintf(stderr, "Incorrect number of arguments. Expected: 2, Given: %d\n", argc);
		signal(run_cmd);
		return 1;}
	fprintf(stdout, "Hello %s, welcome to the world of Xinu!\n", *(argv + 1));
	signal(run_cmd);
       return 0;
}      

