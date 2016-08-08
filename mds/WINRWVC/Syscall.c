/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/*
 * C Forth 83
 * Copyright (c) 1986 by Bradley Forthware
 * Portions copyright (c) Immersive Systems, Inc.
 *
 */
#include <windows.h>
#undef scr
#undef scr2
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "forth.h"
#include "externs.h"
#include "protos.h"

/* DOS Interrupt interface */
cell
dosyscall(callno, format, spp)
cell callno;
u_char *format;
cell **spp;
{
#ifdef REMOVE
    register cell *sp = *spp;
    union REGS regs;
    int count;
    int ret;

    count = *format++;
    while (count >= 0) {
	count--;
	switch(*format++) {
#ifdef _INTELC32_
	    case 'a': regs.h.ah = *sp++; break;
	    case 'A': regs.x.ax = *sp++; break;
	    case 'b': regs.h.bh = *sp++; break;
	    case 'B': regs.x.bx = *sp++; break;
	    case 'c': regs.h.ch = *sp++; break;
	    case 'C': regs.x.cx = *sp++; break;
	    case 'd': regs.h.dh = *sp++; break;
	    case 'D': regs.x.dx = *sp++; break;
	    case '-': goto dosys;
#else
	    case 'a': regs.h.ah = *sp++; break;
	    case 'A': regs.x.eax = *sp++; break;
	    case 'b': regs.h.bh = *sp++; break;
	    case 'B': regs.x.ebx = *sp++; break;
	    case 'c': regs.h.ch = *sp++; break;
	    case 'C': regs.x.ecx = *sp++; break;
	    case 'd': regs.h.dh = *sp++; break;
	    case 'D': regs.x.edx = *sp++; break;
	    case '-': goto dosys;
#endif
        }
    }
dosys:
    regs.h.ah = callno;
    ret = intdos(&regs, &regs);

    while (count >= 0) {
	count--;
	switch(*format++) {
#ifdef _INTELC32_
	    case 'a': *--sp = regs.h.ah; break;
	    case 'A': *--sp = regs.x.ax; break;
	    case 'b': *--sp = regs.h.bh; break;
	    case 'B': *--sp = regs.x.bx; break;
	    case 'c': *--sp = regs.h.ch; break;
	    case 'C': *--sp = regs.x.cx; break;
	    case 'd': *--sp = regs.h.dh; break;
	    case 'D': *--sp = regs.x.dx; break;
#else
	    case 'a': *--sp = regs.h.ah; break;
	    case 'A': *--sp = regs.x.eax; break;
	    case 'b': *--sp = regs.h.bh; break;
	    case 'B': *--sp = regs.x.ebx; break;
	    case 'c': *--sp = regs.h.ch; break;
	    case 'C': *--sp = regs.x.ecx; break;
	    case 'd': *--sp = regs.h.dh; break;
	    case 'D': *--sp = regs.x.edx; break;
#endif
	}
    }
    *spp = sp;
    return(regs.x.cflag ? ret : 0);
#endif	/* REMOVE */
	return 0;
}

#ifdef SIGNALS
/*
 * Interface between Forth interpreter and signal handling code:
 * 	env				- environment structure for setjmp
 *	init_signals()	- Arrange to handle signals
 *	setjmp(env)		- 
 *	longjmp(env)	-
 *	report_signal()	- Display a signal message.
 *	insane			- variable used to detect repeated signals
 */

#include <setjmp.h>
#include <signal.h>

jmp_buf env;
int insane = 0;

static int signal_received;
static char *signal_message;

#include <float.h>
char * fperrs[] = {
    "Floating point invalid operand\n",
    "Floating point denormalized number\n",
    "Floating point divide by zero\n",
    "Floating point overflow\n",
    "Floating point underflow\n",
    "Floating point inexact result\n",
    "Floating point unemulated operation\n",
    "Floating point square root of negative number\n",
    "Floating point unlisted error\n",	/* Not listed in float.h */
    "Floating point stack overflow\n",
    "Floating point stack underflow\n",
    "SIGFPE sent from other process\n",
    "Floating point-to-integer conversion overflow\n",
    "Floating point extra error 1", /* Not listed in float.h */
    "Floating point extra error 2", /* Not listed in float.h */
    "Floating point extra error 3", /* Not listed in float.h */
    "Floating point extra error 4", /* Not listed in float.h */
    "Floating point extra error 5", /* Not listed in float.h */
    "Floating point extra error 6", /* Not listed in float.h */
};

void
handle_signal(sig,code)
int sig, code;
{
/*	signal(sig, SIG_DFL);	/* Prevent reentrance */

	signal_received = sig;

	switch(sig) {
	    case SIGABRT:
	 	signal_message = "Abort\n";
		break;
	    case SIGFPE:
		signal_message = fperrs[code - FPE_INVALID];
		_fpreset();	/* DOS specific */
		break;
	    case SIGBREAK:
	    case SIGINT:
		signal_message = "Interrupt\n";
		break;
	    case SIGILL:
		signal_message = "Illegal Instruction\n";
		break;
	    case SIGSEGV:
		signal_message = "Memory access violation\n";
		break;
	    case SIGTERM:
	    	signal_message = "Termination request\n";
		break;
	    default:
	        signal_message = "Unknown signal\n";
		break;
	}
/*
 * If the interrupt occurs when taking input from a file,
 * recovery is probably not possible,
 * so doing a warm start is inappropriate.
 */
	if( input_name[0] != '\0' ) {
		sprintf(errMsg, "An exception occurred while loading %s:\n%s\n",
							&input_name[1], signal_message);
#ifdef REMOVE
		error(errMsg);
		where();
#else
	    strcat(errMsg, "At ");
	    strcat(errMsg, (char *)&wordbuf[1]);
		strcat(errMsg, ".");
	    if( input_name[0] != '\0' ) {
	        strcat(errMsg, "   Last: ");
			strcat(errMsg, tocstr(&((XV_LAST) -> name)));
			strcat(errMsg, "   File: ");
			strcat(errMsg, tocstr(input_name) );
	    }
	    strcat(errMsg, "\n");
		MessageBox(NULL, errMsg, "Unrecoverable error", MB_OK);
#endif
		ExitMeme(MEMESIGNALSTATUS);
	}
	if ( insane++ ){
	    ExitMeme(MEMESIGNALSTATUS);
	}
	signal(sig, handle_signal);
	longjmp(env,sig);
}

void
init_signals()
{
    signal(SIGINT,   handle_signal);
    signal(SIGBREAK, handle_signal);
    signal(SIGILL,   handle_signal);
    signal(SIGABRT,  handle_signal);
    signal(SIGFPE,   handle_signal);
    signal(SIGSEGV,  handle_signal);
    signal(SIGTERM,  handle_signal);
}

void
report_signal()
{
    if (signal_received != SIGINT && signal_received != SIGBREAK) {
		cprint("\n");
	}
    cprint(signal_message);
/*    signal(signal_received, handle_signal); */
}
#endif
