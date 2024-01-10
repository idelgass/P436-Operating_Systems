#include <future.h>
#include <stdio.h>

future_t* future_alloc(future_mode_t mode, uint size, uint nelems) {
  	intmask mask;
  	mask = disable();

  	// TODO: write your code here
	future_t* f;
	
	// Allocate mem for the future structure
	f = (future_t*)getmem(sizeof(future_t));
	if(f == (future_t*)SYSERR){
		return (future_t*)SYSERR;
	}

	f->state = FUTURE_EMPTY;
	f->size = size;
	f->mode = mode;
	f->pid = -1; // Dummy value
	
	// Allocate mem for future data
	f->data = getmem(size); // Think this should allocate appropraite mem for data, *(future->data) set later
	if(f->data == (void*)SYSERR){
		freemem((char*)f, sizeof(future_t));
		return NULL;
	}

  	restore(mask);
	return f;
}

//TODO: remember to also add definitions for future_set, future_get, future_free
syscall future_free(future_t* f){
	
	if(kill(f->pid) == SYSERR) return SYSERR;
	
	// Free mem for future data
	if(freemem(f->data, f->size) == SYSERR) return SYSERR;
	
	// Free mem for future structure
	if(freemem((char*)f, sizeof(future_t)) == SYSERR) return SYSERR;

	return OK;
}

// Out: pointer to the address in which the Future's value will be copied
syscall future_get(future_t* f,  void* out){
	if(f == NULL || out == NULL) return SYSERR;
	
	if(f->mode == FUTURE_EXCLUSIVE){
		if(f->state == FUTURE_WAITING) return SYSERR; // If future is already waited on by another get (FUTURE_WAITING) throw an error
		
		// If future is not yet ready, i.e. if future is empty (FUTURE_EMPTY) then reschedule and resume from future_set
		//TODO  DOES NOT WORK FOR MULTIPLE GETTERS PER SETTER (FUTURE QUEUE OR SHARED)
		while(f->state != FUTURE_READY){
			f->pid = getpid(); // Save pid of waiting process
			f->state = FUTURE_WAITING; // Claim future
			resched(); // Reschedule current process
		}

		// Now resuming from future_set...
		
		// Pass future's data and clear it to ready for next set
		//TODO use memcpy for arbitrary data types
		*((int*)out) = *((int*)f->data);
		f->pid = -1; // Dummy value 
		// Should I be freeing or assigning NULL here?
		f->state = FUTURE_EMPTY;
	}
	return OK;
}

syscall future_set(future_t* f, void* in){
	if(f == NULL || in == NULL) return SYSERR;

	if(f->mode == FUTURE_EXCLUSIVE){
		if(f->state == FUTURE_READY) return SYSERR; // If future has already been set (FUTURE_READY) throw an error
		
		// Otherwise (FUTURE_READY or FUTURE_EMPTY) we can set value
		*((int*)f->data) = *((int*)in);
		f->state = FUTURE_READY; // Now claim the future

		// resume thread that was waiting on this future
		if(f->pid != -1) resume(f->pid);
	}

	return OK;
}
