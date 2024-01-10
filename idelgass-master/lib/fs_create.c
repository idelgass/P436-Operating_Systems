#include <xinu.h>
#include <fs.h>
#include <string.h>

extern fsystem_t* fsd;

/*
 * fs_create - Return SYSERR if not enough space is available
 *     in the directory or the filename already exists.
 *     Otherwise:
 *        1. Find an available block on the block store
 *        2. Create an inode_t for the new file
 *        3. Update the root directory
 *        4. Write the inode and free bitmask back to the block device
 */
syscall fs_create(char* filename) {
	
	// Check if file already exists
        for(int j = 0; j < DIR_SIZE; j++){
        	if(!strcmp(fsd->root_dir.entry[j].name, filename)) return SYSERR;
        }
	// Check if too many create calls, i.e. dir is full
	if(fsd->root_dir.numentries == DIR_SIZE) return SYSERR;

	// Declare inode_t used for new file later 	
	inode_t *temp;

	// Loop through freemask until available block is discovered
	int fblk_discovered = 0;
	int i = 0;
	while(!fblk_discovered && (i < fsd->freemasksz)){
      		// Available block discovered
		if(fs_getmaskbit(i) == 0){
			fblk_discovered = 1;
			fs_setmaskbit(i); // Claim block as no longer available
			temp->id = i;
			temp->size = 0;
		}
		i++;
	}
	if(!fblk_discovered) return SYSERR;	
	
	// Update root directory
	fsd->root_dir.entry[fsd->root_dir.numentries].inode_block = temp->id;
	//fsd->root_dir.numentries++;
	memcpy(fsd->root_dir.entry[fsd->root_dir.numentries].name, filename, strlen(filename)); // Could use filenamelen

	bs_write(temp->id, 0, temp, sizeof(inode_t));
	bs_write(BM_BIT, 0, fsd->freemask, fsd->freemasksz);
	
	fsd->root_dir.numentries++;
	
	return OK;
}
