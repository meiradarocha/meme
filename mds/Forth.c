/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme - The Multitasking Extensible Messaging Environment
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 * Portions of this code copyright (c) 1986 by Bradley Forthware.
 */


#include <windows.h>
#ifdef	scr1
# undef	scr1
#endif
#ifdef	scr2
# undef	scr2
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <direct.h>

#ifndef unix
# include <conio.h>
# include <io.h>
#endif

#include <sys/timeb.h>
#include <time.h>

#ifdef unix
#include <sys/errno.h>
#endif

#include "config.h"
#include "forth.h"
#include "meme.h"
#include "prims.h"
#include "memever.h"
#include "memeops.h"
#include "serops.h"
#include "externs.h"
#include "protos.h"
#include "cyberspc.h"
#include "rfile.h"
#include "errors.h"

/*
 * Timeout in seconds for SendMessage
 */
#define	SENDMESSAGETIMEOUT	(20)

#define binop(operator)	 (tos = *sp++ operator tos)
#define unop(operator)	 (tos = operator tos)
#define bincmp(operator) tos = ((*sp++ operator tos)?-1:0)
#define uncmp(operator)	 tos = ((tos operator 0)?-1:0)
#define branch		 ip += *(cell *)ip;
#define next		 continue
#define push(whatever)	 *--sp = tos; tos = (cell)(whatever)
#define pop		 tos; tos = *sp++
#define apop		 (u_char *)pop
#define fpop		 (FILE *)pop
#define ncomma(n)	 *DP++ = (cell)n
#define comma		 ncomma(pop)
#define forw_mark	 push ( DP ); ncomma(1);
#define forw_resolve	 { cell *start = (cell *)pop;*start = DP - start; }
#define back_mark	 push ( DP );
#define back_resolve	 { cell *start = (cell *)pop; *DP = start-DP;DP++;}

#define	flpush(x)	*--fsp = ftos; ftos = (x);
#define flpop		ftos; ftos = *fsp++;

#define	ThisModule	((module *)XV_CURRENT)

/*
 * The following plausible definition for back_resolve is incorrect
 * because DP could be incremented before (start-DP) is evaluated.
 * #define back_resolve	 { cell *start = (cell *)pop; ncomma((start-DP));}
 */
#define create(cf)	 quote_create((u_char *)canonical(blword()), (token_t)(cf));

#define	SAVE_XCONTEXT *--sp = tos;  xsp = sp;  xrp = rp; *--fsp = ftos;  xfsp = fsp; xup = up; xfrp = frp;

#define	RESTORE_XCONTEXT  frp = xfrp;  up = xup; fsp = xfsp; ftos = *fsp++; rp = xrp; sp = xsp; tos = *sp++;

#define SAVE_TASKCONTEXT *--sp = tos; V_SAVED_IP = (cell) ip; V_SAVED_RP = (cell) rp;  V_SAVED_SP = (cell) sp; *--fsp = ftos; V_SAVED_FP = (cell) fsp; V_SAVED_FRP = (cell) frp;

#define	RESTORE_TASKCONTEXT frp = (cell *) V_SAVED_FRP; fsp = (double *) V_SAVED_FP; ftos = *fsp++; sp = (cell *) V_SAVED_SP; tos = *sp++; rp = (token_t **) V_SAVED_RP; ip = (token_t *) V_SAVED_IP;

#define SAVE_XTASKCONTEXT XV_SAVED_IP = (cell) ip; XV_SAVED_RP = (cell) xrp;  XV_SAVED_SP = (cell) xsp; XV_SAVED_FP = (cell) xfsp; XV_SAVED_FRP = (cell) xfrp;

#define	RESTORE_XTASKCONTEXT xfrp = (cell *) XV_SAVED_FRP; xfsp = (double *) XV_SAVED_FP; xsp = (cell *) XV_SAVED_SP; xrp = (token_t **) XV_SAVED_RP; ip = (token_t *) XV_SAVED_IP;

#define minuslevel if (declevel()) { SAVE_XCONTEXT (void)inner_interpreter(comp_buffer); RESTORE_XCONTEXT }

#define seterror	V_ERRNO = (tos < 0) ? errno : 0
/* #define compile(token) (*DP++ = (cell)token) */
#define linkcomma(adr)	tokstore((token_t)adr, DP++)
#define xlinkcomma(adr)	tokstore((token_t)adr, XDP++)
#define compile(token)  tokstore((token_t)token, DP++)
#define xcompile(token)  tokstore((token_t)token, XDP++)

/* Top of parameter stack */
#define ps_top   (&parameter_stack[PSSIZE])

/* From meme.c */
extern	cell	NumberObjects();

/* From memerend.c */
extern	cell	NumberFrames;

