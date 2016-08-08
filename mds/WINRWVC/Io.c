/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Branched off from old version 1.15 */
/* io.c 1.11 92/09/02 */

/* Modified for Meme by Marc de Groot.
 * Portions of this file copyright (c) Immersive Systems, Inc.  
 * All rights reserved.
 */

/*
 * I/O subroutines for C Forth 83.
 * This code mostly uses C standard I/O, so it should work on most systems.
 *
 * Exported definitions:
 *
 * init_io();			    Initialize io system
 * error(s);			    Print a string on the error stream
 * actual = cexpect(addr, count);   Collect a line of input
 * forth_string input_name[];       Name of the current input file
 * prompt();			    Prompt for an input line
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "forth.h"
#include "externs.h"
#include "protos.h"
#include "sernum.h"
#include "memever.h"
#include "prims.h"
#include "ascii.h"

#include "winextrn.h"	

/* Used by the Editor Window proc */
BOOL pasting = FALSE;


extern char *version;
extern FILE *fopen();
extern char *tocstr();
extern void type();

#define MAXINFILES 16
struct infile {	/* Stack of input files */
    FILE *if_fp;
    u_char if_name[256];
} infiles[MAXINFILES];

int infileno;			/* Current input file number within stack */

static FILE *input_file;
static int gargc;
static char **gargv;

#define STRINGINPUT (FILE *) -5
#define CONSOLEINPUT (FILE *) -6
char strbuf[100];
char *strptr;

extern char myhostname[];

#define SERIALCHECK

void
title()
{
#ifndef MEMEVIEW
	char buff[200];

# ifdef SERIALCHECK
	char ser[SERNUMLEN];
	if (DecryptSernum(ser, CBlock.serial, &CBlock.checksum)) {
		fatal("Invalid serial number\n", MEMESERIALNUMSTATUS);
	}
# endif
	sprintf(buff, "Meme(tm) (c) 1993-%s Immersive Systems, Inc.  All rights reserved.\n", CurrentYear() );
	cprint(buff);
	cprint( "Version " );
	cprint( version );
#ifdef RWFIXED
	cprint( " [FixPt] " );
#endif
# ifdef _DEBUG
	cprint("  < DEBUG build > ");
# endif
	cprint("   Serial # ");
# ifdef	SERIALCHECK
	cprint(ser);
# else
	cprint("*** NOT FOR DISTRIBUTION ***"); 
# endif
	cprint("\n");
	cprint_evalcopymessage();	/* If this is an eval copy, print msg with
								 * expiration date.
								 */
# if RENDERING == RENDERWARE
	if (!interpreterOnly) {
		RenderingBanner();
	}
# endif
	NetTitle();
#endif
}

void
init_io()
{
	gargc = filesCount; gargv = filesText;
	infileno = -1;	/* No file open */
	input_file = (FILE *)0;
	if (gargc <= 1) {
		set_input_file("", CONSOLEINPUT);
	title();	
	} else {
		open_next_file();
	}
}

void
pop_all()
{
	while ((infileno >= 0) && (*input_name != '\0')) {
		load_pop();
	}
	if (infileno == -1) {
		(void)fflush(stdout);
		ExitMeme(0);
	}
}

void
cprint(str)
char *str;
{
	type((u_char *)str, (cell)strlen(str));
	/* No need to flush stdout; we're writing to the console output */
	/* (void)fflush(stdout); */
}

void
warning(str)
char *str;
{
	if (webMode || editWindowIsHidden) {
		if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
		MessageBox(NULL, str, "Warning", MB_OK);
	} else {
		cprint(str);
	}
}

void
error(str)
char *str;
{
	if (webMode) {
		if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
		MessageBox(NULL, str, "Fatal error", MB_OK);
		ExitMeme(1);
	} else if (editWindowIsHidden) {
		if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
		MessageBox(NULL, str, "Error", MB_OK);
	} else {
		cprint(str);
	}
}

void
fatal(str, status)
char *str;
int status;
{
	if (str[strlen(str) - 1] == '\n') str[strlen(str) - 1] = '\0';
	MessageBox(NULL, str, "Fatal error", MB_OK);
    ExitMeme(status);
}

int
nextchar(int canbackspace)
{
	register int	c;

	static HGLOBAL hGlobal;
	static char *lptstr;

	for (;;) {
		if (pasting) {
			if (*lptstr) {
				emit(*lptstr);
				return (*lptstr++);
			} else {
				pasting = FALSE;
				GlobalUnlock(hGlobal);
				CloseClipboard();
			}
		}
		if ( input_file == STRINGINPUT ) {
			if( (c = *strptr++) != '\0')
				return(c);
		} else {	/* Not STRINGINPUT */
			if (input_file == CONSOLEINPUT) {
				while ( /* key_avail() */ KEYAVAIL == 0) {
					execute_one(PAUSE);
				}
				if (!webMode && !editWindowIsHidden) {
					SetFocus(theEditWindow);
				}
				c = key();	/* key() returns the next char in the kbd queue */
				switch (c) {
					/* Copy or cut characters: send to edit control, do not return to expect */
					case CHAR_CTLC:
					case CHAR_CTLX:
						emit((u_char)c);
						continue;

					/* Paste character: get clipboard contents and set up state machine */
					case CHAR_CTLV:
						if (!IsClipboardFormatAvailable(CF_TEXT)) break;
						if (!OpenClipboard(NULL)) break;
						hGlobal = GetClipboardData(CF_TEXT);
						if (!hGlobal) break;
						lptstr = (char *)GlobalLock(hGlobal);
						if (!lptstr) break;
						pasting = TRUE;
						continue;

					/* Translate CR to LF and return it to expect */
					case CHAR_CTLM:
						c = CHAR_CTLJ;
						emit((u_char)c);
						return(c);

					/* Translate DEL to BS and do destructive backspace*/
					case CHAR_DEL:
						c = CHAR_CTLH;
					case CHAR_CTLH:
						if (canbackspace) {
							emit(CHAR_CTLH); emit(' '); emit(CHAR_CTLH);
						}
						return(c);
					default:
						emit((u_char)c);
						return(c);
				}
			} else {
				if( (c = getc(input_file)) != EOF )
					return(c);
			}
		}			/* End of file */
		load_pop();
		prompt();
		if (infileno < 0)
			open_next_file();
	}
}

