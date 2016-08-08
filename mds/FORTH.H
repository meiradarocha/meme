/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

#ifndef	__FORTH_H__
#define __FORTH_H__

/* Used by COLD in forth.c and the cold start code in init.c */
#define	SETJMPRTNCOLDSTART	(-12345)
/*
 * The following definitions are shared by various C source code files.
 */
#define u_cell unsigned cell

#ifndef	u_char
# define u_char   unsigned char
#endif

#define var(index)      (up[index])
#define xvar(index)	(xup[index])

#define V_NUM_USER     var(0)  /* offset of 1st unused location in user area */
#define V_SPAN         var(1)  /* count of chars received from last expect */
#define V_TO_IN        var(2)  /* present char offset in the input stream */
#define V_BASE         var(3)  /* @ of var containing conversion radix */
#define V_BLK          var(4)  /* @ of var containing blk# being interpreted */
#define V_NUM_TIB      var(5)  /* @ of var containing #bytes in input buffer */
#define V_STATE        var(6)  /* compilation state (compiling/interpreting) */
#define V_CURRENT      var(7)  /* @ of var specifying the current voc */
#define V_DELIMITER    var(8)  /* the char that stopped the last word/etc */
#define V_SPZERO       var(9)  /* @ of the bottom of the data stack */
#define V_RPZERO       var(10) /* @ of the bottom of the return stack */
#define V_UPZERO       var(11) /* contains the start addr of the user area */

/*
 * LASTP needed for use as an lvalue, where the cast doesn't work.
 */
#define V_LAST         ((dict_entry_t *)var(12))
#define V_LASTP        var(12) /* the nfa of the last word created */
#define V_NUM_OUT      var(13) /* the column number on the output device */
#define V_NUM_LINE     var(14) /* incremented each time that cr is executed */
#define V_VOC_LINK     var(15) /* contains @ of last vocabulary created */
#define V_DPL          var(16) /* describes decimal point of last # parsed */
#define V_WARNING      var(17) /* non-zero yields "isn't unique" messages */
#define V_CAPS	       var(18) /* true = case insensitive */
#define V_ERRNO        var(19) /* error code of the last failing system call */
#define V_FNUMPLACES   var(20) /*  */
#define V_HANDLER      var(21) /* the @ of service routine for a throw (?) */
#define V_TICK_EXPECT  var(22) /* addr of expect routine, usually SYS_EXPECT */

#define V_LINK         var(23) /* pointer to next task */
#define V_TASK_ACTIVE  var(24) /* task status, 0 = inactive */
#define V_SAVED_IP     var(25) /* saved IP while task isn't executing */
#define V_SAVED_RP     var(26) /* saved RP while task isn't executing */
#define V_SAVED_SP     var(27) /* saved SP while task isn't executing */
#define V_DP           var(28) /* dictionary pointer */
#define	DP		(((cell **)up)[28])
#define V_DEBUGGING    var(29) /* nonzero if we're debugging */
#define V_TICK_DEBUG   var(30) /* token when interpreted starts debugging */
#define V_IP_L         var(31) /* low bounds of word to debug */
#define V_IP_H         var(32) /* high bounds of word to debug */
#define V_CNT          var(33) /* -1 = no support, 0 = off, 1 = on */
#define V_MOD_COMP     var(34) /* -1 = compiling into module, 0 = dictionary */
#define	V_TICK_TIB     var(35) /* Terminal Input Buffer pointer */
#define	V_MOD_LINK     var(36) /* Head of linked list of named modules */
#define V_SAVED_FP     var(37) /* Saved floating-point stack pointer */
#define V_FPZERO       var(38) /* Top of floating-point stack */
#define	V_TURNKEY      var(39)
#define	V_NAMEDTASK	   var(40)
#define	V_PERTASK      var(41) /* Ptr to programmer-specified per-task data */
#define V_SAVED_FRP    var(42) /* Saved frame pointer (for local variables) */
#define	V_REGIONVAR	   var(43) /* Reserved ptr for use by region DB code	*/
#define V_CONTEXT      var(44) /* array @ specifying dictionary search order */

#define XV_NUM_USER     xvar(0)  /* offset of 1st unused location in user area */
#define XV_SPAN         xvar(1)  /* count of chars received from last expect */
#define XV_TO_IN        xvar(2)  /* present char offset in the input stream */
#define XV_BASE         xvar(3)  /* @ of var containing conversion radix */
#define XV_BLK          xvar(4)  /* @ of var containing blk# being interpreted */
#define XV_NUM_TIB      xvar(5)  /* @ of var containing #bytes in input buffer */
#define XV_STATE        xvar(6)  /* compilation state (compiling/interpreting) */
#define XV_CURRENT      xvar(7)  /* @ of var specifying the current voc */
#define XV_DELIMITER    xvar(8)  /* the char that stopped the last word/etc */
#define XV_SPZERO       xvar(9)  /* @ of the bottom of the data stack */
#define XV_RPZERO       xvar(10) /* @ of the bottom of the return stack */
#define XV_UPZERO       xvar(11) /* contains the start addr of the user area */

/*
 * LASTP needed for use as an lvalue, where the cast doesn't work.
 */