int
inner_interpreter(ip)
register token_t *ip;
{
    register cell tos = *xsp++;		/* Top of int stack		*/
    register cell *sp = xsp;		/* The inner interpreter's sp	*/
    register token_t *token;		/* Equivalent to the W register	*/
    register token_t **rp = xrp;	/* Inner interpreter's r stk ptr */
    register double *fsp = xfsp;	/* The float stack pointer	*/
    register double ftos = *fsp++;	/* Top of float stack		*/
    register cell scr;				/* Scratch register for ints	*/
    register u_char *ascr;			/* Scratch reg for addresses	*/
	register cell *frp = xfrp;		/* Frame pointer for local variables */
    register cell *up = xup;		/* The user pointer		*/
	cell *entrycontext = xup;		/* Checked in FINISHED */
    FILE *file;						/* Scratch file pointer		*/
    u_char *ascr1;					/* Yet another scratch addr reg	*/
    cell scr2;						/* auto var, can take addr of it */
    long lscr, lscr1;				/* Double-prec scratch (archaic) */
    double ftemp;					/* Floating scratch		*/
    unsigned long *ulongp;			/* Pointer scratch		*/

	for (;;) {
	    token = *(token_t **)ip++;		/* W = (IP)++			*/
doprim:
 		switch ((token_t)token) {		/* Jmp W			*/
			case 0:
				sprintf(errMsg, "Tried to execute a null token at 0x%x\n", (u_cell)ip);
				error(errMsg);
				goto abort;
			case NOT:	 unop (~);     next;
			case AND:	 binop (&);    next;
			case OR:	 binop (|);    next;
			case XOR:	 binop (^);    next;
			case PLUS:	 binop (+);    next;
			case MINUS:	 binop (-);    next;
			case TIMES:	 binop (*);    next;
#define FLOORFIX(dividend, divisor, remainder)  ((dividend < 0) ^ (divisor < 0))  &&  (remainder != 0)
			case DIVIDE:{
		    	register cell quot, rem;

		    	scr = *sp++;
		    	quot = scr/tos;
		    	rem  = scr - tos*quot;
		    	if (FLOORFIX(tos,scr,rem))
					tos = quot - 1;
		    	else
					tos = quot;
			} next;
    		case MOD: {
		    	register cell rem;

		    	scr = *sp++;  rem = scr%tos;
		    	if (FLOORFIX(tos,scr,rem))
					tos = tos + rem;
		    	else
					tos = rem;
			} next;
    		case TIM_DIV_MOD: {
		    	register long dividend;
		    	register cell quot, rem;

		    	dividend = *sp++;
		    	dividend *= *sp++;
		    	quot = dividend/tos;
		    	rem  = dividend - tos*quot;
		    	if (FLOORFIX(dividend,tos,rem)) {
					*--sp = rem  + tos; 
					tos = quot - 1;
		    	} else {
		        	*--sp = rem ;
					tos = quot;
		    	}
			} next;
			case SHIFT:
				if ( tos < 0 ) {
    		 	    tos = -tos;
			    	tos = (unsigned cell) *sp++ >> (unsigned cell)tos;
				} else {
					binop(<<);
				}
			next;
			case SHIFTA:	binop(>>); next;
    		case DUP:		*--sp = tos;  next;
    		case DROP:		tos = *sp++;  next;
    		case SWAP:		scr = *sp;    *sp = tos;      tos = scr;      next;
    		case OVER:		push (sp[1]); next;
    		case ROT:		scr = tos;    tos = sp[1];    sp[1] = *sp;    *sp = scr;
			next;
    		case PICK:		tos = sp[tos]; next;
    		case QUES_DUP: if (tos) { *--sp = tos; }    next;
    		case TO_R:		*(cell *)--rp = pop;	    next;
    		case R_FROM:	push ( *(cell *)rp++ );    next;
    		case R_FETCH:	push ( *(cell *)rp );	    next;
    		/* We don't have to account for the tos in a register, because
    		 * push has already pushed tos onto the stack before
    		 * V_SPZERO - sp  is computed.
    		 */
    		case DEPTH:		push ( (cell *)V_SPZERO - sp ) ; next;
    		case LESS:		bincmp (<);     next;
			case EQUAL:		bincmp (==);    next;
    		case GREATER:	bincmp (>);     next;
    		case ZERO_LESS:	uncmp (<);      next;
    		case ZERO_EQUAL:	uncmp (==);     next;
			case ZERO_GREATER:	uncmp (>);      next;
    		case U_LESS:	tos = ((u_cell) * sp++ < (u_cell) tos) ? -1 : 0; next;
    		case ONE_PLUS:	tos++;	     next;
    		case TWO_PLUS:	tos += 2;     next;
    		case TWO_MINUS:	tos -= 2;     next;
    		case U_M_TIMES:
				lscr = (u_cell) *sp * (u_cell) tos;
				*sp = lscr;
				tos = (lscr < 0) ? -1 : 0 ;
			next;
			case TWO_DIVIDE:   tos >>= 1;    next;	   /* Should be signed */
    		case PMAX:  scr = *sp++; if (tos < scr) { tos = scr; }    next;
    		case PMIN:  scr = *sp++; if (tos > scr) { tos = scr; }    next;
    		case ABS:			if (tos < 0)   { tos = -tos; }   next;
    		case NEGATE:  unop (-);		 next;
    		case FETCH:	  tos = *(cell *)tos;	 next;
    		case C_FETCH: tos = *(u_char *)tos;	 next;
    		case W_FETCH: tos = *(unsigned short *)tos; next;
    		case STORE:			*(cell *)  tos = *sp++; tos = *sp++;    next;
    		case C_STORE:		*(u_char *)  tos = (u_char)*sp++; tos = *sp++;    next;
    		case W_STORE:		*(unsigned short *) tos = (unsigned short)*sp++; tos = *sp++;    next;
    		case PLUS_STORE: *(cell *) tos += *sp++; tos = *sp++;    next;
    		case CMOVE:
				ascr = (u_char *)*sp++;
				ascr1 = (u_char *)*sp++;
				cmove(ascr1, ascr, (u_cell)tos);
				tos = *sp++;
			next;
    		case CMOVE_UP:
				ascr = (u_char *)*sp++;
				ascr1 = (u_char *)*sp++;
				cmove_up(ascr1, ascr, (u_cell)tos);
				tos = *sp++;
			next;
    		case FILL: 
				scr = *sp++;
				ascr = (u_char *)*sp++;
				fill((u_char *)ascr, (u_cell)scr, (u_char)tos);
				tos = *sp++;
			next;
			case COUNT: 
				*--sp = (cell)(tos + 1);
				ascr = (u_char *) tos;
				tos = (cell)(*ascr);
			next;
			case DASH_TRAILING: 
				ascr = (u_char *) (*sp + tos);
				tos++;
				while ((--tos != 0) && (*--ascr == ' ')) {}
				next;
			case PAREN:	SAVE_XCONTEXT
				(void)word(')');
				RESTORE_XCONTEXT
				next;
			case BACKSLASH:  if (V_DELIMITER != '\n') { 
				SAVE_XCONTEXT (void)word('\n'); RESTORE_XCONTEXT }	 next;
			case I:    push ( ((cell *)rp)[0] + ((cell *)rp)[1] );	next;
			case J:    push ( ((cell *)rp)[3] + ((cell *)rp)[4] );	next;
			case BRANCH:       branch;	     next;
/* The speedup achievable by storing byte branch offsets is insignificant */
			case QUES_BRANCH:
				if (tos == 0) {
					branch;
				} else {
					/* Pointer alignment */
					ip = (token_t *)(((char *)ip) + sizeof(cell));
				}
				tos = *sp++;
				next;
			case UNNEST:
			case EXIT:	       ip = *rp++;     next;
			case EXECUTE:      token = (token_t *)pop;
execute:
				if ((token_t)token > MAXPRIM  &&  *token < MAXPRIM)
					token = (token_t *)*token;
				goto doprim;

			case INTERPLINE:
				SAVE_XCONTEXT
				interpret_line();
				RESTORE_XCONTEXT
				next;

			case KEY:
#if 1
				if (multitasking && !key_avail()) {
					--ip;
					if (*ip == EXECUTE) {
						push(KEY);	/* Don't delete braces; push is a macro */
					}
					goto pause;
				}
				push (key());
				next;
#else
				if (key_avail()) { push (key()); next; }
				if (multitasking) {
					--ip;
					if (*ip == EXECUTE) {
						push(KEY);	/* Don't delete braces; push is a macro */
					}
					goto pause;
				}
#endif
			case KEY_QUESTION:
				SAVE_XCONTEXT
				MemeOften();
				RESTORE_XCONTEXT
				push ( key_avail() );
				next;
			case EMIT:		scr = pop; SAVE_XCONTEXT emit ((u_char)scr);     
							RESTORE_XCONTEXT next;
			case CR: SAVE_XCONTEXT emit ('\n'); RESTORE_XCONTEXT next;
			case DOT_PAREN: 
				SAVE_XCONTEXT
				ascr = word (')');
				RESTORE_XCONTEXT
				push( &ascr[1] );
				push( *ascr );
				/* Fall through */
			case TYPE: {
				cell scr1;
				scr1 = pop; scr = pop;
				SAVE_XCONTEXT
				type( (u_char *) scr, scr1);
				RESTORE_XCONTEXT
			} next;
			case TO_BODY:      tos += sizeof (cell);   next;
			case ALLOT:
				V_DP = (cell)((cell *)((char *)DP + tos));	/* Ptr alignment */
				tos = *sp++;
				/* The following cast is necessary for Turbo C in huge model */
				if ((u_cell)DP > (u_cell)dict_end && V_MOD_COMP == 0)
					error( "Out of dictionary space\n" );
				next;
			case FIND:	push ( find (sp) );	   next;
			case VFIND: tos = vfind ((u_char **)sp, (vocabulary_t *)tos); next;
			case EXPECT: 
				token = (token_t *)(V_TICK_EXPECT);
				goto execute;
			case SYS_EXPECT:
#ifdef REMOVE
				cexpect ((u_char *)*sp++, (cell)tos);
				tos = *sp++;
				next;
#else
				scr = pop;
				ascr = apop;
				SAVE_XCONTEXT
				cexpect ((u_char *)ascr, (cell)scr);
				RESTORE_XCONTEXT
				next;
#endif
			case ABORT:
abort:
				if (comp_level) V_DP = (cell) saved_dp;
				comp_level = 0;
				push (-1);
				/* Fall through */

			case THROW:
				if (tos != 0) {
					if (V_HANDLER == 0) {
						/* Modified for background tasks */
						rp = (token_t **)V_RPZERO;
						sp = ( (cell *)V_SPZERO ) + 1;
						fsp = ( (double *)V_FPZERO ) + 1;
						frp = (cell *)0;
						goto throwexit;
					}
					rp = (token_t **)V_HANDLER;
					V_HANDLER = (cell)*rp++;
/* NEW */
					frp = *(cell **)rp++;		/* Restore frame pointer */
					fsp = *(double **)rp++;		/* Restore float stack ptr */

					ulongp = (unsigned long *)rp;
					fbits.fb_long[1] = ulongp[0];
					fbits.fb_long[0] = ulongp[1];
					rp = (token_t **)(ulongp+2);
					ftos = fbits.fb_float;		/* Restore float top of stk */
/* End NEW */
					sp = ((cell *)*rp++)+1;	    /* Err number remains in tos */
					ip = *rp++;
/* Added this because THROW was leaving the zero on the stack */
				} else {
					tos = *sp++;
				}
/* End of added code */
				next;
	    
			case QUIT:
				/* Don't let the inner interpreter exit unless we have
				 * the same task context as we had on entry.
				 */
throwexit:
				if (entrycontext != up) {
					--ip;	/* FIX: doesn't work with EXECUTE */
					goto pause;
				}
				pop_all();
				/*
				 * Restore the local copies of the virtual machine
				 * registers to the external copies and exit to the
				 * outer interpreter.
				 */
				V_BLK = 0;
				V_TICK_TIB = (cell)&tibbuf[0];
				SAVE_XCONTEXT
				return(1);
			case FINISHED:
				/* Don't let the inner interpreter exit unless we have
				 * the same task context as we had on entry.
				 */
				if (entrycontext != up) {
					--ip;		/* FIX: doesn't work with EXECUTE */
					SAVE_TASKCONTEXT
					do {
						up = (cell *)V_LINK;
					} while (up != entrycontext);
					RESTORE_TASKCONTEXT
					if (!V_TASK_ACTIVE) {
						goto pause;
					}
				}
				/*
				 * Restore the local copies of the virtual machine registers
				 * to external copies and exit to the outer interpreter.
				 */
				SAVE_XCONTEXT
				return(0);
			case DECIMAL:      V_BASE = 10;	  next;
			case HEX:	       V_BASE = 16;	  next;
			case COLD:			longjmp(env, SETJMPRTNCOLDSTART);
			case HERE:	       push ( DP );		 next;
			case TIB:	       push ( V_TICK_TIB );	 next;
			case DOT: scr = pop; SAVE_XCONTEXT dot( scr ); RESTORE_XCONTEXT next;
			case U_DOT: scr = pop;
				SAVE_XCONTEXT udot((u_cell)scr ); RESTORE_XCONTEXT 
				next;
			case PWORD:
				scr = pop;
				SAVE_XCONTEXT 	
				scr = (cell) word((u_char)scr);
				RESTORE_XCONTEXT
				push(scr);
				next;
			case COMMA:	       comma;			 next;
			case DOT_QUOTE:
				SAVE_XCONTEXT 
				xcompile(P_DOT_QUOTE); comma_string( word('"') ); 
				RESTORE_XCONTEXT next;
			case COLON:
				SAVE_XCONTEXT
				create (DOCOLON); hide();   
				RESTORE_XCONTEXT
				V_STATE = (cell)COMPILING; V_CONTEXT = V_CURRENT;
				next;
			case SEMICOLON:    
				SAVE_XCONTEXT
				xcompile(UNNEST);  reveal(); 
				RESTORE_XCONTEXT
				V_STATE = (cell)INTERPRETING;   next;
			case ABORT_QUOTE:
				SAVE_XCONTEXT
				xcompile(PAREN_ABORT_QUOTE);    comma_string(word('"'));	   
				RESTORE_XCONTEXT
				next;
			case COMPILE:
				SAVE_XCONTEXT
				xcompile(PAREN_COMPILE);
				RESTORE_XCONTEXT next;
			case CONSTANT: SAVE_XCONTEXT    create (DOCON); RESTORE_XCONTEXT
				comma;	    next;
			case VARIABLE: SAVE_XCONTEXT create (DOVAR); RESTORE_XCONTEXT
				ncomma(0);  next;
			case CREATE: SAVE_XCONTEXT create (DOVAR);	RESTORE_XCONTEXT next;
			case QUOTE_CREATE: ascr = apop;
				SAVE_XCONTEXT
				quote_create (ascr, (token_t)DOVAR); 
				RESTORE_XCONTEXT next;
			case USER_SIZE:    push (MAXUSER);	next;
			case USER: SAVE_XCONTEXT create (DOUSER); RESTORE_XCONTEXT
				comma;	next;
			case IMMEDIATE:    SAVE_XCONTEXT makeimmediate (); 
				RESTORE_XCONTEXT next;
			case DOES: SAVE_XCONTEXT xcompile(P_DOES);	RESTORE_XCONTEXT next;
			case IF:	SAVE_XCONTEXT pluslevel(); xcompile(QUES_BRANCH); 
				RESTORE_XCONTEXT forw_mark; next;
			case ELSE:	SAVE_XCONTEXT xcompile(BRANCH); RESTORE_XCONTEXT
				forw_mark;
				scr = *sp;   *sp = tos;	  tos = scr;  /* swap */
				forw_resolve;
				next;
			case THEN:	forw_resolve;  minuslevel; next;
			case FORW_RESOLVE: forw_resolve;	next;
			case FORW_MARK:	  forw_mark;		next;
			case LITERAL: 
				SAVE_XCONTEXT xcompile(PAREN_LIT); RESTORE_XCONTEXT comma; next;
			case DO:	SAVE_XCONTEXT pluslevel(); xcompile(P_DO);   
				RESTORE_XCONTEXT
				forw_mark;     back_mark;  next;
			case QUES_DO: SAVE_XCONTEXT pluslevel(); 
				xcompile(PAREN_QUESTION_DO);
				RESTORE_XCONTEXT    forw_mark;	 back_mark;	 next;
			case LOOP:	SAVE_XCONTEXT xcompile(PAREN_LOOP);  RESTORE_XCONTEXT
				back_resolve; forw_resolve;
				minuslevel; next;
			case PLUS_LOOP:	SAVE_XCONTEXT xcompile(PAREN_PLUS_LOOP);
				RESTORE_XCONTEXT back_resolve;
				forw_resolve;  minuslevel;  next;
			case QUES_LEAVE: scr = pop;  if (!scr) { next; }
				 /* else fall through */
			case LEAVE:		rp += 2;	/* Throw away the loop indices */
				ip = *(token_t **)rp++; /* Go to location after (do */
				branch;	      /* Get the offset there */
				next;
			case BEGIN: SAVE_XCONTEXT pluslevel(); RESTORE_XCONTEXT 
				back_mark; next;
			case BACK_MARK:	back_mark;	     next;
			case BACK_RESOLVE: back_resolve;	     next;

			case WHILE:	SAVE_XCONTEXT xcompile(QUES_BRANCH); RESTORE_XCONTEXT 
				forw_mark; next;
			case REPEAT:scr = *sp; *sp = tos; tos = scr;   /* swap */
				SAVE_XCONTEXT xcompile(BRANCH); RESTORE_XCONTEXT
				back_resolve; /* Branch back to begin */
				forw_resolve;		     /* Complete while */
				minuslevel; next;
			case UNTIL:	SAVE_XCONTEXT xcompile(QUES_BRANCH); RESTORE_XCONTEXT
				back_resolve;
				minuslevel; next;
    case LEFT_BRACKET:	  V_STATE = (cell)INTERPRETING;  next;
    case TICK:	{ u_char *temp;
	SAVE_XCONTEXT temp = canonical(blword());
	if ( find((cell *)&temp) ) { RESTORE_XCONTEXT push ( temp );  }
	else		   { where();  RESTORE_XCONTEXT goto abort;}
	} next;

    case BRAC_TICK: SAVE_XCONTEXT xcompile(PAREN_TICK); RESTORE_XCONTEXT
	/* Yes, it is correct that there is no "next" here */
    case BRAC_COMPILE: 
	SAVE_XCONTEXT scompile(canonical(blword ())); RESTORE_XCONTEXT next;
    case RIGHT_BRACKET:	  V_STATE = (cell)COMPILING;	  next;
    case PAREN_QUESTION_DO:
	scr = *sp++;
	if ( scr == tos ) { tos = *sp++; branch; next; }

	*(token_t **)--rp = ip++;		/* Address of offset to end */
	*(cell *)--rp =	scr ;		/* limit value */
	*(cell *)--rp = tos - scr ;		/* Distance up to 0 */
	tos = *sp++;
	next;

    case P_DO: 
	scr = *sp++;
	*(token_t **)--rp = ip++;		/* Address of offset to end */
	*(cell *)--rp =	scr ;		/* limit value */
	*(cell *)--rp = tos - scr ;		/* Distance up to 0 */
	tos = *sp++;
	next;

    case PAREN_LOOP: 
	if (++(*(cell *)rp) != 0) {
	    branch;
	    next;
	}
	/* Loop terminates; clean up return stack and skip branch offset */
        /* Pointer alignment */
	rp = (token_t **)((char *)rp + 2 * sizeof(cell) + sizeof(token_t *));
	++ip; next;
    case PAREN_PLUS_LOOP: 
	/*
	 * The loop terminates when the index crosses the boundary between
	 * limit-1 and limit.  We have biased the internal copy of the index
	 * so that the loop terminates when the internal index crosses the
	 * boundary between -1 and 0.  In the +LOOP case, we have to cope
	 * with the possibility of either positive or negative increment
	 * values.  The following calculation assumes 2's-complement
	 * arithmetic.  It can be understood as follows:
	 * tos: increment value   scr: old biased index
	 * scr+tos: new biased index
	 * Continue looping if the new biased index and the increment
	 * value have different signs (we haven't crossed the boundary yet),
	 *r if the old biased index and the increment value have the
	 * same sign (we are more than half the number circle away from
	 * the -1/0 boundary).
	 * This scheme allows loops to work over both signed number ranges
	 * and unsigned address ranges, with problems at the "rollover"
	 * point where the largest signed positive integer is adjacent to
	 * the smallest negative integer.
	 * Typically, in assembly language Forth implementations, the
	 * index is biased to terminate at that rollover point, using the
	 * overflow bit to test for the boundary crossing.  The overflow
	 * bit is not available from C.  A calculation very similar to the
	 * following may be used to test for overflow (just interchange
	 * < and >=).  However, the normal LOOP case may be implemented more
	 * efficiently in C when the boundary is at 0, and we must use the
	 * same boundary for LOOP and +LOOP since the same DO sets up the
	 * biased index in both cases.
	 */
        scr = *(cell *)rp;
	if ((((*(cell *)rp = scr+tos)^tos) < 0) || ((scr^tos) >= 0)) {
	    tos = *sp++; branch; next;
	}
	/* Loop terminates; clean up return stack and skip branch offset */
	tos = *sp++;
        /* Pointer alignment */
	rp = (token_t **)((char *)rp + 2 * sizeof(cell) + sizeof(token_t *));
	++ip; next;
/*
 * This is subtle.  The rule is that whenever a token appears on the stack,
 * it is in the form of the absolute address of the code field of its header.
 * When a token is stored in a definition, it is either the switch index
 * for a primitive or the code-field-address.  (' has to decide which it
 * is, and if it is the switch index, convert it to the appropriate cfa.
 * A table of cfa's indexed by the switch index appears as the first thing
 * in the dictionary, at origin.  This table is constructed by the code
 * that initializes the dictionary.
 */
    case PAREN_TICK:	token = *(token_t **)ip++;
			if ( (token_t)token < MAXPRIM )
			    token = (token_t *)origin[(cell)token];
			push ( token ); next;

    case PAREN_LIT:    push ( *ip++ );	  next;

    case P_DOES:
	SAVE_XCONTEXT
	tokstore((token_t)ip,
	         (cell *)name_from(&((dict_entry_t *)V_LAST) -> name));
	RESTORE_XCONTEXT
	ip = *rp++;
	next;

    case P_DOT_QUOTE:
	ascr = (u_char *)ip;
	SAVE_XCONTEXT
	type( ascr+1, (cell)*ascr );
	RESTORE_XCONTEXT
	ip = aligned( ascr + *ascr + 1 );
	next;
    case PAREN_ABORT_QUOTE:
	ascr = (u_char *)ip;
	if (tos != 0) {
	    SAVE_XCONTEXT
	    type(ascr+1, (cell)*ascr);
	    RESTORE_XCONTEXT
	    tos = *sp++;
	    goto abort;
	}
	tos = *sp++;
	ip = aligned( ascr + *ascr + 1 );
	next;
    case PAREN_COMPILE: SAVE_XCONTEXT xcompile(*ip++); RESTORE_XCONTEXT next;
    case BYE:
	ExitMeme(MEMESUCCESSFULSTATUS);
    case LOSE: SAVE_XCONTEXT error("Undefined word encountered\n");  
		RESTORE_XCONTEXT goto abort;

    /* Don't need to modify sp to account for the top of stack being */
    /* in a register because push has already put tos on the stack */
    /* before the argument ( sp ) is evaluated */

    case SPFETCH:    push ( sp );	      next;
    case SPSTORE:    scr = pop;	 sp = (cell *)scr;  (void)pop;  next;
    case RPFETCH:    push ( rp );	      next;
    case RPSTORE:    rp = (token_t **)pop;    next;
    case UPFETCH:    push ( up );	      next;
    case UPSTORE:    up = (cell *)pop;      next;
    case TICK_WORD:  push ( &wordbuf[0] );    next;
    case DIVIDE_MOD: scr = *sp; *sp = scr%tos;
		     if (((scr < 0) ^ (tos < 0))  &&  *sp != 0) {
			    *sp += tos;
			    tos = (scr/tos) - 1;
			    next;
		     }
		     tos = scr/tos; next;

    case DNEGATE:   tos = ~tos + ((*sp = -*sp) == 0);	/* 2's complement */
		    next;

    case DMINUS:
/* Borrow calculation assumes 2's complement arithmetic */
#define BORROW(a,b)  ((u_cell)a < (u_cell)b)

#define al scr
#define bl tos
		    {
			cell ah, bh;
                        bh  = tos;      bl  = *sp++;
                        ah  = *sp++;    al  = *sp;
			*sp = al - bl;  tos = ah - bh - BORROW(al, bl);
		    }
#undef al
#undef bl
#undef BORROW
		    next;
    case DPLUS:

/* Carry calculation assumes 2's complement arithmetic. */
#define CARRY(res,b)  ((u_cell)res < (u_cell)b)
#define al scr
#define bl tos
		    {
			cell ah, bh;

			bh  = tos;      bl  = *sp++;
			ah  = *sp++;    al  = *sp;
			*sp = al += bl;  tos = ah + bh + CARRY(al, bl);
		    }
#undef al
#undef bl
#undef CARRY
		    next;

    case U_M_DIVIDE_MOD:
		     lscr = (long)*++sp;	/* Ignore upper half */
       		     *sp  = (cell)((unsigned long)lscr % (u_cell)tos);
		     tos  = (long)((unsigned long)lscr / (u_cell)tos);
		     next;

    case DIGIT:	     if ( (scr = digit ( tos, (u_char)*sp )) >= 0 ) {
			*sp = scr; tos = -1;
		     } else
			tos = 0;
		     next;
    case NUM_QUESTION: {
	cell scr1;
	cell rtnval;
	scr = pop;
	SAVE_XCONTEXT
	rtnval = number ( (u_char *) scr, (long *)&scr1 );
	RESTORE_XCONTEXT
	push(scr1);
	push(rtnval ? -1 : 0);
    } next;
    case HIDE: SAVE_XCONTEXT hide(); RESTORE_XCONTEXT next;
    case REVEAL: SAVE_XCONTEXT reveal(); RESTORE_XCONTEXT next;
    case INPUT_FILE_NAME: push (input_name); next;
    case QUOTE_LOAD: ascr = apop; SAVE_XCONTEXT load(ascr);	RESTORE_XCONTEXT next;
    case FLOAD: SAVE_XCONTEXT load(blword());  RESTORE_XCONTEXT next;
    case SEMI_S:       load_pop();  next;
    case ORIGIN:       push( origin );	next;
    case CANONICAL:    (void)canonical((u_char *)tos); next;
    case MAXDP:        push( dict_end );next;
    case MAXPRIMITIVE: push( MAXPRIM ); next;
    case PER_N:        push( sizeof(cell) ); next;
    case THIRTY_TWO_BACKSLASH:
			next;
    case SIXTEEN_BACKSLASH:
			if (V_DELIMITER != '\n') {
				SAVE_XCONTEXT
				(void)word('\n'); 
				RESTORE_XCONTEXT
			} next;
    case SYSCALL:  scr = pop; ascr1 = (u_char *)sp;
		   tos = dosyscall (scr, (u_char *)tos, (cell **)&ascr1);
		   sp = (cell *)ascr1; next;	/* Pointer alignment */

    case CCALL:    scr = pop; ascr1 = (u_char *)sp;
		   tos = doccall (scr, (u_char *)tos, (cell **)&ascr1);
		   sp = (cell *)ascr1; next;	/* Pointer alignment */

    case COMMAND:  ascr = apop; ascr = (u_char *)tocstr(ascr);
		   push(system((char *)ascr));
		   next;
    case CHDIR:	   ascr = apop; ascr = (u_char *)tocstr(ascr);
		   push(chdir((char *)ascr));
		   seterror;
		   next;
    case ERRNO:		push( V_ERRNO ); next;	/* Self fetching */
	case WHY:		strcpy(errMsg, _sys_errlist[errno]); error(errMsg); next;
    case BL:		push(' '); next;
    case SINDEX:    /* adr1 len1 adr2 len2 -- n */
		    /* len2 in tos */  ascr  = (u_char *)*sp++;
		    scr = *sp++;       ascr1 = (u_char *)*sp++;
		    tos = strindex(ascr1, scr, ascr, tos);
		    next;
/*
 * Why are FOPEN and FCLOSE outside the NOFILEIO conditional compilation? -MdG
 * (Maybe it's to support stdio open and close of stdin, stdout, and stderr)
 */
	/* ( pstring mode -- fd ) */
    case FOPEN:	scr = pop;
		switch((int)scr) {
		    case 0: ascr1 = (u_char *)READ_MODE; break;
		    case 1: ascr1 = (u_char *)WRITE_MODE; break;
		    case 2: ascr1 = (u_char *)MODIFY_MODE; break;
		    default: ascr1 = (u_char *)scr; break;
		}
		ascr = (u_char *)tocstr((u_char *)tos);
		tos = (cell) fopen((char *)ascr, (char *)ascr1); next;
	/* ( fd -- ) */
    case FCLOSE:file = fpop; (void)fclose(file); next;
#ifdef NOFILEIO
	case FGETC:
    case FGETS:	
    case FPUTC:	
    case FPUTS: file = fpop;  scr = pop;  ascr1 = apop;
		(void)fwrite((char *)ascr1, 1, scr, file);
		next;
    case FGETLINE: 
    case FCR:	
    case FSEEK: 
    case FTELL: 
    case FSIZE: 
    case FFLUSH:  
    case FUNGETC: 
		error("File I/O support is not enabled\n");
		next;
#else
	/* ( fd -- char TRUE )
	 * ( fd -- FALSE )			FALSE if EOF 
	 */
	case FGETC:
		tos = fgetc( (FILE *)tos );
		if (tos >= 0) {
			push( (cell)-1 );
			next;
		}
		tos = 0;
		next;
	/* ( addr len file -- actual ) */
    case FGETS:	file = fpop; scr = pop;
		tos = pfgets((u_char *)tos, scr, file);
		next;
	/* ( char fd -- ) */
    case FPUTC:	file = fpop;  scr = pop;  (void)putc((char)scr, file);  next;
	/* ( addr len fd -- ) */
    case FPUTS: file = fpop;  scr = pop;  ascr1 = apop;
		while (scr--)
			(void)putc((char)*ascr1++, file);
		next;
	/* ( addr fd -- fd eofbool ) eofbool=0 if end of file */
	/* Reads a pstring from the file to the address */
    case FGETLINE: file = fpop;  ascr1 = apop;
		   scr = pfgetline((u_char *)ascr1, file);
		   push(file);  push(scr);  next;
	/* ( fd -- ) */
    case FCR:	file = fpop;  (void)fputs(SNEWLINE, file);  next;
	/* ( pos fd -- ) */
    case FSEEK: file = fpop;
		lscr = (long)pop;
		(void)fseek(file, lscr, 0);
		next;
	/* ( fd -- pos ) */
    case FTELL: file = fpop;  lscr = ftell(file);
		push(lscr);
		next;
	/* ( fd -- size ) */
    case FSIZE: file = fpop;
		lscr1 = ftell(file);		/* Save position */
		(void)fseek(file, 0L, 2);	/* End of file */
		lscr = ftell(file);
		(void)fseek(file, lscr1, 0);	/* Return */
		push(lscr);
		next;
	/* ( fd -- ) */
    case FFLUSH:  file = fpop;  (void)fflush(file);  next;
	/* ( char fd -- ) */
    case FUNGETC: file = fpop; scr = pop; (void)ungetc((char)scr, file); next;
#endif /* NOFILEIO */
	/* ( -- newline ) */
    case FNEWLINE: push(CNEWLINE); next;

    case TORELBIT:
#ifdef RELOCATE
		/*
		 * If address is in dictionary, return address in dictionary
		 * relocation map.
		 */
		if (tos >= (cell)origin && 
					tos < ((cell)origin + DICT_SIZE)) {
			scr = (cell)((cell *)tos - origin);
			ascr = &relmap[scr>>3];
			tos = (cell)ascr;
		    push(bit[scr & 7]);
		    next;
		}
		/*
		 * If address is in user area, return address in user area
		 * relocation map.
		 */
		if ((unsigned cell)tos >= (unsigned cell)up &&
				(unsigned cell)tos < ((unsigned cell)up + MAXUSER*sizeof(cell))) {
			scr = (cell)((cell *)tos - up);
			ascr = &urelmap[scr>>3];
			tos = (cell)ascr;
			push(bit[scr & 7]);
			next;
		}
#endif
		/*
		 * Otherwise, return a "safe" address and a 0 bitmask.
		 */
		tos = (cell)nullrelmap;
		push(0);
		next;

	/* Binary Floating Point Operators */
	case FPLUS:	ftos = *fsp++ + ftos;		next;
	case FMINUS:	ftos = *fsp++ - ftos;		next;
	case FTIMES:	ftos = *fsp++ * ftos;		next;
	case FDIVIDE:	ftos = *fsp++ / ftos;		next;

/* On some systems, "fmod()" may need to be replaced by "drem()" */
	case FMOD:	ftos = fmod(*fsp++, ftos);	next;

	/* Unary Floating Point Operators */
	case FNEGATE:	ftos = -ftos;			next;
	case FSIN:	ftos = sin(ftos);		next;
	case FCOS:	ftos = cos(ftos);		next;
	case FTAN:	ftos = tan(ftos);		next;
	case FLOG:	ftos = log10(ftos);		next;
	case FLN:	ftos = log(ftos);		next;
	case FATAN:	ftos = atan(ftos);		next;
	case FATAN2:	ftos = atan2(*fsp++, ftos);	next;
	case FASIN:	ftos = asin(ftos);		next;
	case FACOS:	ftos = acos(ftos);		next;
	case FCEIL:	ftos = ceil(ftos);		next;
	case FCOSH:	ftos = cosh(ftos);		next;
	case FSINH:	ftos = sinh(ftos);		next;
	case TANH:	ftos = tanh(ftos);		next;
	case FSQRT:	ftos = sqrt(ftos);		next;
	case FEXP:	ftos = exp(ftos);		next;
	case FABS:	ftos = fabs(ftos);		next;
	case FFLOOR:	ftos = floor(ftos);		next;
	case FPOW:	ftos = pow(*fsp++, ftos);	next;
	
	/* Floating Point Stack Manipulation */
	case FDUP:	*--fsp = ftos;			next;
	case FDROP:	ftos = *fsp++;			next;
	case FOVER:	*--fsp = ftos; ftos = fsp[1];	next;
	case FSWAP: 	ftemp = ftos; ftos = *fsp; *fsp = ftemp; next;
	case FROT:	ftemp = ftos; ftos = fsp[1];
			fsp[1] = fsp[0]; fsp[0] = ftemp;
			next;
	case FMINROT:	ftemp = ftos; ftos = fsp[0]; 
			fsp[0] = fsp[1]; fsp[1] = ftemp;
			next;
    /* Floating Point operations involving the Forth data stack */
	case FDEPTH:
		push( (cell)((double *)V_FPZERO - fsp) + 1 );
		next;
	case FPSTORE:	/* Set Floating Point Stack Pointer */
		fsp = (double *)pop;
		next;

	    /* Floating Point Memory Access */
	case FSTORE:
		ulongp = (unsigned long *)pop;
		fbits.fb_float = ftos;
		ftos = *fsp++;
		*ulongp++ = fbits.fb_long[0];
		*ulongp++ = fbits.fb_long[1];
		next;
	case FFETCH:
		ulongp = (unsigned long *)pop;
		*--fsp = ftos;
		fbits.fb_long[0] = *ulongp++;
		fbits.fb_long[1] = *ulongp++;
		ftos = fbits.fb_float;
		next;

	/* Move numbers between Floating Point and Integer Stacks */
	case FINT:	/* Float to Integer */
		if (ftos < (-TWO_31ST)) {
			push((-TWO_31ST));
		} else if (ftos > (TWO_31ST - 1.0)) {
			push(TWO_31ST - 1.0);
		} else {
			push( ftos );
		}
		ftos = *fsp++;
		next;
	case FFLOAT:	/* Integer to Float */
		*--fsp = ftos;
		ftos = (double)pop;
		next;
	case FPOP:	/* Move unconverted bits from FP stack to Int stack */
		ulongp = (unsigned long *)sp;
		ulongp -= 2;
		sp = (cell *)ulongp;
		fbits.fb_float = ftos;
		ulongp[1] = fbits.fb_long[1];
		ulongp[0] = fbits.fb_long[0];
		ftos = *fsp++;
		next;
	case FPUSH:	/* Move unconverted bits from Int stack to FP stack */
		ulongp = (unsigned long *)sp;
		*--fsp = ftos;
		fbits.fb_long[1] = ulongp[1];
		fbits.fb_long[0] = ulongp[0];
		sp = (cell *)(ulongp+2);
		ftos = fbits.fb_float;
		next;

	/* Floating Point Input and Output */
	/* Implement F. as FSTRING TYPE , E. as ESTRING TYPE */
	case FSTRING:
		(void) sprintf(floatstr, "%.*f", (int)V_FNUMPLACES, ftos); 
		push( floatstr );
		push( strlen(floatstr) );
		ftos = *fsp++;
		next;
	case ESTRING:
		(void) sprintf(floatstr, "%.*e", (int)V_FNUMPLACES, ftos);
		push( floatstr );
		push( strlen(floatstr) );
		ftos = *fsp++;
		next;

	/* Comparisons */
#define flag(boolean)	((boolean) ? -1 : 0)
#define fbincmp(operator) push ( flag(*fsp++ operator ftos) ); ftos = *fsp++;
#define funcmp(operator) push ( flag(ftos operator 0.0) ); ftos = *fsp++;

	case FEQ:		fbincmp(==);			next;
	case FNEQ:		fbincmp(!=);			next;
	case FLT:		fbincmp(<);			next;
	case FGT:		fbincmp(>);			next;
	case FLEQ:		fbincmp(<=);			next;
	case FGEQ:		fbincmp(>=);			next;

	case FZEQ:		funcmp(==);			next;
	case FZNEQ:		funcmp(!=);			next;
	case FZLT:		funcmp(<);			next;
	case FZGT:		funcmp(>);			next;
	case FZLEQ:		funcmp(<=);			next;
	case FZGEQ:		funcmp(>=);			next;

	case FPICK:
		*--fsp = ftos;
		ftos = fsp[tos]; tos = *sp++;
		next;
	case FNUMQUES:	/* True if string is a valid floating number */
		scr = pop;
		SAVE_XCONTEXT
		scr = (cell)isfloatnum((unsigned char *)scr);
		RESTORE_XCONTEXT
		push(scr);
		next;
	case FNUMBER:	/* Convert string to floating point number */
		*--fsp = ftos;
		ftos = atof(tocstr((unsigned char *)tos));
		tos = *sp++;
		next;
    case FPAREN_LIT:
	ulongp = (unsigned long *)ip;
	fbits.fb_long[0] = *ulongp++;
	fbits.fb_long[1] = *ulongp++;
	*--fsp = ftos;
	ftos = fbits.fb_float;
	ip = (token_t *)ulongp;
	next;
			case FPRESENT: push(-1);  next;
#ifdef REMOVE
			/* #locals -- */
			case ALLOCLOC:
				ascr = (u_char *)rp;
				if (tos > 0) {		/* Init locals in ANS Forth order */
					rp -= tos;		/* + *sp++ */
					*--rp = (token_t *)frp;
					*--rp = (token_t *)ascr;	/* Pointer alignment */
					frp = (cell *)(rp+2);
					for (scr = 0; scr < tos; scr++)
						frp[scr] = *sp++;
				} else {		/* Init locals in reverse order */
					rp += tos;		/* + *sp++ */
					*--rp = (token_t *)frp;
					*--rp = (token_t *)ascr;	/* Pointer alignment */
					frp = (cell *)(rp+2);
					for (tos = (-tos); tos; )
						frp[--tos] = *sp++;
				}
				tos = *sp++;
				next;
#else
			/* #flocals #locals -- */
			case ALLOCLOC:
				ascr = (u_char *)rp;
				scr2 = *sp++;		/* #flocals */
				if (tos > 0 || scr2 > 0) { /* Init locals in ANS Forth order */
					if (scr2) {
						rp -= scr2;		/* + *sp++ */
						scr2 /= 2;
						for (scr = 0; scr < scr2; scr++) {
							((double *)rp)[scr] = flpop;
						}
					}
/* FIX: assumes sizeof double is 2 * sizeof cell */
					rp -= tos;
					*--rp = (token_t *)frp;
					*--rp = (token_t *)ascr;	/* Pointer alignment */
					frp = (cell *)(rp+2);
					if (tos) {
						for (scr = 0; scr < tos; scr++) {
							frp[scr] = *sp++;
						}
					}
				} else {		/* Init locals in reverse order */
					if (scr2) {
						rp += scr2;		/* + *sp++ */
						scr2 /= 2;
						for (scr2 = (-scr2); scr2; ) {
							((double *)rp)[--scr2] = flpop;
						}
					}
/* FIX: assumes sizeof double is 2 * sizeof cell */
					rp += tos;
					*--rp = (token_t *)frp;
					*--rp = (token_t *)ascr;	/* Pointer alignment */
					frp = (cell *)(rp+2);
					if (tos) {
						for (tos = (-tos); tos; ) {
							frp[--tos] = *sp++;
						}
					}
				}
				tos = *sp++;
				next;
#endif
			/* -- */
			case FREELOC:
				frp = (cell *)frp[-1];
				rp = *(token_t ***)rp;
				next;
			/* offset -- value */
			case GETLOC:
				tos = frp[tos];
				next;
			/* value offset -- */
			case SETLOC:
				frp[tos] = *sp++;
				tos = *sp++;
				next;
			/* name-addr name-len data code -- */
			case LOCNAME:
				locnames[nlocnames].code = (token_t)tos;
				locnames[nlocnames].data = *sp++;
				locnames[nlocnames].name[0] = (u_char)*sp++;
				cmove((u_char *)*sp++, (u_char *)(&locnames[nlocnames].name[1]),
					(cell)(locnames[nlocnames].name[0]));
				++nlocnames;
				tos = pop;
				next;
			/* -- */
			case FREENAMES:
				nlocnames = 0;
				next;
    		case DOLOCNAME:
				push (locnames[locnum].data);
	        	token = (token_t *)locnames[locnum].code;
				goto execute;
#if 1
			case PAUSE:	/* ( -- ) Curr task relinquishes control of CPU */
pause:			if (multitasking) {
			    	SAVE_TASKCONTEXT
					up = (cell *) V_LINK;
					SAVE_XCONTEXT MemeOften(); RESTORE_XCONTEXT
task_dispatcher:	while (!V_TASK_ACTIVE) {
						up = (cell *) V_LINK;
						SAVE_XCONTEXT MemeOften(); RESTORE_XCONTEXT
					}
		    		RESTORE_TASKCONTEXT
					next;
				}
				SAVE_XCONTEXT MemeOften(); RESTORE_XCONTEXT
				next;
#else
			case PAUSE:
pause:			SAVE_XCONTEXT
				MemeOften();
				if (multitasking && xup != (cell *) XV_LINK) {
					SAVE_XTASKCONTEXT
					do {
						xup = (cell *) XV_LINK;
task_disp1:				if (XV_TASK_ACTIVE) break;
						MemeOften();
					} while (xup != (cell *) XV_LINK) ;
					RESTORE_XTASKCONTEXT
				}
				RESTORE_XCONTEXT
				next;
task_dispatcher:
				SAVE_XCONTEXT
				SAVE_XTASKCONTEXT
				goto task_disp1;
#endif

   case SINGLE:			/* -- */
		/* disable multitasking */
		multitasking = 0;
		V_TASK_ACTIVE = -1;
		next;
		
   case MULTI:			/* -- */
		/* enable multitasking */
		multitasking = -1;
		next;

   case ALLOC_MEM:		/* size -- addr */
		/* allocate a block of memory */
		{
		    unsigned size = pop; 
		    push((cell *) malloc(size));
		    next;
		}
   
   case FREE_MEM:		/* addr -- */
		/* release a previously allocated block of memory */
		{
		    char *cp = (char *) pop;
		    free(cp);
		    next;
		}
    case TOMAINTASK:
		/* Pass control to the main task directly.  This is
		 * used by a task that has terminated to pass control
		 * for the last time.
		 * Note that context is not saved.
		 * If the current task is maintask, this code is a no-op.
		 */
		if (up != (cell *)main_task) {
			up = (cell *)main_task;	
			while (!V_TASK_ACTIVE) {
				up = (cell *) V_LINK;
			}
			RESTORE_TASKCONTEXT
		} next;
    case MAINTASK:
		push(main_task);
		next;
    case NFRAMES:
		push(&NumberFrames);
		next;
    case MAXCODEFIELD:
		push(MAXCF - 1);
		next;
    case DEBUGGERDATA:
		push(&DebuggerData);
		next;
    case IFPAREN_LIT: {
		cell exponent, mantissa;
		*(ip-1) = (token_t)FPAREN_LIT;
		exponent = *(cell *)ip;
		mantissa = *((cell *)ip + 1);
		*--fsp = ftos;
		IFloat2Float(exponent, mantissa, &ftemp);
		*(double *)ip = ftos = ftemp;
		ip += 2;
	} next;
	/* mantissa exponent -- */
	/* [float] -- f */
    case IF2F:	scr = pop;		/* exponent */
		IFloat2Float(scr, tos, &ftemp);
		flpush(ftemp); tos = *sp++;
		next;
	/* -- mantissa exponent */
	/* [float] f -- */
    case F2IF:	ftemp = flpop;
		*--sp = tos; --sp;
		Float2IFloat(ftemp, &scr2, sp);
		tos = scr2;
		next;
    case SETRELOCBIT:
		SAVE_XCONTEXT
		set_relocation_bit((cell *)*xsp);
		RESTORE_XCONTEXT
		next;
		
	case OBJECT:		scr = pop; push((cell)&Objects[scr]);	next;
	case RENDER:		Render(); 				next;
	case NUMOBJECTS:	push( NumberObjects() );		next;
	/* ( parent linktype x y width height -- objaddr ) */
	case NEWCAMERA: { 
		Object *obj, *parent;
		cell linktype, subtype, height, width, x, y; 
		height = pop; width = pop; y = pop; x = pop;
		subtype = pop; linktype = pop; parent = (Object *)pop;
		obj = NewCameraObject(parent, linktype, subtype, x, y, width, height);
		push( (cell) obj); next;
	}

	/* ( parent linktype subtype -- objaddr ) */
	case NEWLIGHT: {
		Object *parent, *obj;
		cell linktype, subtype;
		subtype = pop; linktype = pop; parent = (Object *)pop;
		obj = NewLightObject(parent, linktype, subtype);
		push(obj); next;
	}

	/* ( parent linktype subtype filenameaddr -- objaddr ) */
	case NEWVISIBLE: {
		Object *obj, *parent;
		char *filename;
		cell linktype, subtype;
		RFRequest *rq;

		filename = (char *)pop;

		/* If filename is null, make an object with no geometry */
		if (filename == NULL) {
			subtype = pop;
			linktype = pop;
			parent = (Object *)apop;
			obj = NewVisibleObject(parent, linktype, subtype, 0);
			push(obj);
			next;
		}
		if (filename != (char *)-1) {	/* First time, try to open URL */
			rq = malloc(sizeof(RFRequest));
			if (!rq) {
				sp += 3; tos = 0;
				next;
			}
			OpenURL(filename, rq, rfrqFILE);
		} else {						/* Not first time, we're idling */
			rq = (RFRequest *)pop;
			WaitURL(rq);
		}
		if (rq->status == 0) {		/* File opened */
			filename = rq->tmpname;
			subtype = pop;
			linktype = pop;
			parent = (Object *)pop;
			SAVE_XCONTEXT
			obj = NewVisibleObject(parent, linktype, subtype, filename);
#ifdef REMOVE
			/* Don't remove temporary files anymore, because of caching */
			ascr = strrchr(filename, '.');
			if (ascr && !strcmp(ascr, ".tmp")) unlink(filename);
#endif
			RESTORE_XCONTEXT
			push(obj);
			free(rq);
			next;
		}
		if (rq->status == 1) {		/* Request failed */
			strcpy(errMsg, "Couldn't read URL ");
			if (filename == (char *)-1) {
				filename = rq->urlname;
			}
			strcat(errMsg, filename);
			strcat(errMsg, "\n");
			error(errMsg);
			sp += 2; tos = 0;
			free(rq);
			next;
		}
		if (rq->status == -1) {		/* Request pending */
			push(rq);
			push((cell)-1);
			--ip;
			if (*ip == EXECUTE) {
				push(NEWVISIBLE);
			}
			goto pause;
		}
	}
	case NEWAUDIBLE:
		error("NewAudible: Unimplemented Meme operation\n"); next;
	/* ( parent linktype -- objaddr ) */
	case NEWIMAGINARY: {
		Object *obj, *parent;
		cell linktype;
		linktype = pop; parent = (Object *)pop;
		obj = NewImaginaryObject(parent, linktype);
		push(obj); next;
	}
	case CURRCAMERA:
		ascr = apop;
		MemeCurrentCamera((Object *)ascr);
		next;
	case MOUSEX: push(MemeMouseX()); next;
	case MOUSEY: push(MemeMouseY()); next;
	case MOUSEB: push(MemeMouseB()); next;
			/* objaddr destnodeaddr resultaddr -- err */
			case TRANSFORM: {
				Object *objp;
				objp = (Object *)apop;
				ascr1 = apop;
				ascr = apop;
				SAVE_XCONTEXT;
				scr = Transform((Object *)ascr, (Object *)ascr1, objp);
				RESTORE_XCONTEXT;
				push(scr);
			} next;
			/* [float] -- 0.0 */
			case FZERO:
				flpush(0.0); next;
	case SETCAMERABACKCOLOR: {
		Object	*obj;
		double r, g, b;
		b = flpop; g = flpop; r = flpop;
		obj = (Object *)pop;
		MemeSetCameraBackColor(obj, r, g, b);
	} next;
	case SETCAMERAVIEWWINDOW: {
		Object *obj;
		double w, h;
		h = flpop; w = flpop; obj = (Object *)pop;
		MemeSetCameraViewWindow(obj, w, h);
	} next;
	case GETTIME: {
		struct timeb t;
		ftime(&t);
		push((cell) t.time);
		push((cell) t.millitm);
	} next;

	case ORPHAN:
		ascr =  apop;
		scr = Orphan((Object *)ascr);
		if (scr) {	V_ERRNO = scr;	push(-1);	next;	} else {	push(0);	next;	}
		next;

	case ADOPTS:
		ascr = apop; /* child */ ascr1 = apop; /* parent */
		scr = Adopts((Object *)ascr1, (Object *)ascr);
		if (scr) { V_ERRNO = scr;	push(-1);	next;	} else {	push(0); 	next;	}  

	case SCRHEIGHT: push(MemeScreenHeight()); next;
	case SCRWIDTH: push(MemeScreenWidth()); next;
	case MATERIALOPACITY: {
		double opacity;
		ascr = apop;
		opacity = flpop;
		MatOpacity(ascr, opacity);
	} next;
	case JOYSTICKX: push(MemeJoystickX()); next;
	case JOYSTICKY: push(MemeJoystickY()); next;
	case JOYSTICKB: push(MemeJoystickB()); next;
	case DESTROYOBJECT: ascr = apop; DestroyObject((Object *)ascr); next;

	    /* Return the vertex, polygon, and clump that are pointed
	     * to by screenx and screeny.  If clump = 0
	     * then nothing was found.
	     * If nothing found:
	     * ( screenx screeny pickrecordptr -- 0 )
	     * If a vertex was found:
	     * ( screenx screeny pickrecordptr -- -1 )
	     */
	case PICKOBJECT: {
		cell x, y;
		MemePickRecord *mrecp = (MemePickRecord *)apop;
		y = pop;
		x = pop;
		MemePickObject(mrecp, x, y);
		if (mrecp->clump == (void *)0) {
			push(0);
		} else {
			push(-1);
		}
	} next;

	/* Set the vertex whose object address and vertex index
	 * are given, to the xyz triplet on the float stack.
	 * ( vindex objaddr -- )
	 * ( [float] x y z -- )
	 */
	case SETVERTEX: {
	    Object *obj;
	    cell vindex;
		double x, y, z;
	    obj = (Object *) apop; vindex = pop;
	    z = flpop; y = flpop; x = flpop;
	    MemeSetVertex(obj, vindex, x, y, z);
	} next ;
	/* Get the number of polygons in the visible object.
	 * ( objaddr -- #polys )
	 */
	 case POLYGONCOUNT:
		ascr = apop; push( MemePolygonCount((Object *)ascr) ); next; 
	
	 /* Create a texture for mapping onto a polygon or clump
	  * ( filenameaddr -- textureptr )
	  */
		case NEWTEXTURE: {
			cell tp;
			char *filename;
			RFRequest *rq;

			filename = (char *)apop;
			if (filename != (char *)-1) {	/* First time, try to open URL */
				rq = malloc(sizeof(RFRequest));
				if (!rq) {
					push(0);
					next;
				}
				OpenURL(filename, rq, rfrqFILE);
			} else {						/* We're idling */
				rq = (RFRequest *)pop;
				WaitURL(rq);
			}
			if (rq->status == 0) {		/* File opened */
				filename = rq->tmpname;
				SAVE_XCONTEXT
				tp = MemeNewTexture(filename);
#ifdef REMOVE
				/* Don't remove temporary files anymore, because of caching */
				ascr = strrchr(filename, '.');
				if (ascr && !strcmp(ascr, ".tmp")) unlink(filename);
#endif
				RESTORE_XCONTEXT
				push(tp);
				free(rq);
				next;
			}
			if (rq->status == 1) {		/* Request failed */
				strcpy(errMsg, "Couldn't read texture file ");
				if (filename == (char *)-1) {
					filename = rq->urlname;
				}
				strcat(errMsg, filename);
				strcat(errMsg, "\n");
				error(errMsg);
				push(0);
				free(rq);
				next;
			}
			if (rq->status == -1) {		/* Request pending */
				push(rq);
				push((cell)-1);
				--ip;
				if (*ip == EXECUTE) {
					push(NEWTEXTURE);
				}
				goto pause;
			}
		}

		/* -- 0 */
		case PUSH_FALSE:	push(0); next;

	    /* Set the frame step for the texture frames
	     * ( step textureptr -- )
	     */
	    case TEXTUREFRAMESTEP:
		ascr = apop;
		scr = pop;
		MemeTextureFrameStep(ascr, scr);
		next;

	    /* Move to the next frame in the texture
	     * ( textureptr -- )
	     */
	    case TEXTURENEXTFRAME:
		ascr = apop;
		MemeTextureNextFrame(ascr);
		next;

	    /* Set the texture frame #
	     * ( frame# textureptr -- )
	     */
	    case SETTEXTUREFRAME:
		ascr = apop;
		scr = pop;
		MemeSetTextureFrame(ascr, scr);
		next;

	    /* Create a new material.
	     * ( -- materialptr )
	     */
	    case NEWMATERIAL: push(NewMat()); next;

	    /* Destroy a material.
	     * ( materialptr -- )
	     */
	    case DESTROYMATERIAL:
		ascr = apop;
		DestroyMat(ascr);
		next;

	    /* Set the material's color.
	     * ( materialptr -- )
	     * ( [float] Kr Kg Kb -- )
	     */
	    case MATERIALCOLOR: {
		double r, g, b;
		b = flpop;
		g = flpop;
		r = flpop;
		ascr = apop;
		MatColor(ascr, r, g, b);
	    } next;

	    /* Set the material's surface
	     * ( materialptr -- )
	     * ( [float] Ka Kd Ks power -- )
	     */
	    case MATERIALSURFACE: {
		double a, d, s, power;
		power = flpop; s = flpop; d = flpop; a = flpop;
		ascr = apop;
		MatSurface(ascr, a, d, s, power);
	    } next;

	    /* Set the material's shading.
	     * ( shadingtype materialptr -- )
	     */
	    case MATERIALSHADING:
		ascr = apop; scr = pop;
		MatShading(ascr, scr);
		next;

	    /* Set the material's geometry type.
	     * ( geometrytype materialptr -- )
	     */
	    case MATERIALGEOMETRY:
		ascr = apop; scr = pop;
		MatGeometry(ascr, scr);
		next;

	    /* Set the material's texture.
	     * ( textureptr materialptr -- )
	     */
	    case MATERIALTEXTURE:
		ascr = apop; ascr1 = apop;
		MatTexture(ascr, ascr1);
		next;

	    /* Set a polygon's material.
	     * ( materialptr polygonptr -- )
	     */
	    case POLYGONMATERIAL:
		ascr = apop; ascr1 = apop;
		MemePolygonMaterial(ascr, (mMat *)ascr1);
		next;

	    /* Set the shape's material.
	     * ( materialptr objaddr -- )
	     */
	    case SHAPEMATERIAL:
		ascr = apop; ascr1 = apop;
		MemeShapeMaterial(ascr, ascr1);
		next;

	    /* Write the shape to a file.
	     * ( objaddr filenameaddr filenamelen -- )
	     */
	    case WRITESHAPE:
		scr = pop;	/* Discard the filenamelen */
		ascr1 = apop;
		ascr = apop;
		MemeWriteShape(ascr, ascr1);
		next;
		/* hostid -- bool */
		case ISMYHOSTID: scr = pop; SAVE_XCONTEXT
							scr = IsMyHostID(scr);
						RESTORE_XCONTEXT push(scr); next;
		/* GETTIMEOFDAY accepts a pointer to storage, in
		 * which it puts the time of day structure.
		 */
		/* addr -- */
	    case GETTIMEOFDAY: {
			time_t theTime;
			ascr = apop;
			theTime = time(NULL);
			memcpy(ascr, localtime(&theTime), sizeof(struct tm));
		} next;
	    case DESTROYTEXTURE:
			ascr = apop;
			MemeDestroyTexture(ascr);
			next; 
	    /* Get the number of vertices in a clump
	     * ( objaddr -- num )
	     */
	    case GETVERTEXCOUNT:
		ascr = apop;
		push(MemeGetVertexCount((Object *)ascr));
		next;

	    /* Get a clump's vertex
	     * ( index objaddr -- )
	     * ( [float] -- x y z )
	     */
	    case GETVERTEX: {
		double x, y, z;
		ascr = apop;
		scr = pop;
		MemeGetVertex((Object *)ascr, scr, &x, &y, &z);
		flpush(x);
		flpush(y);
		flpush(z);
	    } next;
	    /* Create a new module object
	     * Filename is a dummy.  NewCompound is
	     * redefined in module.m.
	     * ( parent linktype filename -- objaddr )
	     */
	    case NEWCOMPOUND: {
		Object *obj, *parent;
		cell linktype;
		char *filename;
		filename = apop;
		linktype = pop; parent = (Object *) apop;
		obj = NewCompoundObject(parent, linktype);
		push(obj); next;
	    } next;
	    /* Find the Meme object that has the input argument
	     * in its ptr field.  Used for finding the object addr
	     * of a compound object, given its module addr.  Also used
		 * for finding the object given a clump pointer.
	     */
	    case WHICHOBJECT: tos = (cell)MemeWhichObject((void *)tos); next;
		/* -- -1 */
		case PUSH_TRUE:	push(-1); next;
	    /* size -- addr */
	    case NEWMODULE:
			tos = (cell)NewModule(tos); next;
	    /* addr -- */
	    case DESTROYMODULE:
			ascr = apop; DestroyModule((void *)ascr); next;
		/* msgptr -- result */
		/* result is 0 if no error, 1 if error */
	    case SENDMESSAGE: {
				time_t timeout;
				ascr = apop;
				if (ascr != (void *)(0xffffffff)) {	/* If 1st time */
					timeout = time((time_t)0) + SENDMESSAGETIMEOUT;
				} else {
					ascr = apop;
					timeout = pop;
				}
				SAVE_XCONTEXT
				scr = SendMemeMessage((void *)ascr, (cell)0);
				RESTORE_XCONTEXT
				/* If the connection is pending, "idle" this task by
				 * decrementing IP to point at the token for SENDMESSAGE,
				 * and pushing the message address back on the int stack.
				 * If SENDMESSAGE was called by EXECUTE, push the token
				 * for SENDMESSAGE on the int stack as well.
				 */
				if (scr == -1) {
					if (timeout > time((time_t)0)) {	/* Not timed out yet */
						push(timeout);
						push(ascr);
						push(-1);
						--ip;
						if (*ip == EXECUTE) {
							push(SENDMESSAGE); /* Keep braces; push is macro */
						}
						goto pause;
					} else {				/* Timed out */
						scr = 1;			/* Indicate failure */
						V_ERRNO = ESENDMSGTIMEDOUT;
					}
				}
				push(scr);
			} next;
		/* msgptr -- */
		/* Never returns */
	    case SENDFINALMESSAGE: {
				time_t timeout;
				ascr = apop;
				if (ascr != (void *)(0xffffffff)) {	/* If 1st time */
					timeout = time((time_t)0) + SENDMESSAGETIMEOUT;
				} else {
					ascr = apop;
					timeout = pop;
				}
				V_TASK_ACTIVE = 0;
				SAVE_XCONTEXT
				scr = SendMemeMessage((void *)ascr, (cell)0);
				RESTORE_XCONTEXT
				/* If the connection is pending, "idle" this task by
				 * decrementing IP to point at the token for SENDMESSAGE,
				 * and pushing the message address back on the int stack.
				 * If SENDMESSAGE was called by EXECUTE, push the token
				 * for SENDMESSAGE on the int stack as well.
				 */
				if (scr == -1) {
					if (timeout > time((time_t)0)) {	/* Not timed out yet */
						V_TASK_ACTIVE = -1;
						push(timeout);
						push(ascr);
						push(-1);
						--ip;
						if (*ip == EXECUTE) {
							push(SENDFINALMESSAGE); /* Braces for push macro */
						}
						goto pause;
					}
				}
			} 
			/* Switch context directly to the main task */
			if (up != (cell *)main_task) {
				up = (cell *)main_task;
				goto task_dispatcher;
			}
			next;
		case REGISTERDECKMODULE:
			ascr = apop;
			SAVE_XCONTEXT
			scr = RegisterDeckModule( (module *)ascr );
			RESTORE_XCONTEXT
			push(scr);
			next;
#ifdef	CMAN
			case CYBERMAN: *--sp = tos; tos = (cell)CyberMan(); next;
#else /* CMAN */
			case CYBERMAN:
				error("CyberMan support is unimplemented in this version.\n");
				next;
#endif /* CMAN */
			/* -- extended-err-code */
			case GETLASTERROR:	push(V_ERRNO);	next;
			/* comport baudrate parity databits stopbits -- portdesc */
			case SER_OPEN: {
				cell	comport;		/* 0 = COM1, 1 = COM2, etc.	*/
				cell	baudrate;		/* 9600 = 9600 baud, etc.	*/
				cell	parity;			/* 'N', 'E', or 'O'			*/
				cell	word_length;	/* 7 or 8					*/
				cell	stop_bits;		/* 1 or 2					*/
				stop_bits = pop;
				word_length = pop;
				parity = pop;
				baudrate = pop;
				comport = pop;
				push(SerialOpen(comport, baudrate, parity,
											word_length, stop_bits));
			} next;
			/* portdesc -- errflag */
			case SER_CLOSE: {
				cell portdesc;
				portdesc = pop;
				push(SerialClose(portdesc));
			} next;
			/* portdesc buffer length -- actual */
			case SER_READ: {
				cell portdesc;
				cell buff;
				cell len;
				len = pop;  buff = pop; portdesc = pop;
				push(SerialRead(portdesc, buff, len));
			} next;
			/* portdesc buffer length -- actual */
			case SER_WRITE: {
				cell portdesc;
				cell buff;
				cell len;
				len = pop; buff = pop; portdesc = pop;
				push(SerialWrite(portdesc, buff, len));
			} next;
	    	/* Open an active connection to the network. sockdesc
			 * = -1 if failure.
	    	 * ( ipnumber port timeout -- sockdesc )
	    	 */
	    	case NET_CONNECT: {
				unsigned long ipnumber;
				cell port;
				cell timeout;
				timeout = pop;
				port = pop;
				ipnumber = (unsigned long)pop;
				SAVE_XCONTEXT
				scr = NetConnect(ipnumber, port, timeout);
				RESTORE_XCONTEXT
				push(scr);
	    	} next;
		
	   		/* Open a passive connection to the network.  sockdesc
			 * = -1 if failure.
	    	 * ( port timeout -- sockdesc )
	    	 */
	    	case NET_LISTEN: {
				cell port, timeout;
				timeout = pop; port = pop; 
				SAVE_XCONTEXT
				scr = NetListen(port, timeout);
				RESTORE_XCONTEXT
				push(scr);
	    	} next;

	    	/* Read data from the network
	    	 * ( sockdesc buffer len -- actual )
	    	 */
	    	case NET_READ:
				scr = pop; ascr = apop; ascr1 = apop;
				SAVE_XCONTEXT
				scr = NetRead(ascr1, ascr, scr);
				RESTORE_XCONTEXT
				push(scr);
				next;
	    	/* Write data to the network
	    	 * ( sockdesc buffer len -- actual )
	    	 */
	    	case NET_WRITE:
				scr = pop; ascr = apop; ascr1 = apop;
				SAVE_XCONTEXT
				scr = NetWrite(ascr1, ascr, scr);
				RESTORE_XCONTEXT
				push(scr);
				next;
	    	/* Close the network connection
	    	 * ( sockdesc timeout -- code )
			 */
	    	case NET_DISCONNECT:
				scr = pop;
				ascr = apop;
				SAVE_XCONTEXT
				scr = NetDisconnect(ascr, scr);
				RESTORE_XCONTEXT
				push(scr);
				next;
			/* Wait for the network connection to be established
			 * ( sockdesc -- code )
			 */
			case NET_CONNECTED:
				ascr = apop;
				SAVE_XCONTEXT
				scr = NetConnected(ascr);
				RESTORE_XCONTEXT
				push(scr);
				next;
			/* Wait for the network connection to close
			 * ( sockdesc -- code )
			 */
			case NET_DISCONNECTED:
				ascr = apop;
				SAVE_XCONTEXT
				scr = NetDisconnected(ascr);
				RESTORE_XCONTEXT
				push(scr);
				next;
			/* Return the network error code
			 * ( -- n )
			 */
			case NET_ERROR:
				push(V_ERRNO);
				next;
			/* Return the number of bytes waiting on a socket.
			 * ( sockdesc -- #bytes )
			 */
			case NET_STATUS:
				ascr = apop;
				SAVE_XCONTEXT
				scr = NetStatus(ascr);
				RESTORE_XCONTEXT
				push(scr);
				next;
			/* Return a random 32-bit value */
			case RAND: push((cell)myrand()); next;
			case RANDOMIZE: myrandomize(); next;
			case WINDOWHEIGHT:
				push( MemeWindowHeight() );
				next;
			case WINDOWWIDTH:
				push( MemeWindowWidth() );
				next;
			/* ( regionptr prismptr -- regionptr ) */
			case ADDTOREGION:
				ascr1 = apop;
				ascr = apop;
				ascr = (u_char *)AddToRegion((Region *)ascr, (RPrism *)ascr1);
				push(ascr);
				next;
			/* ( regionptr -- ) */
			case DESTROYREGION:
				ascr = apop;
				DestroyRegion((Region *)ascr);
				next;
			/* ( [float] x y z -- ) ( regionptr -- boolean ) */
			case POINTINREGION: {
				double x, y, z;
				z = flpop; y = flpop; x = flpop;
				ascr = apop;
				push((cell)PointInRegion((Region *)ascr, x, y, z));
				next;
			}
			/* ( [float] x y z -- ) ( prismptr -- boolean ) */
			case POINTINPRISM: {
				double x, y, z;
				z = flpop; y = flpop; x = flpop;
				ascr = apop;
				push((cell)PointInPrism((RPrism *)ascr, x, y, z));
				next;
			}
			/* ( prismptr1 prismptr2 -- boolean ) */
			case OVERLAPPINGPRISMS:
				ascr1 = apop;
				ascr = apop;
				push(OverlappingPrisms((RPrism *)ascr, (RPrism *)ascr1));
				next;
			/* ( regionptr1 regionptr2 -- boolean ) */
			case OVERLAPPINGREGIONS:
				ascr1 = apop;
				ascr = apop;
				push(OverlappingRegions((Region *)ascr, (Region *)ascr1));
				next;
			/* ( regionptr -- boolean ) */
			case REGISTERREGION:
				ascr = apop;
				SAVE_XCONTEXT
				ascr = (u_char *)RegisterDBRegion((Region *)ascr);
				RESTORE_XCONTEXT
				if (ascr) { push(-1); } else { push(0); }
				next;
			/* ( regionptr -- boolean ) */
			case UNREGISTERREGION:
				ascr = apop;
				SAVE_XCONTEXT
				ascr = (u_char *)UnregisterDBRegion((Region *)ascr);
				RESTORE_XCONTEXT
				if (ascr) { push(-1); } else { push(0); }
				next;
			/* ( -- ptr ) ( [float] x y z -- ) */
			case ENCLOSINGREGION: {
				double x, y, z;
				z = flpop; y = flpop; x = flpop;
				SAVE_XCONTEXT
				ascr = (u_char *)EnclosingDBRegion(x, y, z);
				RESTORE_XCONTEXT
				push(ascr);
				next;
			}
			/* ( regionptr -- firstregionptr ) */
			case FIRSTOVERLAPPINGREGION:
				ascr = apop;
				SAVE_XCONTEXT
				ascr = (u_char *)FirstOverlappingDBRegion((Region *)ascr);
				RESTORE_XCONTEXT
				push(ascr);
				next;
			/* ( regionptr -- nextregionptr ) */
			case NEXTOVERLAPPINGREGION:
				ascr = apop;
				SAVE_XCONTEXT
				ascr = (u_char *)NextOverlappingDBRegion((Region *)ascr);
				RESTORE_XCONTEXT
				push(ascr);
				next;
			/* ( cstr -- len ) */
			case STRLEN:
				tos = strlen((char *)tos);
				next;
			/* ( src dest -- ) */
			case STRCPY:
				ascr1 = apop;	/* dest */
				ascr = apop;	/* src */
				strcpy(ascr1, ascr);
				next;
			/* ( cstr1 cstr2 -- diff ) */
			case STRCMP:
				ascr = apop;
				ascr1 = apop;
				push(strcmp(ascr, ascr1));
				next;
			/* ( cstr -- n ) */
			case ATOI:
				ascr = apop;
				push(atoi(ascr));
				next;
			/* ( n cstr radix -- ) */
			case ITOA: {
				cell radix;
				char *cstr;
				radix = pop;
				cstr = apop;
				scr = pop;
				(void)itoa((int)scr, cstr, (int)radix);
			} next;
			/* ( cstr -- ) ( [float] -- n ) */
			case ATOF:
				ascr = apop;
				flpush(atof(ascr));
				next;
			/* ( cstr -- ) ( [float] n -- ) */
			case FTOA:
				ascr = apop;
				ftemp = flpop;
				sprintf(ascr, "%f", ftemp);
				next;
			/* ( n1 n2 --- n1 ) If n1 <> n2 */
			/* ( n1 n2 -- ) If n1 = n2 */
			case POF:
				scr = pop;
				if (tos == scr) {
					tos = *sp++;
					/* Pointer alignment */
					ip = (token_t *)(((char *)ip) + sizeof(cell));
					next;
				}
				branch;
				next;
			/* ( addr1 addr2 -- addr1 ) If string1 <> string2 */
			/* ( addr1 addr2 -- ) If string1 = string2 */
			case PSOF:
				scr = pop;
				if (!strcmp((char *)tos, (char *)scr)) {
					tos = *sp++;
					/* Pointer alignment */
					ip = (token_t *)(((char *)ip) + sizeof(cell));
					next;
				}
				branch;
				next;
			/* ("s) -- leave pascal-string addr */
			case PQUOTES:
				push(ip);		/* Push address */
				ip =	aligned((u_char *)ip + *(u_char *)ip + 2);
				next;
			/* (") -- leave pascal-string addr+1 and length */
			case PQUOTE:
				push((cell)ip + 1);		/* Push address */
				push(*(unsigned char *)ip);
#ifdef REMOVE
				ip =	(token_t *)(
						(char *)ip + 			/* Current address */
						( *(unsigned char *)ip + /* Plus length of string */
						+ 2						/* One len byte and one null */
						+ sizeof(cell) & ~(sizeof(cell) - 1) ) );/* align */
#else
				ip =	aligned((u_char *)ip + *(u_char *)ip + 2);
#endif
				next;
			/* (c") -- leave C-string addr */
			case PCQUOTE:
				push((cell)ip + 1);				/* Push address */
#ifdef REMOVE
				ip =	(token_t *)(
						(char *)ip + 			/* Current address */
						( *(unsigned char *)ip + /* Plus length of string */
						+ 2						/* One len byte and one null */
						+ sizeof(cell) & ~(sizeof(cell) - 1) ) );/* align */
#else
				ip =	aligned((u_char *)ip + *(u_char *)ip + 2);
#endif
				next;

			/* OPENURLASFILE opens a file whose location is described by the URL
			 * that cstrURL points to.  cstrURL is a pointer to a cstring containing
			 * the URL.  fnamestrbuffptr is a pointer to an array that can hold the
			 * cstring pathname of the local copy of the file, which is created in
			 * MEMETMP directory.  If the file can't be opened, fnamestrbuff contains
			 * a null string.
			 * If the URL specifies a remote host and the network interface is disabled,
			 * the call always fails.
			 */
			/* cstrURL fnamestrbuffptr -- */
			case OPENURLASFILE: {
				char *filename;
				char *destname;
				RFRequest *rq;
				destname = (char *)pop;
				
				if (destname != (char *)-1) { /* First time, try to open URL */
					filename = (char *)pop;
					rq = malloc(sizeof(RFRequest));
					if (!rq) {
						*destname = '\0';
						next;
					}
					OpenURL(filename, rq, rfrqFILE);
				} else {						/* We're idling */
					rq = (RFRequest *)pop;
					destname = (char *)pop;
					WaitURL(rq);
				}
				if (rq->status == rfrSUCCESS) {	/* File opened */
					strcpy(destname, rq->tmpname);
					free(rq);
					next;
				}
				if (rq->status == rfrFAILURE) {	/* Request failed */
					*destname = '\0';
					free(rq);
					next;
				}
				if (rq->status == rfrPENDING) {	/* Request pending */
					push(destname);
					push(rq);
					push((cell)-1);
					--ip;
					if (*ip == EXECUTE) {
						push(OPENURLASFILE);
					}
					goto pause;
				}
			}

			/* OPENURLASBUFFER opens a file whose location is described by the URL
			 * that cstrURL points to.  cstrURL is a pointer to a cstring containing
			 * the URL.  msgptr is a pointer to a malloc'd buffer that contains the file.
			 * If the open fails then msgptr is NULL.
			 * If the URL specifies a remote host and the network interface is disabled,
			 * the call always fails.
			 */
			/* cstrURL -- msgptr */
			case OPENURLASBUFFER: {
				char *filename;
				RFRequest *rq;
				filename = (char *)pop;
				
				if (filename != (char *)-1) { /* First time, try to open URL */
					rq = malloc(sizeof(RFRequest));
					if (!rq) {
						push(0);
						next;
					}
					OpenURL(filename, rq, rfrqBUFFER);

				} else {						/* We're idling */
					rq = (RFRequest *)pop;
					WaitURL(rq);
				}
				if (rq->status == rfrSUCCESS) {	/* File opened */
					push(rq->buff);
					free(rq);
					next;
				}
				if (rq->status == rfrFAILURE) {	/* Request failed */
					push(0);
					free(rq);
					next;
				}
				if (rq->status == rfrPENDING) {	/* Request pending */
					push(rq);
					push((cell)-1);
					--ip;
					if (*ip == EXECUTE) {
						push(OPENURLASBUFFER);
					}
					goto pause;
				}
				/* If we get here we have a bogus value for rq->status */
				rq = rq; //  A place to breakpoint
			}
			/* cstr -- */
			case CTYPE:
				ascr = apop; SAVE_XCONTEXT cprint(ascr); RESTORE_XCONTEXT next;
			/* offset -- */
			/* [float] -- value */
			case GETFLOC:
				scr = pop;
				flpush(*(double *)&frp[scr]);
				next;
			/* offset -- */
			/* [float] value -- */
			case SETFLOC:
				scr = pop;
				*(double *)&frp[scr] = flpop;
				next;
			/* wrap materialptr -- */
			case MATERIALWRAP:
				ascr = apop;
				scr = pop;
				MatWrap(ascr, scr);
				next;
			/* objptr -- */
			/* [float] r g b -- */
			case LIGHTCOLOR: {
				double r, g, b;
				b = flpop; g = flpop; r = flpop;
				ascr = apop;
				MemeLightColor((Object *)ascr, r, g, b);
			} next;
			/* objptr -- */
			/* [float] umbra penumbra -- */
			case LIGHTCONEANGLE: {
				double umbra, penumbra;
				penumbra = flpop; umbra = flpop; ascr = apop; 
				MemeLightConeAngle((Object *)ascr, umbra, penumbra);
			} next;
			/* cstringptr -- ip-in-host-byte-order */
			case NETGETHOSTBYNAME:
				ascr = apop;
				push(NetName2HostID(ascr));
				next;
			/* msockptr -- newsockptr<or>statusvalue */
			case NETACCEPTED:
				tos = NetAccepted((void *)tos);
				next;
			/* msockptr flags timeout -- errcode */
			case NETDISPATCH:
				scr = pop;
				scr2 = pop;
				ascr = apop;
				SAVE_XCONTEXT
				scr = NetDispatch(ascr, scr2, scr);
				RESTORE_XCONTEXT
				push(scr);
				next;
			/* -- floatstkptr */
			case FPFETCH:
				push(fsp);
				next;
			/* -- frameptr */
			case FRPFETCH:
				push(frp);
				next;
			/* errnum -- */
			case SETLASTERROR:
				V_ERRNO = pop;
				next;
			/* bufaddr -- */
			case GETCWD:
				ascr = apop;
				if (getcwd((char *)ascr, 256) == NULL) *(char *)ascr = '\0';
				next;
			/* -- addr */
			case MAINTASKRCVR:
				push(&maintaskReceiver);
				next;
			/* cstringtexfilename cstringmaskfilename -- textureptr */
			case NEWMASKEDTEXTURE: {
				char *texname;						/* Texture file name or URL */
				char *maskname;						/* Mask file name or URL */
				char *texlocalname;					/* Texture file name in the cache */
				cell tp;							/* Texture pointer returned from MemeMaskedTexture */
				RFRequest *rq;						/* Remote file request struct */
				cell statenum;						/* State # */

				switch (tos) {
					case (-1):						/* Request for texture file is pending */
						statenum = pop;
						rq = (RFRequest *)apop;
						maskname = (char *)apop;
						WaitURL(rq);
						if (rq->status == 0) {		/* File opened */
							texlocalname = strdup(rq->tmpname);
							push(texlocalname);
							push(maskname);
							push(rq);
							push((cell)-2);
							--ip;
							if (*ip == EXECUTE) {
								push(NEWMASKEDTEXTURE);
							}
							goto pause;
						}
						if (rq->status == 1) {		/* Request failed */
							push(0);
							V_ERRNO = ETEXINVALIDTEXFILE;
							free(rq);
							next;
						}
						if (rq->status == -1) {		/* Request pending */
							push(maskname);
							push(rq);
							push((cell)-1);
							--ip;
							if (*ip == EXECUTE) {
								push(NEWMASKEDTEXTURE);
							}
							goto pause;
						}
					case (-2):						/* Texture file successfully opened--open mask file */
						statenum = pop;
						rq = (RFRequest *)apop;
						maskname = (char *)apop;
						texlocalname = (char *)apop;
						OpenURL(maskname, rq, rfrqFILE);
						if (rq->status == 0) {		/* File opened */
							SAVE_XCONTEXT
							tp = MemeNewMaskedTexture(texlocalname, rq->tmpname);
							RESTORE_XCONTEXT
							push(tp);
							free(texlocalname);
							free(rq);
							next;
						}
						if (rq->status == 1) {		/* Request failed */
							push(0);
							V_ERRNO = ETEXINVALIDMASKFILE;
							free(texlocalname);
							free(rq);
							next;
						}
						if (rq->status == -1) {		/* Request pending */
							push(texlocalname);
							push(maskname);
							push(rq);
							push((cell)-3);
							--ip;
							if (*ip == EXECUTE) {
								push(NEWMASKEDTEXTURE);
							}
							goto pause;
						}
					case (-3):						/* Request for mask file is pending */
						statenum = pop;
						rq = (RFRequest *)apop;
						maskname = (char *)apop;
						texlocalname = (char *)apop;
						WaitURL(rq);
						if (rq->status == 0) {		/* File opened */
							SAVE_XCONTEXT
							tp = MemeNewMaskedTexture(texlocalname, rq->tmpname);
							RESTORE_XCONTEXT
							push(tp);
							free(texlocalname);
							free(rq);
							next;
						}
						if (rq->status == 1) {		/* Request failed */
							push(0);
							V_ERRNO = ETEXINVALIDMASKFILE;
							free(texlocalname);
							free(rq);
							next;
						}
						if (rq->status == -1) {		/* Request pending */
							push(texlocalname);
							push(maskname);
							push(rq);
							push((cell)-3);
							--ip;
							if (*ip == EXECUTE) {
								push(NEWMASKEDTEXTURE);
							}
							goto pause;
						}
					default:						/* First time through--open texture file */
						maskname = (char *)apop;
						texname = (char *)apop;
						rq = malloc(sizeof(RFRequest));
						if (!rq) {
							push(0);
							next;
						}
						OpenURL(texname, rq, rfrqFILE);
						if (rq->status == 0) {		/* File opened */
							texlocalname = strdup(rq->tmpname);
							push(texlocalname);
							push(maskname);
							push(rq);
							push((cell)-2);
							--ip;
							if (*ip == EXECUTE) {
								push(NEWMASKEDTEXTURE);
							}
							goto pause;
						}
						if (rq->status == 1) {		/* Request failed */
							push(0);
							V_ERRNO = ETEXINVALIDTEXFILE;
							free(rq);
							next;
						}
						if (rq->status == -1) {		/* Request pending */
							push(maskname);
							push(rq);
							push((cell)-1);
							--ip;
							if (*ip == EXECUTE) {
								push(NEWMASKEDTEXTURE);
							}
							goto pause;
						}
				} /* switch (tos) */
			} next;		/* Just in case; the "next" shouldn't be necessary */
			/* Set the backdrop image for this camera
			 * ( cstringfilename cameraobjaddr --  )
			 */
			case SETCAMERABACKDROP: {
				Object *obj;
				char *filename;
				RFRequest *rq;

				obj = (Object *) apop;
				filename = (char *)apop;

				/* If the filename ptr is zero, call MemeSetCameraBackdroo
				 * to reset the backdrop to nothing.
				 */
				if (filename == NULL) {
					SAVE_XCONTEXT
					MemeSetCameraBackdrop(obj, filename);
					RESTORE_XCONTEXT
					next;
				}
				if (filename != (char *)-1) {	/* First time, try to open URL */
					rq = malloc(sizeof(RFRequest));
					if (!rq) {
						V_ERRNO = ESCBNOSPACE;
						next;
					}
					OpenURL(filename, rq, rfrqFILE);
				} else {						/* We're idling */
					obj = (Object *)apop;
					rq = (RFRequest *)apop;
					WaitURL(rq);
				}
				if (rq->status == 0) {		/* File opened */
					filename = rq->tmpname;
					SAVE_XCONTEXT
					MemeSetCameraBackdrop(obj, filename);
	#ifdef REMOVE
					/* Don't remove temporary files anymore, because of caching */
					ascr = strrchr(filename, '.');
					if (ascr && !strcmp(ascr, ".tmp")) unlink(filename);
	#endif
					RESTORE_XCONTEXT
					free(rq);
					next;
				}
				if (rq->status == 1) {		/* Request failed */
					V_ERRNO = ESCBINVALIDFILE;
					free(rq);
					next;
				}
				if (rq->status == -1) {		/* Request pending */
					push(rq);
					push(obj);
					push((cell)-1);
					--ip;
					if (*ip == EXECUTE) {
						push(NEWTEXTURE);
					}
					goto pause;
				}
			}
			/* parent linktype textureptr -- objaddr */
			case NEWSPRITE:
				ascr1 = apop;			/* textureptr */
				scr = pop;				/* linktype */
				ascr = apop;			/* parent */
				ascr = (char *)NewSpriteObject( (Object *)ascr, scr, (cell)ascr1 );
				push(ascr);
				next;
			/* index objaddr -- polygonptr */
			case GETPOLYGON:
				ascr = apop;
				scr = pop;
				SAVE_XCONTEXT
				ascr1 = (u_char *)MemeGetPolygon((Object *)ascr, scr);
				RESTORE_XCONTEXT
				push(ascr1);
				next;
			/* x y cameraobjaddr -- */
			case SETCAMERABACKDROPOFFSET: {
				cell x, y;
				ascr = apop; y = pop; x = pop;
				SAVE_XCONTEXT
				MemeSetCameraBackdropOffset((Object *)ascr, x, y);
				RESTORE_XCONTEXT
			} next;
			/* addr1 newsize -- addr2 */
			case REALLOC_MEM:	
				scr = pop;	tos = (cell)realloc((void *)tos, scr);	next;
			/* -- dictsize */
			case DICTIONARYSIZE:	push(DICT_SIZE);	next;
			/* materialp polygonp -- */
			case GETPOLYGONMATERIAL:
				ascr = apop;
				scr = pop;
				MemeGetPolygonMaterial(ascr, (mMat *)scr);
				next;
			/* objaddr -- */
			case EMPTYSHAPE:
				ascr = apop;
				MemeEmptyShape((Object *)ascr);
				next;
			/* objaddr -- vindex */  /* [float] x y z -- */
			case ADDVERTEX: {
				double x, y, z;
				z = flpop; y = flpop; x = flpop;
				ascr = apop;
				push(MemeAddVertex((Object *)ascr, x, y, z));
				next;
			}
			/* vindexlistaddr n objaddr -- polygonptr */
			case ADDPOLYGON: {
				Object *obj;
				cell n;
				void *vindexaddr;
				obj = (Object *)apop; n = pop; vindexaddr = apop;
				push(MemeAddPolygon(obj, vindexaddr, n));
				next;
			}
			/* srcobjaddr destobjaddr -- */
			case ADDSHAPE: {
				Object *srcobj, *destobj;
				destobj = (Object *)apop;
				srcobj = (Object *)apop;
				MemeAddShape(srcobj, destobj);
				next;
			}
			/* polygonptr -- */
			case REVERSEPOLYGONNORMAL: {
				void *polyp = apop;
				MemeReversePolygonNormal(polyp);
				next;
			}
			/* objaddr -- */
			case REVERSESHAPENORMALS: {
				Object *obj = (Object *)apop;
				MemeReverseShapeNormals(obj);
				next;
			}
			/* objaddr textureptr -- */
			case TEXTURECAMERA: {
				cell tex;
				Object *cam;
				tex = pop;
				cam = (Object *)apop;
				MemeTextureCamera(cam, tex);
				next;
			}

			default:	/* Defining memes */ 
				switch (*token++) {
					case DOCOLON:	*--rp = ip; ip = token; next;
					case DOCON:		push ( *(cell *) (token) ); next;
					case DOVAR:		push ( token ); next;
					case DOUSER:	push ( *(cell *) token + (cell) up ); next;
					case DODEFER:	token = (token_t *)*token; goto execute;
					case DOVOC:		V_CONTEXT = (cell)(token-1); next;
#ifdef REMOVE
					case DOCODE:
# ifndef lint
						(*(void (*) ())token)(); next;
# endif
#endif
					default:	       /* DOES> meme */
						/* Push parameter field address */
						push ( token );
	   					/* Use the code field as the address of a colon definition */
	   					/* Maybe we should pick it up as a token? Then */
	   					/* we could do ;code stuff by adding its code to the switch */
						*--rp = ip;
						ip = (token_t *)*--token;
						next;
				}
		} /* End of top level switch */
	} /* End of for (;;) */
}

