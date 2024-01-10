#include <xinu.h>
#include <prodcons.h>
#include <stdio.h>

void producer(int count) {
  // TODO: implement the following:
  // - Iterates from 0 to count (count including)
  //   - setting the value of the global variable 'n' each time
  //   - print produced value (new value of 'n'), e.g.: "produced : 8"
	for(int i = 0; i <= count; i++){
		wait(prod);
		
		//Critical
		n = i;
		fprintf(stdout, "produced : %d\n", n);
		
		signal(cons);
	}
}
