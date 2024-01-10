#include <xinu.h>
#include <fs.h>
#include <string.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * fs_open - Return SYSERR if the file is already open or does not exist
 *     otherwise, add a record to the oft in the first current FSTATE_CLOSED
 *     slot corresponding to the opened file and copy the inode from the block
 *     device to the oft record and return the index into the oft table for the
 *     newly opened file.
 */
int fs_open(char* filename, int flags) {
	// Loop through oft looking for file by directory index de
	for(int i = 0; i < NUM_FD; i++){
		// Locate file by filename
		if(strcmp(fsd->root_dir.entry[oft[i].de].name, filename) == 0){
			// Check if file is already open
			if(oft[i].state == FSTATE_OPEN) return SYSERR;
			// if not, open it
			else if(oft[i].state == FSTATE_CLOSED){
				oft[i].state = FSTATE_OPEN;
			
				// Set flags from fs_open args
				oft[i].flag = flags;
				

				// Copy contents of inode block
				bs_read(fsd->root_dir.entry[oft[i].de].inode_block, 0, &oft[i].in, sizeof(inode_t));
			
				return i;
			}
		}
	}
  	return SYSERR;
}
