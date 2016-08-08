/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Extern declarations of global variables.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */
#ifndef __EXTERNS_H__
#define	__EXTERNS_H__
#ifdef SIGNALS
#include <setjmp.h>
#endif
#include "config.h"
#include "forth.h"
#include "meme.h"
#include "sernum.h"

extern	int			DebugLogging;

extern	int			comp_level;
extern	cell *		saved_dp;
extern	int			quietMode;			/* If non-zero console I/O is disabled.
										 * Set by the -q command line option.
										 */
extern	int			verboseMode;		/* If <> 0, print every line of source
										 * as it is interpreted.
										 */
extern	char		errMsg[];			/* For formatting error messages */
extern	int			NetworkType;		/* Type of network driver */
extern	unsigned long DummyIP;			/* Dummy IP # for when net driver=none.
										 * Stored in host byte order
										 */
extern	char		startupModuleName[];	/* Module to execute on startup */

extern	struct  AnomData DebuggerData;	/* Used by the anomaloscope */
extern	token_t		comp_buffer[];
extern	char *		version;
extern	u_char		tibbuf[];
extern	cell		parameter_stack[];
extern	token_t *	return_stack[];
extern	cell *		xsp;
extern	token_t **	xrp;
extern	double *	xfsp;
extern	cell *		xup;
extern	cell *		main_task_active;
extern	cell *		xfrp;
extern	double		floatstack[];
extern	cell		multitasking;
extern	int			nlocnames;
extern	struct local_name locnames[];
extern	int			locnum;
#ifdef SIGNALS
extern jmp_buf		env;
extern int			insane;      /* Talks with signal handler, if any. */
#endif
#ifdef RELOCATE
extern	u_cell		nrelbytes, nurelbytes;
extern	u_char		*relmap, *urelmap;
extern	u_char		bit[];
#endif
extern	u_char		nullrelmap[];
extern	cell *		origin;			/* Start of dictionary */
extern	cell *		dict_end;		/* End of dictionary */
extern	char		main_task[];	/* User area of main task */
extern	char		anti_task[];	/* User area when btwn tasks */
extern	u_char		wordbuf[];
extern	int			filesCount;
extern	char		**filesText;
extern	cell		initializeDict;
extern	u_char *	input_name;
extern	int			strCount;
extern	char		**strText;
extern	int			interpreterOnly;
extern	int			noMouse;
extern	Object		Objects[];
extern	cell		NumberCameras;
extern	CModule		Modules[];
extern	char		floatstr[];
extern	union	_fbits	fbits;
extern	cell		DeckModule;
extern	cell		messageSocket;
extern	char		leader[];
extern	ConfigBlock	CBlock;
extern	cell		NetFail;
extern	char 		argv0[];				/* Copy of argv[0]			*/

extern	char		*envMEME;
extern	char		*envMEMEFILES;
extern	char		*envMEMETMP;
extern	char		*oldRWHOME;
extern	int			hasRWHOME;
extern	char		*oldRWSHAPEPATH;
extern	int			hasRWSHAPEPATH;

extern	cell		maintaskReceiver;
extern	cell		currentDay;
extern	cell		currentMonth;
extern	cell		currentYear;

extern	char		cachefilename[];
extern	char		cachetmpfilename[];


extern	int			httpProxy;				/* Flag =TRUE if proxy		*/
extern	char		httpProxyHost[];		/* Proxy hostname			*/
extern	int			httpProxyPort;			/* Proxy portnumber			*/
extern	int			httpProxyIP;			/* Proxy IP number			*/

/*******************************************************************************
 * The current camera
 */
extern struct {
	void *		cam;		/* Renderer-specific pointer to the camera	*/
	cell		subtype;	/* Meme-specific camera subtype				*/
	void *		extraPtr;	/* Extra storage for a pointer				*/
} CurCam;

#endif	/* #ifndef __EXTERNS_H__ */