/*
 * Initialize the dictionary
 */

vocabulary_t tempvoc;


void
init_dictionary()
{
    XV_NUM_USER = NEXT_VAR*sizeof(cell);
    XV_TICK_TIB = (cell)&tibbuf[0];

    tokstore((token_t)origin, &XV_DP);

    /* reserve space for an array of pointers to the cfa of prim headers */
    XDP += (cell)NEXT_PRIM;
    tempvoc.code_field= (token_t)DOVOC;
    tempvoc.last_word = (dict_entry_t *)origin;
    tempvoc.voc_link  = (cell *)origin;

    XV_CURRENT = (cell)&tempvoc;
    XV_CONTEXT = (cell)&tempvoc;

    /* Make the initial dictionary entry header and code field */
    quote_create((u_char *)"\4main", (token_t)DOVOC);


    /* Set current and context to point at the code field of the initial
     * dictionary entry
     */
    tokstore((token_t)(XDP-1), &XV_CONTEXT);
    tokstore((token_t)(XDP-1), &XV_CURRENT);

/* This needs to be changed for hashing */
#ifdef RELOCATE
    set_relocation_bit(XDP);
#endif

    /* Comma the top word in the dictionary */
    xlinkcomma(tempvoc.last_word);


    /* Set the VOC-LINK user variable */
    tokstore((token_t)XDP, &XV_VOC_LINK);

    /* Compile the vocabulary link pointer */
    xlinkcomma(0);		/* voc-link XXX should be origin */

    init_entries();
}

