#include <xinu.h>
#include <heap.h>

extern heap_t heaptab[];



// My additions

// Header struct for each block of memory
typedef struct block_header {
    uint32 size;
    struct block_header *next;
} block_header;

//The head of the free list
static block_header *free_list_head = NULL;



void initheap(char* startaddr, uint32 size) {
  /* This function is called when the heap is constructed */
  /*   Use as needed to setup the free list               */
  	
	block_header *block = (block_header*)startaddr;
	block->size = size;
	block->next = NULL;

 	return;
}

void* malloc(uint32 size) {
  /* Implement memory allocation within the process heap here       */
  /*   Your implementation MUST NOT use getmem                      */
  /*   Your implementation SHOULD use explicit in-memory free lists */
  	
	//3 if statements
	//1. block is equal to size
	//2. block is more than size
	//3. block is less than size

	intmask mask;
	mask = disable();
	
	block_header *prev = NULL;
    	block_header *curr = (block_header*)heaptab[getpid()].freelist;
    	
	// Loop through the freelist
	while (curr != NULL) {
		//If block size is equal to required size
        	if(curr->size == size){
			block_header* block;
			if(prev == NULL) {
				//All this stuff w curr seems redundant given we return block soon
				block = curr;
				curr = curr->next;
			}
			else{
				block = curr;
				prev->next = curr->next;
				curr = curr->next;
			}
			restore(mask);
			return (void*)block;
		}
		//If block size is more than required size
		if(curr->size > size){
			block_header* block;
			block_header* leftover;
			
			//Splitting the block
                        leftover = (block_header*)((uint32)curr + size);
                        leftover->size = curr->size - size;
                        block = curr;
                        block->size = curr->size - leftover->size;
			
			//Remove curr from freelist
			if(prev == NULL){
				curr = leftover;
			}
			else{
				curr = leftover;
				prev->next = leftover;
			}

			heaptab[getpid()].freelist = (char*)leftover;
			restore(mask);
			return (void*)block;
		}
		// Block size is less than size
		else{
			prev = curr;
        		curr = curr->next;
		}
    	}
	restore(mask);
  	return (void*)SYSERR;
}

void free(char* block, uint32 size) {
  	/* Implement memory free within process heap here */
  	/*   Your implementation MUST implement coalesing */
  	intmask mask;
	mask = disable();

	if (block == NULL) {
		return;
    	}

	// Get a pointer to the block header
    	block_header *bh = (block_header *)block;
	bh->size = size;

    	// Add the block to the free list at the start
	// Set next of this block to be the old start of the freelist
	bh->next = (block_header*)heaptab[getpid()].freelist;
	// Set new start of free list to be this block
    	heaptab[getpid()].freelist = (char*)bh;
  	
	restore(mask);
	return;
}