#define XV_LAST         ((dict_entry_t *)xvar(12))
#define XV_LASTP        xvar(12) /* the nfa of the last word created */
#define XV_NUM_OUT      xvar(13) /* the column number on the output device */
#define XV_NUM_LINE     xvar(14) /* incremented each time that cr is executed */
#define XV_VOC_LINK     xvar(15) /* contains @ of last vocabulary created */
#define XV_DPL          xvar(16) /* describes decimal point of last # parsed */
#define XV_WARNING      xvar(17) /* non-zero yields "isn't unique" messages */
#define XV_CAPS	       xvar(18) /* true = case insensitive */
#define XV_ERRNO        xvar(19) /* error code of last failing system call */
#define XV_FNUMPLACES   xvar(20) /*  */
#define XV_HANDLER      xvar(21) /* the @ of service routine for a throw (?) */
#define XV_TICK_EXPECT  xvar(22) /* addr of expect, usually SYS_EXPECT */

#define XV_LINK         xvar(23) /* pointer to next task */
#define XV_TASK_ACTIVE  xvar(24) /* task status, 0 = inactive */
#define XV_SAVED_IP     xvar(25) /* saved IP while task isn't executing */
#define XV_SAVED_RP     xvar(26) /* saved RP while task isn't executing */
#define XV_SAVED_SP     xvar(27) /* saved SP while task isn't executing */
#define XV_DP           xvar(28) /* dictionary pointer */
#define	XDP		(((cell **)xup)[28])
#define	MAIN_DP	(((cell **)main_task)[28])
#define XV_DEBUGGING    xvar(29) /* nonzero if we're debugging */
#define XV_TICK_DEBUG   xvar(30) /* token when interpreted starts debugging */
#define XV_IP_L         xvar(31) /* low bounds of word to debug */
#define XV_IP_H         xvar(32) /* high bounds of word to debug */
#define XV_CNT          xvar(33) /* -1 = no support, 0 = off, 1 = on */
#define XV_MOD_COMP     xvar(34) /* -1 = compiling into module, 0 = dict */
#define	XV_TICK_TIB     xvar(35) /* Terminal Input Buffer pointer */
#define	XV_MOD_LINK     xvar(36) /* Head of linked list of named modules */
#define XV_SAVED_FP     xvar(37) /* Saved floating-point stack pointer */
#define XV_FPZERO       xvar(38) /* Top of floating-point stack */
#define	XV_TURNKEY		xvar(39)
#define	XV_NAMEDTASK	xvar(40)
#define	XV_PERTASK		xvar(41) /* Ptr to programmer-specified per-task data */
#define XV_SAVED_FRP    xvar(42) /* Saved frame pointer (for local variables) */
#define	XV_REGIONVAR	xvar(43) /* Reserved ptr for use by region DB code	*/
#define XV_CONTEXT      xvar(44) /* array @ specifying dict search order */
   /* 45-60 are part of context */
#define NVOCS 16			/* size of search order array. When changing this
							 * also change #vocs in util.fth
							 */
#define NEXT_VAR       (44+NVOCS)

#define	NTHREADS	1		/* # of link threads.  Also change #threads
							 * in util.fth
							 */


#define MAXUSER (0x200 * sizeof(cell))

#define INTERPRETING 0
#define COMPILING 1

#define FEOF -1

#define TIBSIZE 256			/* If you change this, change task-tibsize in tasking.m. */
#define PSSIZE 256
#define RSSIZE 256
#define FSTKLEN 32
#define MAXVARS 100

#define CBUFSIZE (256)
#define MAXLOCALS (32)
#define MAXSTRING (255)

#define	TWO_31ST	(2147483648.0)

/* Possibilities for exit status */
#define	MEMESUCCESSFULSTATUS			(0)
#define	MEMEINVALIDOPTIONSTATUS			(1)
#define	MEMEENVERRSTATUS				(2)
#define	MEMESIGNALSTATUS				(3)
#define	MEMECHECKSUMSTATUS				(4)
#define MEMEDICTBUILDSTATUS				(5)
#define	MEMEVIEWOUTERINTERPRETERSTATUS	(6)
#define	MEMEORPHANSTATUS				(7)
#define MEMEONEINSTANCESTATUS			(8)
#define	MEMENOMODULESTARTUPSTATUS		(9)
#define MEMEWINERRSTATUS				(10)	/* Windows-related */
#define	MEMEDICTALLOCSTATUS				(11)
#define MEMESERIALNUMSTATUS				(12)
#define	MEMERENDERERRSTATUS				(13)
#define	MEMEMEMALLOCSTATUS				(14)
#define	MEMEVIEWNOFILESTATUS			(15)

/* The type of network */
enum NetworkType {
	netNoNetwork,
	netFTPPacketDriver,					/* DOS only						*/
	netNDISDriver,						/* DOS only (never implemented) */
	netODIDriver,						/* DOS only (never implemented) */
	netWinsockDriver					/* Windows only					*/
};

/* The local name data structure */
struct local_name {
    u_char  name[33];
    token_t code;
    cell  data;
};

/* Dictionary entry structure */
struct d_e_t {
    struct d_e_t  *link;
    u_char         flags;
    u_char         name;
};
#define dict_entry_t struct d_e_t

/* Vocabulary data structure */
struct voc_t {
    token_t	  code_field;
    dict_entry_t *last_word;
    cell       * voc_link;
};
#define vocabulary_t struct voc_t

/* Union of cells and floating-point values */
union _fbits {
	double fb_float;
	unsigned long   fb_long[2];
};

#endif