void
init_variables()
{
	struct tm *curTime;
	time_t timevar;

    comp_level = 0;
    nlocnames = 0;
    XV_SPAN = XV_TO_IN = XV_BLK = XV_NUM_TIB = 0;
    XV_NUM_OUT = XV_NUM_LINE = 0;
    /* This is not redundant with the similar line in init_dictionary,
     * because init_dictionary is not executed when loading from a .dic file.
     */
    XV_TICK_TIB = (cell)&tibbuf[0];
    XV_BASE = 10;
    XV_TICK_EXPECT = (cell)SYS_EXPECT;
    XV_STATE = INTERPRETING;
    XV_SPZERO  = (cell)ps_top;
    XV_RPZERO  = (cell)&return_stack[RSSIZE];
    XV_UPZERO  = (cell)xup;
    XV_FPZERO  = (cell)&floatstack[FSTKLEN];
    XV_LINK = (cell)xup;		/* Link main task to self */
    multitasking = -1;
    XV_TASK_ACTIVE = -1;
    XV_WARNING = 1;
    XV_DPL = -1;
    XV_CNT = -1;		/* Signifies debug support not present */
    XV_DEBUGGING = XV_IP_L = XV_IP_H = 0;
    XV_MOD_COMP = 0;	/* Not compiling into a module -MdG */
    XV_MOD_LINK = 0;	/* No named modules currently exist */
	XV_NAMEDTASK = 0;	/* The main task is unnamed.  Even though .Task
						 * doesn't use this variable for printing the main
						 * task, this variable is copied from the main task
						 * when a new task is initialized.  This is a
						 * way of initializing it.
						 */
	XV_PERTASK = 0;
	XV_REGIONVAR = 0;
    DebuggerData._debug_in_progress = 0;
	DeckModule = 0;		/* Reset the deck module ID if re-entering after
						 * handling a signal
						 */
	maintaskReceiver = 0;

	timevar = time((time_t *)0);
	curTime = localtime(&timevar);
	currentDay = curTime->tm_mday;
	currentMonth = curTime->tm_mon;
	currentYear = curTime->tm_year;
}

