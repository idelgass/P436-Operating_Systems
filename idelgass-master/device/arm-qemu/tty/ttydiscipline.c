#include <xinu.h>

#define TY_ESC   0x1b
#define TY_BRACE 0x5b
#define TY_A     0x41

#define SKIP     0
#define DO_PRINT 1

local void echo(char ch, struct ttycblk* typtr, struct uart_csreg* csrptr);
local void clearline(struct ttycblk* typtr, struct uart_csreg* csrptr);

/*
 * ttydiscipline - Handles special character interactions.
 * 
 * Returns - Whether to print character (SKIP or DO_PRINT)
 */
int ttydiscipline(char ch,
		  struct ttycblk *typtr,
		  struct uart_csreg* csrptr) {

	  if (ch == TY_NEWLINE || ch == TY_RETURN) {
	    	/* 
	     	* Copy the contents of the 'tyibuff' buffer from the 'tyihead' through 'tyitail'
	     	*     into the 'typrev' buffer.
	     	*/
		
		memset(typtr->typrev, NULL, TY_IBUFLEN); //size of prev = size of i
		
		char *temp = typtr->tyihead; 
	  	int icsr = typtr->tyicursor;
		
		for(int i = 0; i < icsr; i++){
			typtr->typrev[i] = *temp;
			temp++;
		//	typtr->tyicursor--;
			if (temp >= &typtr->tyibuff[TY_IBUFLEN]) temp = typtr->tyibuff;
		}
	}

	 /*
	 * Check if the up key was pressed:
	 * Use 'tycommand' to indicate if a command is in progress and the previous character
	 *     If the characters appear in the sequence TY_ESC, then TY_BRACE, then TY_A
 	 *     the up key was sent
  	 */
  	// If the up key is detected (as above)
	if(ch == TY_ESC){
		typtr->tycommand = ch;
		return SKIP;
	}
	if(typtr->tycommand == TY_ESC && ch == TY_BRACE){
		typtr->tycommand = ch;
		return SKIP;
	}
	if(typtr->tycommand == TY_BRACE && ch == TY_A){
  		/*
       		* Clear the current input with the 'clearline' function and copy the contents of 
       		*     'typrev' buffer into the 'tyibuff' buffer
       		*     remember to reset the 'tyicursor' as well
       		*  Call 'echo' on each character to display it to the screen
       		*/
		clearline(typtr, csrptr);
		//Loop thru typrev from head to tail
		// Need to make this a proper ring buffer, this is not the right way
		// Jeremy said in class to just copy as a string and add a null character	
		
		// No cursor for typrev, what do?
		int i = 0;
		char c = '0'; // Place
		
		//Checking ony for if c is null means if the new output is shorter than the previous, the ending characters from the previous are printed as well
		while(c != NULL){
			c = typtr->typrev[i++];
			echo(c, typtr, csrptr);
			*typtr->tyitail = c;
			typtr->tyitail++;
			typtr->tyicursor++;
			if (typtr->tyitail >= &typtr->tyibuff[TY_IBUFLEN]) typtr->tyitail = typtr->tyibuff;
		}	

		
		return SKIP;	
	}
  	return DO_PRINT;
}

local void echo(char ch, struct ttycblk* typtr, struct uart_csreg* csrptr) {
  *typtr->tyetail++ = ch;

  /* Wrap around buffer, if needed */
  if (typtr->tyetail >= &typtr->tyebuff[TY_EBUFLEN])
    typtr->tyetail = typtr->tyebuff;
  ttykickout(csrptr);
  return;
}

local void clearline(struct ttycblk* typtr, struct uart_csreg* csrptr) {
  int i, j;
  while (typtr->tyitail != typtr->tyihead)
    if ((--typtr->tyitail) < typtr->tyibuff)
      typtr->tyitail += TY_IBUFLEN;

  for (i=0; i < typtr->tyicursor; i++) {
    echo(TY_BACKSP, typtr, csrptr);
    echo(TY_BLANK, typtr, csrptr);
    echo(TY_BACKSP, typtr, csrptr);
    for (j=0; j<3; j++)
      ttyhandle_out(typtr, csrptr);
  }
  typtr->tyicursor = 0;
}