void
cexpect(addr, count)
u_char *addr;
cell count;
{
	int c;
	u_char *p;
    
	if (input_file == CONSOLEINPUT) {
		linemode();
	}

	for (p = addr; count > 0; count--) {
		c = nextchar(p > addr);
		if (c == EOF) {
			(void)fflush(stdout);
			ExitMeme(0);
		}
		if (c == 0x8) {
			if (p != addr) {
				p--;
			}
		} else {
			*p++ = c;
		}
		/* This violates the Forth 83 standard which says that the newline */
		/* at the end of the line is not stored in the user's buffer, but */
		/* storing the newline makes backslash SO much easier */
		if ( c == '\n')
			break;
	}

	if ( *input_name == '\0' ) { 
		/* We have to do this because we're using the terminal driver */
		/* to do the echoing, and the 'return' that ends the line puts the */
		/* cursor at column 0 */
		XV_NUM_OUT = 0;
	}
	XV_SPAN = (cell)(p - addr); 
}

void
load(filename)
u_char *filename;
{
#ifdef MEMEVIEW
	file_error("Viewer does not have load capability.  Didn't load ", tocstr(filename));
#else
    FILE *file;

    if ((file = fopen((char *)&filename[1], "r")) == (FILE *)0) {
		file_error("Can't open ", tocstr(filename));
    } else {
		set_input_file(tocstr(filename), file);
    }
#endif
}

void
file_error(str, filename)
char *str, *filename;
{
	sprintf(errMsg, "%s%s\n", str, filename);
	error(errMsg);
}

void
set_input_file(str, file)
char *str;
FILE *file;
{
    if (++infileno >= MAXINFILES) {
		file_error("Loads nested too deeply\n", str);
		--infileno;
		return;
    }
    input_name = infiles[infileno].if_name;
    (void)strcpy ((char *)&input_name[1], str);
    input_name[0] = strlen(str);
    input_file = infiles[infileno].if_fp = file;
}

void
prompt()
{
    if ( *input_name == '\0' ) { 
	if (XV_NUM_OUT != 0)
	    emit( '\n' );
	if (xup != (cell *)main_task) {
	    cprint("<bg>");
	}
	if (XV_STATE == (cell)INTERPRETING)
	    cprint("ok ");
	else
	    cprint(" ] ");
    }
}

void
load_pop()
{
    if (input_file != CONSOLEINPUT && input_file != STRINGINPUT)
	(void)fclose(input_file);
    --infileno;
    if (infileno >= 0) {
	input_file = infiles[infileno].if_fp;
	input_name = infiles[infileno].if_name;
    }
}

void
open_next_file()
{
	register FILE * file;
	static int vflag = 0; /* controls reporting of file names */

	while (--gargc > 0) {
		++gargv;
		if ((file = fopen(*gargv, "r")) == (FILE *)0) {
			file_error("Can't open ",*gargv);
			continue;
		} else {
			set_input_file(*gargv, file);
			if (vflag) {
				file_error("File: ",*gargv);
			}
			return;
		}
	}
	/* We've run out of input, so exit */
	(void)fflush(stdout);
	ExitMeme(0);
}

/* pfgetc - read a byte from a file.
 * fp - file pointer
 * returns a cell.  The low 8 bits are
 * the read character.  If the high bit
 * is set it signals EOF.
 */
cell
pfgetc(fp)
FILE *fp;
{
	cell rtn = 0;
	if ( (rtn = fgetc(fp)) == EOF ) {
		return 0x80000000;
	} else {
		return rtn & 0xff;
	}
}

/* pfgets - read a string of bytes from a file.
 * adr - address of buffer
 * len - number of bytes
 * f - file descriptor
 *
 * returns actual number of bytes read
 */
cell
pfgets(adr, len, f)
u_char *adr;
register cell len;
register FILE *f;
{
    register int c;
    register u_char *p;

    for (p = adr; len--; p++) {
		if ((c = getc(f)) == EOF) {
		    break;
		}
		*p = (u_char)c;
    }
	MemeOften();
    return((cell)(p-adr));
}

cell
pfgetline(adr, f)	/* Returns 0 if end of file */
u_char *adr;
register FILE *f;
{
    register int c;
    register u_char *p;

    *adr = 0;
    if ((c = getc(f)) == EOF) {
		return((cell)0);
	}

    for( p = adr+1; c != CNEWLINE && c != EOF; ) {
		if (p - adr < 256) {
			*p++ = c;
		}
		c = getc(f);
	}

    if (p[-1] == '\r' || p[-1] == '\n') {
    	--p;
	}
   	*adr = (p - adr < 256) ? p - adr - 1 : 255 ;
	
	MemeOften();
    return((cell)-1);
}