void
init_entries()
{
    u_char *command;
    int immediate;
    int next_prim = 1;
    int next_uvar = 0;

    load((u_char *)INIT_FILENAME);
    while (1) {
	cexpect(tibbuf, (cell)TIBSIZE);
	XV_NUM_TIB = XV_SPAN;
	XV_TO_IN = 0;

	command = blword();
	if ( *command == 0 )
	    break;
#ifdef NOISY
printf("Word: %s\n", tibbuf);
#endif
	immediate = 0;
	switch ( command[1] ) {
	case 'i':
		immediate  = 1;
	case 'p':
		quote_create( blword(), (token_t)next_prim );
		tokstore((token_t)(XDP-1), &origin[next_prim]);
		if ( immediate ) makeimmediate();
		next_prim++;
		break;
	case 'v':
	case 'u':
		quote_create( blword(), (token_t)DOUSER );
#ifdef REMOVE
		ncomma(next_uvar);
#else
		*XDP++ = (cell)next_uvar;
#endif
		next_uvar += sizeof(cell);
		break;
	case 'e':
		return;
	default:
		strcpy(errMsg, "Unknown command while initializing the dictionary: ");
		strcat(errMsg, (char *)command);
		strcat(errMsg, "\n");
		MessageBox(NULL, errMsg, "Fatal error", MB_OK);
		ExitMeme(MEMEDICTBUILDSTATUS);
	}
    }
}

