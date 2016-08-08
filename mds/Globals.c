/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Global variables for Meme.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */
#ifdef SIGNALS
#include <setjmp.h>
#endif
#include <stdio.h>
#include "config.h"
#include "forth.h"
#include "meme.h"
#include "memever.h"
#include "sernum.h"




/*
 *
 * Debug logging.  Enabled if non-zero.  See config.h.
 * Also enabled with command-line option.
 *
 */
int					DebugLogging = 0;




int					comp_level;
cell				*saved_dp;
int					quietMode = 0;	/* If non-zero console I/O is disabled.
									 * Set by the -q command line option.
									 */
int 				verboseMode = 0;	/* If <> 0, print every line of source
										 * as it is interpreted.
										 */
char				errMsg[1024];		/* For formatting error messages */
struct AnomData		DebuggerData;			/* Used by the anomaloscope */
token_t				comp_buffer [CBUFSIZE];
char				*version = VERSION;
u_char				tibbuf[TIBSIZE];
cell				parameter_stack[PSSIZE+4];	/* Leave 4 "slop" elements */
token_t				*return_stack[RSSIZE+4];	/* Leave 4 "slop" elements */
cell				*xsp;
token_t				**xrp;
double				*xfsp;
cell				*xup;
cell				*xfrp = 0;					/* Frame pointer */
cell				*main_task_active;			/* Ptr to task_active */
double				floatstack[FSTKLEN+4];		/* Leave 4 "slop" elements */
cell				multitasking;
int					nlocnames;
struct local_name	locnames[MAXLOCALS];
int					locnum;
#ifdef RELOCATE
u_cell				nrelbytes, nurelbytes;
u_char				*relmap, *urelmap;
u_char				bit[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
#endif
u_char				nullrelmap[1] = { 0 };
cell				*origin;			/* Start of dictionary */
cell				*dict_end;			/* End of dictionary */
char 				main_task[MAXUSER];	/* User area of main task */
char				anti_task[MAXUSER];	/* User area when btwn tasks */
u_char 				wordbuf[MAXSTRING + 2];
int					filesCount = 0;
char				**filesText = NULL;
cell				initializeDict = 0;
u_char				*input_name = "";
int					strCount = 0;
char				*strText = NULL;
int					interpreterOnly = 0;
int					NetworkType = netNoNetwork;	/* Type of network driver */
int					noMouse = 0;
unsigned long		DummyIP = 0;
char				startupModuleName[64];

Object				Objects[MAXOBJECTS]; 	/* Array of Meme objects */
cell				NumberCameras = 0;		/* # of cameras that now exist */

CModule				Modules[MAXMODULES];	/* Array of modules */
char				floatstr[50];			/* For formatting fp numbers */
union _fbits		fbits;

/* Variables used for sending and receiving messages */
cell				DeckModule = 0;

/* Configuration/serial number block */
ConfigBlock			CBlock = {
	LEADER,
	{
		0, 0, 0, 0,		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,

		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,

		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,

		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0,
		0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0, 	0, 0, 0, 0
	},
	0,
	netNoNetwork, 0, 0
};


cell			NetFail = 999;		/* Flag for net initialization			*/
char 			argv0[128];			/* Copy of argv[0]						*/

char			*envMEME;
char			*envMEMEFILES;
char			*envMEMETMP;
char			*oldRWHOME;
int				hasRWHOME = 0;
char			*oldRWSHAPEPATH;
int				hasRWSHAPEPATH = 0;

cell			maintaskReceiver = 0;
cell			currentDay;
cell			currentMonth;
cell			currentYear;

char			cachefilename[128];
char			cachetmpfilename[128];

int				httpProxy = 0;			/* Flag =TRUE if proxy		*/
char			httpProxyHost[100];		/* Proxy hostname			*/
int				httpProxyPort = 0;		/* Proxy portnumber			*/
int				httpProxyIP = 0;		/* Proxy IP number			*/


/*******************************************************************************
 * The current camera
 */
struct {
	void *		cam;		/* Renderer-specific pointer to the camera	*/
	cell		subtype;	/* Meme-specific camera subtype				*/
	void *		extraPtr;	/* Extra storage for a pointer				*/
} CurCam;