void
read_dictionary(name)
char *name;
{
    char tmpstr[50];
    size_t actual;
    FILE *fd;
    struct {
		cell magic, serial, dstart, dsize, ustart, usize, entry, res1;
    } header;

#define MAGIC 0x581120
    if ((fd = fopen(name, "rb")) == NULL) {
		fatal("Can't open dictionary file\n", MEMEDICTALLOCSTATUS);
	}

    actual = fread((char *)&header, 1, sizeof(header), fd);
    if (actual != sizeof(header)) {
		fatal("Can't read header\n", MEMEDICTALLOCSTATUS);
	}

    if (header.magic != MAGIC) {
		fatal("Bad magic number in dictionary file\n", MEMEDICTALLOCSTATUS);
	}

#ifndef RELOCATE
    if (header.dstart != (cell)origin)
	fatal("Remake the dictionary file\n", MEMEDICTALLOCSTATUS);
#endif

    actual = fread((char *)origin, 1, header.dsize, fd);
    if (actual != (u_cell)header.dsize) {
		sprintf(tmpstr, "Can't read dictionary image.  File position: %lu\n", ftell(fd));
		fatal(tmpstr, MEMEDICTALLOCSTATUS);
    }

    XV_DP = (cell) ((cell *)((char *)origin + header.dsize)); /* Ptr align */

    actual = fread((char *)xup, 1, header.usize, fd);
    if (actual != (u_cell)header.usize) {
		fatal("Can't read user area image\n", MEMEDICTALLOCSTATUS);
    }
#ifdef RELOCATE
    {
	   	unsigned int rbytes, rbits;
		register unsigned int i;
		cell *tp;
		cell offset = (cell)origin - header.entry;

		rbits = header.dsize/sizeof(cell);
		rbytes = (rbits + 7) >> 3;
		if (fread((char *)relmap, 1, (int)rbytes, fd) != (u_cell)rbytes) {
		    fatal("Can't read dictionary relocation map\n", MEMEDICTALLOCSTATUS);
		}
		if (offset != 0) {
			for (i = 0; i < rbits; i++) {
				if (relmap[i>>3] & bit[i&7]) {
					tp = &origin[i];
					*tp += offset;
				}
			}
		}
		rbits = header.usize/sizeof(cell);
		rbytes = (rbits + 7) >> 3;
		if (fread((char *)urelmap, 1, (int)rbytes, fd) != (u_cell)rbytes) {
		    fatal("Can't read data relocation map\n", MEMEDICTALLOCSTATUS);
		}
		if (offset != 0) {
			for (i = 0; i < rbits; i++) {
				if ((urelmap[i>>3] & bit[i&7]) && ((token_t)xup[i] >= MAXPRIM)) {
					tp = &xup[i];
					*tp += offset;
				}
			}
# ifdef notdef
		    /* Save the relocated dictionary to a file for debugging */
		    (void)fclose(fd);

		    if ((fd = fopen("meme.dx", WRITE_MODE)) < (FILE *) 0)
			fatal("Can't open debugging dictionary image for writing\n", MEMEDICTALLOCSTATUS);

		    fprintf(fd, "offset = 0x%-4.4x\n", offset);
		    if (fwrite(origin, 1, (unsigned int)header.dsize, fd)
		    != (unsigned int)header.dsize)
			fatal("Can't write dictionary image\n", MEMEDICTALLOCSTATUS);

		    if (fwrite((char *)up, 1, (unsigned int)header.usize, fd)
		    != (unsigned int)header.usize)
			fatal("Can't write user area image\n", MEMEDICTALLOCSTATUS);
# endif
		}
	}
#endif	/* RELOCATE */
    (void)fclose(fd);
}

u_char num_buf[40];

/* OS-9 C Compiler can't handle this as "u_char" */
static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

void
udot(u)
unsigned cell u;
{
    register u_char *p = &num_buf[40];
    register unsigned cell a = u;

    *--p = ' ';		/* gratuitous space at the end */

    do {
	*--p = (u_char)digits[ a % XV_BASE ];
    } while ((a /= XV_BASE) != 0);

    type(p, (cell)(&num_buf[40] - p));
}

void
dot(n)
cell n;
{
    if ( n < 0 ) {
	emit('-');
	n = -n;
    }
    udot((unsigned cell)n);
}

void
type(adr,len)
register u_char *adr;
register cell len;
{
    while (len--)
	emit(*adr++);
}

/*
 * Implements vocabularies, searching, and vocabulary search order
 * using linked headers.
 */

/*
 * Header format:
 * Field				Type			Description
 * 
 * Link Field:			cell *			Points to previous link field
 * Flag byte:			u_char			Attribute bits (immediate bit)
 * Name Field:			packed-string	Name of this entry
 * Alignment:			n * '\0'		Padding to align to next cell (-1)
 * Code Field:			cell			Determines what kind of work
 * Parameter Field:		n * cell		Word-specific parameters
 *
 */

#define IMMEDBIT 0x01

#define isimmediate(lfa) ((lfa)->flags & IMMEDBIT)

token_t *
aligned(addr)
u_char * addr;
{
    /* This calculation assumes twos-complement representation */
    return( (token_t *)
      ( ((cell)addr + ALIGN_BOUNDARY - 1) & (-((cell)ALIGN_BOUNDARY)) )
    );
}

/* This should be called skip-string */
token_t *
name_from( nfa )
u_char * nfa;
{
    return ( aligned( &nfa[(*nfa)+1] ) );
}

void
makeimmediate()
{
    XV_LAST->flags |= IMMEDBIT;
}

/*
 * Look for a name within a vocabulary.	 Sp is a copy of the stack pointer.
 * The top of the stack contains a pointer to a string which is the name to
 * be searched for.
 *
 * If the name is found, the string address on top of the stack is replaced
 * by the token associated with that name and vfind returns either
 * 1 (if the name has the immediate attribute) or -1.
 *
 * If the name is not found, the top of the stack is left unchanged, and
 * vfind returns 0 (Forth FALSE).
 *
 * Forth system keeps the true top of the stack in a register.	The stack
 * pointer passed to vfind is actually the address of the stack item
 * just below the top of stack.	 This turns out to be quite convenient,
 * since the string passed to vfind is supposed to be below the vocabulary.
 */

#define hash(voc,str) (&((voc)->last_word))

int
vfind(sp, voc)
u_char **sp;
vocabulary_t *voc;
{
    /* The first character in the string is the Forth count field. */
    register u_char *s,*p;
    register int length;
    register u_char *str = *sp;
    register dict_entry_t *dictp;
    token_t * cfa;

    /* Relocatable modules cannot compile origin into the bottom-most
     * word's link field, as other machines will not have the same
     * origin.  Origin in this implementation always contains a zero,
     * so we dereference the dictp pointer and look for zero rather
     * than checking the pointer for equality with the origin.
     */
    dictp = *hash(voc, str); 
    while ( *(cell *)dictp != 0 ) {
	s = &dictp->name;
	p = str;
	length = *s++;
	if ( *p++ != length )
	    goto nextword;
	while ( length-- )
	    if ( *s++ != *p++ )
		goto nextword;

	cfa = name_from( &dictp->name );
	*(token_t *)sp = (token_t) cfa;

	return ( isimmediate(dictp) ? 1 : -1 );
nextword:
	dictp = dictp->link;
    }
    return (0);	       /* Not found */
}

token_t lnamebuf[] = { DOLOCNAME, };

/* sp is a pointer to a pointer to a pstring.  If the search is successful,
 * the pointer to the string is modified to be the acf of the found word,
 * and find_local() returns TRUE.  If not found, the pointer is left alone
 * and a FALSE is returned.
 */
int
find_local(sp)
u_char **sp;
{
    /* The first character in the string is the Forth count field. */
    register u_char *s,*p;
    register int length;
    register u_char *str = *sp;

    for ( locnum = 0; locnum < nlocnames; ) {
		s = locnames[locnum].name;
		p = str;
		length = *s++;
		if ( *p++ != length)
			goto nextword;

		while ( length-- )
			if ( *s++ != *p++ )
				goto nextword;

        *(token_t *)sp = (token_t)lnamebuf;
		return (1);			/* Immediate */
nextword:
		locnum++;
    }
    return (0);
}

int
find(sp)
cell *sp;
{
	int i, found = 0;
	vocabulary_t *voc, *last_voc;

	/* If the symbol is found in the local variable names, return */
	if ((found = find_local((u_char **)sp)) != 0) {
		return (found);
	}

	/* Search the vocabularies mentioned in the search order */
    last_voc = (vocabulary_t *)0;
    for (i = 0; i < NVOCS; i++) {
		voc = ((vocabulary_t **)&XV_CONTEXT)[i];
		if ( voc == 0 || voc == (vocabulary_t *)origin) {
			continue;
		}
		if ((voc != last_voc) && ((found = vfind((u_char **)sp, voc)) != 0)) {
			break;
		}
		last_voc = voc;
    }
    return (found);
}

void
align()
{
    register length = (int)((char *)aligned((u_char *)XDP) - (char *)XDP);
    register u_char *rdp = (u_char *)XDP;
    while ( length-- )
	 *rdp++ = '\0';
    XV_DP = (cell) ((cell *)rdp);		/* Pointer alignment */
}

/* Place a string in the dictionary */
void
comma_string(str)
u_char *str;
{
    register u_char *s = str;
    register u_char *rdp = (u_char *)XDP;
    register length = (*s) + 1;

    while ( length-- )
	 *rdp++ = *s++;
    XV_DP = (cell) ((cell *)rdp);		/* Pointer alignment */
    align();
}

void
quote_create(str, cf)
u_char *str;
token_t cf;
{
    dict_entry_t ** threadp = hash ((vocabulary_t *) XV_CURRENT, str);
    register	u_char * rdp;
    u_char *tmpstr = str;

    if (XV_WARNING && vfind((u_char **)&tmpstr, (vocabulary_t *)XV_CURRENT)) {
		sprintf(errMsg, "%s isn't unique\n", tocstr(str));
		error(errMsg);
    }

    align();
    rdp = (u_char *) XDP;

#ifdef RELOCATE
    set_relocation_bit(XDP);
#endif

#ifdef REMOVE	/* Causing a protection violation if writing into a module */
    tokstore((token_t)rdp, &XV_LASTP);
#else
	XV_LASTP = (cell)rdp;
#endif

 /* Link into vocabulary search list and remember lfa for hide/reveal */
    xlinkcomma((*threadp));
    tokstore((token_t)rdp, (cell *)threadp);
    rdp = (u_char *) XDP;

    *rdp++ = 0;			/* Place flag byte, default not immediate */

    XV_DP = (cell)((cell *) rdp);	/* Pointer alignment */

    comma_string(canonical(str));	/* Place name in dictionary */
    xcompile(cf);
#ifdef NOISY
	sprintf("Meme: %s\n", tocstr(str));
	error(errMsg);
#endif
}

void
hide()
{
    /* str is not actually used, because of the definition of hash() */
    /* u_char *str = &XV_LAST->name; */
    dict_entry_t **threadp = hash( (vocabulary_t *)XV_CURRENT , str );

    *threadp = XV_LAST->link;
}

void
reveal()
{
    /* str is not actually used, because of the definition of hash() */
    /* u_char *str = &XV_LAST->name; */
    dict_entry_t **threadp = hash( (vocabulary_t *)XV_CURRENT, str );

    *threadp = XV_LAST;
}

/*
 * Forth outer (text) interpreter and
 * compile-time actions for immediate words.
 */

/* Read the next delimited word from the input stream */

/* FIXME  The standard says strings from word should end with a blank */
u_char *
blword()
{
	register u_char *bufend = &((u_char *)XV_TICK_TIB)[XV_NUM_TIB];
	register u_char *nextc  = &((u_char *)XV_TICK_TIB)[XV_TO_IN];
	register int c;
	register int i = 1;

	do {
		if ( nextc >= bufend ) {
			XV_DELIMITER = FEOF;
			goto finish;
		}
		c = *nextc++;
	} while ( isspace(c) );

    /* Now c contains a non-delimiter character. */

    do {
		/*
		 * If the string collected is longer than the maximum length
		 * we can store, it is silently truncated.  This is generally
		 * okay because such a string is probably either an error,
		 * in which case the interpreter will almost certainly not find a
		 * match, or a comment, in which case the string will be thrown
		 * away anyway.
		 */
		if ( i < MAXSTRING ) wordbuf[i++] = c;
		if ( nextc >= bufend ) {
			XV_DELIMITER = FEOF;
			goto finish;
		}
		c = *nextc++;
    } while ( !isspace(c) );
    XV_DELIMITER = c;

finish:
    wordbuf[0] = i-1;
    wordbuf[i] = '\0';
    XV_TO_IN = nextc - (u_char *)XV_TICK_TIB;

    if (verboseMode) {
		cprint(&wordbuf[1]);
		cprint(" ");
    }
    return (wordbuf);
}

u_char *
word(delim)
u_char delim;
{
    register u_char *bufend = &((u_char *)XV_TICK_TIB)[XV_NUM_TIB];
    register u_char *nextc = &((u_char *)XV_TICK_TIB)[XV_TO_IN];
    register int c;
    register int i = 1;

    if (delim == ' ')  /* Only skip leading delimiters if delim is space */
	 return (blword());

    for (;;) {
	if ( nextc >= bufend ) {
	    XV_DELIMITER = FEOF;
	    goto finish;
	}
	if ( (c = *nextc++) == delim )
	    break;
	/*
	 * If the string collected is longer than the maximum length
	 * we can store, it is silently truncated.  This is generally
	 * okay because such a string is probably either an error,
	 * in which case the interpreter will almost certainly not find a
	 * match, or a comment, in which case the string will be thrown
	 * away anyway.
	 */
	if ( i < MAXSTRING )
	    wordbuf[i++] = c;
    }
    XV_DELIMITER = c;

finish:
    wordbuf[0] = i-1;
    wordbuf[i] = '\0';
    XV_TO_IN = nextc - (u_char *)XV_TICK_TIB;
    return (wordbuf);
}
void
scompile(str)
u_char *str;
{
    cell tp = (cell)str;

    if( find(&tp) ) {
/*
 * If the word we found is a primitive, compile its primitive number
 * instead of its cfa
 */
	if ( *(token_t *)tp < MAXPRIM )
	    tp = *(token_t *)tp;
	xcompile(tp);
    } else {
	xcompile(LOSE);
	where();
    }
}

void
execute_one(token)
token_t token;
{
	token_t CFABuff[2];

	CFABuff[0] = token;
	CFABuff[1] = (token_t) FINISHED;
	inner_interpreter(CFABuff);
}

void
execute_one_with_one_arg(token,arg)
token_t token;
cell arg;
{
	token_t CFABuff[2];

	*--xsp = arg;
	CFABuff[0] = token;
	CFABuff[1] = (token_t) FINISHED;
	inner_interpreter(CFABuff);
}

void
Xexecute_one_with_one_arg(token,arg)
token_t token;
cell arg;
{
	cell *oldup;
	cell *oldsp;
	token_t **oldrp;
	double *oldfsp;
	cell *oldfrp;
	cell intstack[64];
	token_t *rstack[128];
	double fstack[64];
	token_t CFABuff[2];
	char userarea[MAXUSER];
	char localtibbuf[256];

	/* Set up a context for the meme to be executed.  Copy the main task's
	 * user area.  Use the locally allocated integer, return, and float
	 * stacks.
	 */
	memcpy(userarea, main_task, MAXUSER);
	oldfrp = xfrp; xfrp = 0;
	oldup = xup;  xup = (cell *)userarea; XV_LINK = (cell)xup;
	oldfsp = xfsp; XV_FPZERO = (cell)(xfsp = &fstack[64]);
	oldrp = xrp; XV_RPZERO = (cell)(xrp = &rstack[128]);
	intstack[63] = arg;
	oldsp = xsp; xsp = &intstack[63];
	XV_SPZERO = (cell)&intstack[64];

	
	/* Make sure these user variables are clean regardless of maintask's
	 * values for them.
	 */
	XV_HANDLER = 0;						/* For throw and catch */
	XV_SPAN = XV_TO_IN = XV_BLK = XV_NUM_TIB = 0;
	XV_NUM_OUT = XV_NUM_LINE = 0;
    XV_TICK_TIB = (cell)&localtibbuf[0];
    XV_BASE = 10;
    XV_TICK_EXPECT = (cell)SYS_EXPECT;
    XV_STATE = INTERPRETING;
    XV_TASK_ACTIVE = -1;
    XV_WARNING = 1;
    XV_DPL = -1;
    XV_CNT = -1;		/* Signifies debug support not present */
    XV_DEBUGGING = XV_IP_L = XV_IP_H = 0;
    XV_MOD_COMP = 0;	/* Not compiling into a module -MdG */
    XV_MOD_LINK = 0;	/* No named modules currently exist */
	XV_NAMEDTASK = 0;	/* The main task is unnamed.  Even though .Task
						 * doesn't use this variable for printing the main
						 * task, this variable is copied from the main task
						 * when a new task is initialized.  This is a
						 * way of initializing it.
						 */
	XV_PERTASK = 0;
	XV_REGIONVAR = 0;


	CFABuff[0] = token;
	CFABuff[1] = (token_t) FINISHED;
	inner_interpreter(CFABuff);

	xsp = oldsp;
	xrp = oldrp;
	xfsp = oldfsp;
	xup = oldup;
	xfrp = oldfrp;
}

int
interpret_word(str)
u_char *str;
{
    cell tp = (cell)canonical(str);
    int immed;

    if ((immed = find(&tp)) != 0) {
/*
 * If the word we found is a primitive, use its primitive number
 * instead of its cfa
 */
		if ( *(token_t *)tp < MAXPRIM )
			tp = *(token_t *)tp;
/*
 * If the immediate bit is set or we're in interpreting state
 * execute the token, else compile it at the the top of the dictionary.
 */
		if ( immed > 0 || XV_STATE == (cell)INTERPRETING ) {
			execute_one((token_t)tp);
		} else {
			/* If compiling into a module, check whether we've referred
			 * to a high-level definition outside the module.  If so,
			 * print a warning message.
			 */
			if ( XV_MOD_COMP && XV_STATE == (cell) COMPILING) {
				if (tp >= MAXCF) {
					if ( (unsigned long)tp < (unsigned long)ThisModule ||
								(unsigned long)tp >= (unsigned long)((cell)ThisModule->ModuleDict + 
												ThisModule->RelTableOffset) ) {
						if ( (u_cell)tp < (u_cell)(ThisModule->CompilationLibOrigin) ||
							 (u_cell)tp >= (u_cell)(((module *)(ThisModule->CompilationLibOrigin))->ModuleDP) ) {
							where();
							error("Warning: reference outside the module and std library\n");
							xcompile(LOSE);
							return(1);
						}	/* if tp is outside the std library */
					}	/* if tp is outside the module */
				} /* if (tp > MAXPRIM) */
			}	/* if (XV_MOD_COMP) */
			xcompile(tp);
		}
		return(1);
    }
    if ( isfliteral(str) )
        return(1);

    if ( handle_literal(str) )
        return(1);

    /* Undefined */
    where();
    if ( XV_STATE == (cell)COMPILING ) {
		xcompile(LOSE);
		return(1);
    }
    pop_all();
    return(0);
}

void
where()
{
    strcpy(errMsg, (char *)&wordbuf[1]); strcat(errMsg, " ?");
    if( input_name[0] != '\0' ) {
        strcat(errMsg, "   Last: ");
	strcat(errMsg, tocstr(&((XV_LAST) -> name)));
	strcat(errMsg, "   File: ");
	strcat(errMsg, tocstr(input_name) );
    }
    strcat(errMsg, "\n");
	error(errMsg);
}

int
handle_literal(str)
u_char *str;
{
    long n;
    int ok;

    ok = number( str, &n );
    if ( ok == 1 ) {
	if ( XV_STATE == (cell)COMPILING ) {
	    xcompile(PAREN_LIT);  *XDP++ = (cell)n;
	} else {
	    *--xsp = (cell)n;
	}
	return (1);
    }
    return (0);
}

void
query()
{
	*--xsp = (cell)tibbuf;
	*--xsp = (cell)TIBSIZE;
	(void)execute_one((token_t)EXPECT);
	XV_NUM_TIB = XV_SPAN;
	XV_TO_IN = 0;
}

void
interpret()
{
    u_char *thisword;

    while ( (((thisword = blword())[0]) != 0) && interpret_word(thisword) ) {
		if ( xsp > (cell *)XV_SPZERO ) {
			xsp = (cell *)XV_SPZERO;
			error("Integer stack underflow\n");
			break;
		}
		if ( xfsp > (double *)XV_FPZERO ) {
			xfsp = (double *)XV_FPZERO;
			error("Floating-point stack underflow\n");
			break;
		}
#ifdef SIGNALS
		insane = 0;
#endif
    }
}

void
interpret_line()
{
    prompt();
    query();		/* TIB now contains the line */
#ifndef MEMEVIEW
    interpret();
#endif
}

void
outer_interpreter()
{
#ifdef MEMEVIEW
    ExitMeme(MEMEVIEWOUTERINTERPRETERSTATUS);
#else
    XV_STATE = (cell)INTERPRETING;
    xsp = (cell *)XV_SPZERO;
    xfsp = (double *)XV_FPZERO;

    for (;;) {
		/* Clear the return stack */
		xrp = (token_t **)XV_RPZERO;

		/* Print a prompt, get one line of text, and interpret it. */
		interpret_line();
    }
#endif
}


/*
 * scanf doesn't work because it would accept numbers which don't take up
 * the whole word, as in 123xyz
 */
int
number( str, n )
u_char *str;
long *n;
{
    int base = XV_BASE;
    int len = *str++;
    u_char c;
    int d;
    int isminus = 0;
    long accum = 0;

    XV_DPL = -100;
    if( *str == '-' ) {
	isminus = 1;
	len--;
	++str;
    }
    for( ; len > 0; len-- ) {
	c = *str++;
	if( c == '.' )
	    XV_DPL = 0;
	else {
	    if( -1 == (d = digit( (cell)base, c )) )
		break;
	    ++XV_DPL;
	    accum = accum * base + d;
	}
    }
    if (XV_DPL < 0)
	XV_DPL = -1;
    *n = isminus ? -accum : accum;
    return( len == 0 );
}

/*
 * Converts the character c into a digit in base 'base'.
 * Returns the digit or -1 if not a valid digit.
 * Accepts either lower or upper case letters for bases larger than ten.
 */
cell
digit ( base, c )
cell base;
u_char c;
{
    register int ival = c;

    if ('0' <= c && c <= '9')
        ival = c - '0';
    else if ('a' <= c && c <= 'z')
        ival = 10 + c - 'a';
    else if ('A' <= c && c <= 'Z')
        ival = 10 + c - 'A';
    else
        ival = -1;
    return (ival < base ? ival : -1);
}

void
cmove(from, to, length)
register u_char *from, *to;
register unsigned cell length;
{
    while ((length--) != 0)
        *to++ = *from++;
}

void
cmove_up(from, to, length)
register u_char *from, *to;
register unsigned cell length;
{
    from += length;
    to += length;

    while ((length--) != 0)
        *--to = *--from;
}

void
fill(to, length, with)
register u_char *to;
register u_cell length;
register u_char with;
{
    while ((length--) != 0)
        *to++ = with;
}

char astrbuf[257];
char *
tocstr( from )
register u_char *from;
{
    register char *to = astrbuf;
    register int length = *from++;

    while ((length--) != 0)
	*to++ = *from++;

    *to++ = '\0';
    return (astrbuf);
}

u_char *
tofstr( from )
register u_char *from;
{
    register u_char *to = (u_char *)&astrbuf[1];
    register u_char c;
    register int length = 0;

    while ( (c = *from++) != '\0' ) {
	*to++ = c;
	if (++length >= 255)
	    break;
    }

    *to++ = '\0';
    *(u_char *)astrbuf = length;
    return((u_char *)astrbuf);
}

u_char *
canonical(fstr)
u_char *fstr;
{
    register u_char *str = fstr;
    register int length;
    register char c;

    if ( XV_CAPS ) {
	for ( length = *str++; length--; str++) {
	    c = *str;
	    /* ASCII-dependent */
	    if ( c >= 'A' && c <= 'Z' )
		*str = c - 'A' + 'a';
	}
    }
    return( fstr );
}

/*
 * If string 1 is a proper substring of string2, return the offset from
 * the start of string2 where string1 begins.  If string1 is not a
 * substring of string2, return -1.
 */
cell
strindex(adr1, len1, adr2, len2)
u_char *adr1;
cell len1;
u_char *adr2;
cell len2;
{
    register int n;
    register u_char *p, *q;
    register int i;

    for (n = 0; len2 >= len1; adr2++, len2--, n++) {
	p = adr1;
	q = adr2;
	i = len1;
	while(i-- > 0)
	    if (*p++ != *q++)
	        goto tryagain;
	/* Found match */
	return(n);
tryagain: ;
    }
    return(-1);
}

void
pluslevel()
{
    if (comp_level)
	++comp_level;
    else if (XV_STATE == 0) {
	comp_level = 1;
	saved_dp = XDP;
    	XV_DP = (cell) ((cell *)(comp_buffer));
	XV_STATE = (cell)COMPILING;
    }
}

int
declevel()
{
    if (XV_STATE == 0)
        error("Conditionals not paired\n");
    if (comp_level) {
    	--comp_level;
	if (comp_level == 0) {	/* Dropped back to level 0 */
	    xcompile(FINISHED);	/* compile(EXIT); */
	    XV_DP = (cell)saved_dp;
	    XV_STATE = (cell)INTERPRETING;
	    /* Return nonzero to cause execution of the temporary def'n? */
	    return(1);
	}
    }
    return(0);
}

char *
substr(str, pos, n)
char *str;
int pos, n;
{
	register int len = strlen(str);
	static char outstr[128];

	if( pos < 0 )
		pos += len+1;
	if( pos <= 0 )
		pos = 1;
	if (n < 0)
		n += len;
	if (pos + n - 1 > len) {
		n = len + 1  - pos;
		if (n < 0)
		    n = 0;
	}
	(void)strncpy(outstr, str + pos - 1, n);
	outstr[n] = '\0';

	return(outstr);
}

void
tokstore(token, adr)
token_t token;
cell *adr;
{
	u_char *p;

	if ( XV_MOD_COMP ) {
		if (token >= MAXCF) {
			if ( (unsigned long)token < (unsigned long)ThisModule || (void *)token >= 
										(void *)((cell)ThisModule->ModuleDict + 
												ThisModule->RelTableOffset) ) {
				if ( (u_cell)token < (u_cell)(ThisModule->CompilationLibOrigin) ||
						(u_cell)token >= 
							(u_cell)((module *)ThisModule->CompilationLibOrigin)->ModuleDP ) {
					p = (u_char *)token;
					do {
						p--;
					} while (*p == '\0' || *p > ' ') ;
					sprintf(errMsg, "%s ?", tocstr(p));
					if( input_name[0] != '\0' ) {
						strcat(errMsg, "   Last: ");
						strcat(errMsg, tocstr(&((XV_LAST) -> name)));
						strcat(errMsg, "   File: ");
						strcat(errMsg, tocstr(input_name) );
					}
					strcat(errMsg, "\nWarning: reference outside the module and std library.\n");
					error(errMsg);
					*adr = (cell) LOSE;
					return;

				}
			}
		}
	}
	if ( (token >= MAXCF) ) {
		set_relocation_bit(adr);
	}
	*adr = (cell)token;			/* Store it! (finally)	*/
}

void
set_relocation_bit(adr)
cell *adr;
{
	if (XV_MOD_COMP) {
		module_set_relocation_bit(adr);
#ifdef RELOCATE
	} else {
		main_set_relocation_bit(adr);
#endif
	}
}

#ifdef RELOCATE
/* References to xup don't work in this routine.  The multitasking code takes some
 * liberties with temporarily setting the user pointer to a new task's memory to
 * initialize it.  Often, the dictionary pointer is nonsense.  I have commented
 * out various bits to make sure that the values for the dictionary, etc. are
 * always valid.
 */
void
main_set_relocation_bit(cell *adr)
{
	cell offset;

	offset = (cell)(adr - origin);
#ifdef REMOVE
	if ((offset >= 0)  &&  (adr <= XDP)) {
#else
	if ((offset >= 0) && (adr <= MAIN_DP)) {
#endif
		relmap[offset>>3] |= bit[offset&7];
		return;
	}
#ifdef REMOVE
	offset = (cell)(adr - xup);
#else
	offset = (cell)(adr - (cell *)main_task);
#endif

	if ((offset >= 0)  &&  (offset < MAXUSER)) {
		urelmap[offset>>3] |= bit[offset&7];
		return;
	}
}
#endif

#ifdef REMOVE
/* First try at relocation for the std library.  Doesn't work because often
 * the value is written in the cell AFTER the call to set-relocation-bit()
 */
void
module_set_relocation_bit(cell *adr)
{
	module *mp;					/* Pointer to the module		    				*/
	unsigned char *reltable;	/* Pointer to the module's relocation table			*/
	cell offset;				/* Cell offset of adr from start of module			*/
	char *stdLib;				/* Standard library's origin						*/
	char *stdLibEnd;			/* First byte above the std lib that's outside it	*/

	mp = ThisModule;
	reltable = (unsigned char *)mp + mp->RelTableOffset;
	offset = (cell)(adr - (cell *)mp);
	stdLib = mp->CompilationLibOrigin;
	stdLibEnd = ((module *)stdLib)->ModuleDP;

	/* If adr is a location in the module */
	if ( (u_cell)adr >= (u_cell)mp && (u_cell)adr < (u_cell)reltable ) {

		/* If *adr is a pointer into the standard lib, use 4 as a relocation code,
		 * else use 3
		 */
		if ( (u_cell)*adr >= (u_cell)stdLib && (u_cell)*adr < (u_cell)stdLibEnd ) {
			reltable[offset] = (unsigned char)4;
		} else {
			reltable[offset] = (unsigned char)3;
		}
	}
}
#else
/* Below is a slight variation on the original code--
 * the only thing different is it stores a 3 instead of ORing a 1.
 */
void
module_set_relocation_bit(cell *adr)
{
	module *mp;			/* Pointer to the module		    */
	char *reltable;		/* Pointer to the module's relocation table */
	cell offset;		/* Cell offset of adr from start of module  */

	mp = ThisModule;
	reltable = (char *)mp + mp->RelTableOffset;
	offset = (cell)(adr - (cell *)mp);
	// reltable[offset] |= 1;
	reltable[offset] = 3;
}
#endif

/* Interface to user-supplied C subroutines */
cell
doccall(callno, format, spp)
cell callno;
u_char *format;
cell **spp;
{
    register cell *sp = *spp;
    cell arg0, arg1, arg2, arg3, arg4,  arg5,
           arg6, arg7, arg8, arg9, arg10, arg11;
    cell ret;

    extern char *tocstr();
    extern u_char *tofstr();
    extern cell (*ccalls[])();

/* The following cases are ordered by expected frequency of occurrence */
#define CONVERT(var) switch(*format++) { case 'i': var = *sp++; break; case '-': goto doccall; case 's': var = (cell) tocstr((u_char *)*sp++); break; case 'a': var = *sp++; break; case 'l': var = *sp++; break; }

    CONVERT(arg0);
    CONVERT(arg1);
    CONVERT(arg2);
    CONVERT(arg3);
    CONVERT(arg4);
    CONVERT(arg5);
    CONVERT(arg6);
    CONVERT(arg7);
    CONVERT(arg8);
    CONVERT(arg9);
    CONVERT(arg10);
    CONVERT(arg11);
#undef CONVERT
doccall:
    /* callno is the address of a C subroutine */
    ret = (ccalls[callno])(arg0, arg1, arg2, arg3,
                               arg4, arg5, arg6, arg7,
			       arg8, arg9, arg10, arg11);
    switch(*format) {
    case '\0': ret = *sp++; break;
    case 's': ret = (cell)tofstr((u_char *)ret); break;
	/* Default: ret is correct already */
    }
    *spp = sp;
    return(ret);
}

int
isfliteral(str)
unsigned char *str;
{
	cell *p;

	if(!isfloatnum(str)) return(0);		/* Fail if not a float */
	fbits.fb_float = atof(tocstr(str));	/* Convert str to float */
	if (XV_STATE == COMPILING) {		/* If interp state: compile */
	    /* Compile an interoperable float if compiling into a module */
	    if (XV_MOD_COMP) {
		cell exponent, mantissa;
		xcompile(IFPAREN_LIT);
		Float2IFloat(fbits.fb_float, &exponent, &mantissa);
		p = (cell *)XV_DP;
		*p++ = exponent;
		*p++ = mantissa;
		XV_DP = (cell)p;
	    } else {
		xcompile(FPAREN_LIT);
		p = (cell *)XDP;
		/*
		 * Potential portability problem: what if the alignment
		 * rules are different for longs and cells?
		 */
		*p++ = (cell)fbits.fb_long[0];
		*p++ = (cell)fbits.fb_long[1];
		XV_DP = (cell)p;
	    }
	} else {
	    *--xfsp = fbits.fb_float;
	}
	return (1);
}

int
isfloatnum(str)
	unsigned char *str;
{
	int efound = 0;
	int len;

	/*
	 * Don't recognize floating point numbers unless base is decimal.
	 * This prevents the ambiguity with "e" for exponent or "e" for
	 * a hex digit.
	 */
	if (XV_BASE != 10)
		return(0);

	for (len = *str++; len; len--) {
		switch (*str++) {
		case '0':  case '1':  case '2':  case '3':  case '4':
		case '5':  case '6':  case '7':  case '8':  case '9':
		case '+':  case '-':  case '.':
			break;
		case 'e':
		case 'E':
			efound = 1;
			break;
		default:
			return(0);
		}
	}
	return(efound);
}
