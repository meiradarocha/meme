/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme initialization and cleanup routines.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <direct.h>
#include "forth.h"
#include "prims.h"
#include "externs.h"
#include "winextrn.h"
#include "protos.h"
#include "psprotos.h"

/* Prototypes */
void ExitMeme(int);

#ifdef MEMEVIEW

void
MemeUsage(str)
char *str;
{
	/******************************************************************/
	/* DO NOT REINDENT this code! It looks perfect in the dialog box. */
	/******************************************************************/

	strcpy(errMsg, str);
	strcat(errMsg, "\n");
	strcat(errMsg, "Memeview command-line usage:\n");
	strcat(errMsg, "memeview [-p hostname:port] filename\n\n");
	strcat(errMsg, "-p hostname:port#		Set http proxy host and port");
	MessageBox(0, errMsg, "Meme(tm) Viewer Command-Line Usage", MB_OK);
}

#else

void
MemeUsage(str)
char *str;
{
	/******************************************************************/
	/* DO NOT REINDENT this code! It looks perfect in the dialog box. */
	/******************************************************************/

	strcpy(errMsg, str);
	strcat(errMsg, "\n");
	strcat(errMsg, "Meme command-line options:\n\n");
	strcat(errMsg, "-f file1 [file2] ...		Load sourcefile[s] (must be the last option.)\n");
	strcat(errMsg, "-h			This list.\n");
	// strcat(errMsg, "-i			Interpreter only; no graphics, net, or serial.\n");
	strcat(errMsg, "-m modulename		Load and execute the named module file.\n");
	strcat(errMsg, "-n {winsock | none ip#}	Select the type of network and dummy IP number.\n");
	// strcat(errMsg, "-q			Quiet; no console output.\n");
	strcat(errMsg, "-p hostname:port#		Set http proxy host and port.\n");
	strcat(errMsg, "-v			Verbose; print source as it is interpreted.\n");
	strcat(errMsg, "-w modulename		WWW helper app mode; graphics only.");
	MessageBox(0, errMsg, "Meme(tm) Command-Line Usage", MB_OK);
}

#endif

#ifdef MEMEVIEW

void
ProcessCmdLineArgs(argcp, argvp, cmdline)
int *argcp;
char *(*argvp[]);
char *cmdline;
{
	char *p;

	NetworkType = netWinsockDriver;			/* Force Winsock driver */
	webMode = 1;							/* Force Web mode		*/

#ifdef REMOVE /* _DEBUG */
	/* Print argv */
	{
		int i;
		char msg[500];
		char msgline[80];
		char **av = *argvp;

		strcpy(msg, "Contents of argv:\n");
		
		for(i = 0; i < *argcp; i++) {
			sprintf(msgline, "argv[%d] = \042%s\042\n", i, av[i]);
			strcat(msg, msgline);
		}
		MessageBox(NULL, msg, "Dr. Stunch sez:", MB_OK);
	}
#endif
	/* Startup module filename is "" by default */
	*startupModuleName = '\0';

    /* Skip past the file name */
    *argvp += 1; *argcp -= 1;

	/* All we assume is that there is at least one cmd line argument remaining.
	 * If there isn't even one, display error message.
	 * If the file can't be opened, display error message.
	 */
	if (*argcp < 1) {
		MemeUsage("Error: Wrong number of command-line arguments\n"); 
		exit(MEMEINVALIDOPTIONSTATUS);
	}

	if (!strcmp((*argvp)[0], "-p")) {		/* http proxy host and port # */
		httpProxy = 1;
		*argvp += 1; *argcp -= 1;
		if ( (p = strchr((*argvp)[0], ':')) ) {
			memcpy(httpProxyHost, (*argvp)[0], p - (*argvp)[0]);
			httpProxyHost[p - (*argvp)[0] + 1] = '\0';
			if (httpProxyHost[0] == '\0' || httpProxyHost[0] == '-') {
				MemeUsage("Error: Invalid proxy hostname\n");
				exit(MEMEINVALIDOPTIONSTATUS);
			}
			httpProxyPort = atoi(p + 1);
			if (httpProxyPort == 0) {
				MemeUsage("Error: Invalid proxy port number\n");
				exit(MEMEINVALIDOPTIONSTATUS);
			}
		} else {
			MemeUsage("Error: no proxy port number specified");
			exit(MEMEINVALIDOPTIONSTATUS);
		}
		/* Skip past -p and host:port */
		*argvp += 1; *argcp -= 1;
	}
	/* All we assume is that there is at least one cmd line argument remaining.
	 * If there isn't even one, display error message.
	 * If the file can't be opened, display error message.
	 */
	if (*argcp < 1) {
		MemeUsage("Error: Wrong number of command-line arguments\n"); 
		exit(MEMEINVALIDOPTIONSTATUS);
	}
	webMode = 1;

	/* Because Windows doesn't do the right thing to elements of argv
	 * when a filename has a blank in it, find the filename in the
	 * raw command line string, and use everything from the filename
	 * to the end of line as the full filename.
	 */
	p = strstr( cmdline, (*argvp)[0] );
	strcpy(startupModuleName, p);
}

#else

void
ProcessCmdLineArgs(argcp, argvp, cmdline)
int *argcp;
char *(*argvp[]);
char *cmdline;
{
	char msg[80];
	cell ip1, ip2, ip3, ip4;
	char *p;

    /* Start with config block default values */
    NetworkType = CBlock.defaultNet;
    noMouse = CBlock.defaultNoMouse;	/* Unused in Windows version */
	DummyIP = CBlock.dummyIP;

	/* Startup module filename is "" by default */
	*startupModuleName = '\0';

    /* Skip past the file name */
    *argvp += 1; *argcp -= 1;

	/* Special case because Windows 3.1 is too hosed to accept command-line switches
	 * for applications that have an "association" with a document of a particular
	 * filename extension.  If there is a single argument on the command line, its
	 * extension is examined for .mod, and it is executed in -w mode, i. e. like a WWW helper
	 * app with no edit window available.
	 */
	if (*argcp == 1) {
		strcpy(msg, (*argvp)[0]);
		if ( (p = strstr(msg, ".mm")) == NULL ) {
			p = strstr(msg, ".MM");
		}
		if (p != NULL) {
			if (p != msg && p[4] == ' ' || p[4] == '\t' || p[4] == '\0') {
				/* We got a filename ending in .mm */
				webMode = 1;
				strcpy(startupModuleName, msg);
				return;
			}
		}
	}

    /* While there are command line args left */
    while (*argcp) {
		if (!strcmp((*argvp)[0], "initdict")) {
		    initializeDict = -1;
		} else if ((*argvp)[0][0] == '-') {
		    switch( (*argvp)[0][1] ) {
				case 'h':	/* Print usage list */
				    interpreterOnly = 1;	/* Show on cmd line */
				    MemeUsage("");
				    exit(MEMESUCCESSFULSTATUS);
#ifdef REMOVE
				case 'i':
				    interpreterOnly = -1;
				    break;
				case 'q':	/* Disable console text output */
				    quietMode = 1;
				    break;
#endif
				case 'v':	/* Print every line of interpreted source */
				    verboseMode = 1;	
				    break;
				case 'n':
				    *argvp += 1; *argcp -= 1; /* Skip to next arg */
					if (!strcmp( (*argvp)[0], "winsock" )) {
						NetworkType = netWinsockDriver;
				    } else if (!strcmp( (*argvp)[0], "none" )) {
						NetworkType = netNoNetwork;
						*argvp+= 1; *argcp -= 1;	/* Skip to next arg */
						if (!*argcp) {
							interpreterOnly = 1;
							MemeUsage("Error: no dummy IP number specified after -n none\n");
							exit(MEMEINVALIDOPTIONSTATUS);
						}
						if (sscanf((*argvp)[0], "%d.%d.%d.%d", &ip1, &ip2,
														&ip3, &ip4) != 4) {
							interpreterOnly = 1;
							sprintf(msg, "Error: invalid dummy IP number %s\n", (*argvp)[0]);
							MemeUsage(msg);
							exit(MEMEINVALIDOPTIONSTATUS);
						}

		// The dummy IP number is stored in host byte order
		#ifdef BIG_ENDIAN
						DummyIP = (ip1 << 24 & 0xff000000) |
							(ip2 << 16 & 0xff0000) | (ip3 << 8 & 0xff00) |
							(ip4 & 0xff);
		#else
						DummyIP = (ip4 << 24 & 0xff000000) |
							(ip3 << 16 & 0xff0000) | (ip2 << 8 & 0xff00) |
							(ip1 & 0xff);
		#endif
					} else {
						interpreterOnly = 1;	/* Show on cmd line */
						sprintf(msg, "Error: Invalid network type: %s\n", (*argvp)[0]);
						MemeUsage(msg);
						exit(MEMEINVALIDOPTIONSTATUS);
				    }
				    break;
				case 'm':
					*argvp += 1; *argcp -= 1;	/* Skip to next arg */
					if ( (*argvp)[0][0] == '-' ) {
						MemeUsage("Error: Invalid filename specified after -m\n");
						exit(MEMEINVALIDOPTIONSTATUS);
					}

					/* Because Windows doesn't do the right thing to elements of argv
					 * when a filename has a blank in it, find the filename in the
					 * raw command line string, and use everything from the filename
					 * to the end of line as the full filename.
					 */
					p = strstr( cmdline, (*argvp)[0] );
					strcpy(startupModuleName, p);
					break;
				case 'f':
				    filesCount = *argcp;
				    filesText = *argvp;
				    return;
#ifdef REMOVE
				case 's':
				    strCount = *argcp;
				    strText = *argvp;
				    return;
#endif
				case 'w':		/* Web browser helper app mode */
					webMode = 1;
					*argvp += 1; *argcp -= 1;	/* Skip to next arg */
					if ( (*argvp)[0][0] == '-' ) {
						MemeUsage("Error: Invalid filename specified after -w\n");
						exit(MEMEINVALIDOPTIONSTATUS);
					}
					/* Because Windows doesn't do the right thing to elements of argv
					 * when a filename has a blank in it, find the filename in the
					 * raw command line string, and use everything from the filename
					 * to the end of line as the full filename.
					 */
					p = strstr( cmdline, (*argvp)[0] );
					strcpy(startupModuleName, p);
					break;
				case 'p':		/* http proxy host and port # */
					httpProxy = 1;
					*argvp += 1; *argcp -= 1;
					if ( (p = strchr((*argvp)[0], ':')) ) {
						memcpy(httpProxyHost, (*argvp)[0], p - (*argvp)[0]);
						httpProxyHost[p - (*argvp)[0] + 1] = '\0';
						if (httpProxyHost[0] == '\0' || httpProxyHost[0] == '-') {
							MemeUsage("Error: Invalid proxy hostname\n");
							exit(MEMEINVALIDOPTIONSTATUS);
						}
						httpProxyPort = atoi(p + 1);
						if (httpProxyPort == 0) {
							MemeUsage("Error: Invalid proxy port number\n");
							exit(MEMEINVALIDOPTIONSTATUS);
						}
					} else {
						MemeUsage("Error: no proxy port number specified");
						exit(MEMEINVALIDOPTIONSTATUS);
					}					
					break;
				default:
				    interpreterOnly = 1;	/* Show on cmd line */
				    sprintf(msg, "Invalid command line argument: %s\n", (*argvp)[0]);
				    MemeUsage(msg);
				    exit(MEMEINVALIDOPTIONSTATUS);
			}
		} else {
			interpreterOnly = 1;	/* Show on cmd line */
			sprintf(msg, "Invalid command line argument: %s\n", (*argvp)[0]);
			MemeUsage(msg);
			exit(MEMEINVALIDOPTIONSTATUS);
		}
		*argvp += 1;
		*argcp -= 1;
    }
}

#endif

void
SetUpInterpreter()
{
    char dictionary_fname[256];		/* Nice 'n' long */

    allocate_dictionary((unsigned cell)DICT_SIZE);
    xup = (cell *)main_task;
    if (initializeDict) {
		init_dictionary();
    } else {					/* Load dictionary from file */
	    strcpy(dictionary_fname, envMEME);

		/* Don't append a backslash if there's already one on the end of the
		 * environment variable; it fools Microsoft OSes into looking for
		 * a hostname in the first part of the path.
		 */
		if (dictionary_fname[strlen(dictionary_fname) - 1] != '\\') {
	    	strcat(dictionary_fname, "\\");
		}
	    strcat(dictionary_fname, DEFAULT_DIC);
		read_dictionary(dictionary_fname);
    }
    init_variables();
	check_expiry();
    init_io();

}


/* Set up the environment variables.
 * If MEME is defined, it is used as the path name for the dictionary
 * file.  If not defined, the current directory is searched, then all dirs
 * in the operating systems PATH env variable, and finally c:\meme is checked.
 * If still not found, exit with error message.
 * It is the path to the directory where MEME.EXE and MEME.DIC
 * are defined.
 * If MEMEFILES is defined, it is used as the path name for the shape files,
 * else MEMEFILES is set to the same value as MEME.
 * If MEMETMP is defined, it is used as the path name for the shape files,
 * else MEMETMP is set to the same value as MEME.
 * RWHOME is first checked to see if there was a previous value set.  It is
 * then set to the same value as MEME.
 * RWSHAPEPATH is first checked to see if there was a previous value set.  It
 * is then set to a concatenation of the old value and MEMEFILES.
 */
void
SetUpEnv(void)
{
    char *p;
	char destenvstr[512];
	char pathstr[512];
	static int BeenHereBefore = 0;

	if (BeenHereBefore) return;				/* Don't run twice */
	BeenHereBefore = 1;

	/* If the environment variable MEME is not defined, search for
	 * meme.exe in the PATH and current directory.  Set the MEME env
	 * var to the resulting directory.  If not found anywhere in the
	 * PATH, exit.
	 */
	if ((p = getenv("MEME")) == 0) {
		p = getcwd(NULL, 512);
		if (p) {
			strcpy(pathstr, p);
			strcat(pathstr, ";");
			free(p);
		} else {
			*pathstr = '\0';
		}
		if (p = getenv("PATH")) {
			strcat(pathstr, p);
		}
#ifdef MEMEVIEW
		strcat(pathstr, ";");
		strcat(pathstr, DEFAULT_DIR);
#endif
		PathName(DEFAULT_EXE, destenvstr, pathstr);
		if (*destenvstr == '\0') {		/* executable not found anywhere in the path */

			fatal("Please define the MEME environment variable.  Its value should be\n\
the name of the directory containing the Meme program files.", MEMEENVERRSTATUS);
		}
		p = strrchr(destenvstr, '/');
		if (p) {
			*p = '\0';
		} else {
			*destenvstr = '\0';
		}
		envMEME = strdup(destenvstr);
	} else {
		envMEME = strdup(p);
	}

    /* Put the value of "MEMEFILES" in envMEMEFILES */
    if ((p = getenv("MEMEFILES")) == 0) {
		strcpy(destenvstr, ".;");
		strcat(destenvstr, envMEME);
		envMEMEFILES = strdup(destenvstr);	/* MEMEFILES set to "curr dir;MEME" if not defined */
    } else {
		envMEMEFILES = strdup(p);
    }

	/* Put the path for temporary files in envMEMETMP */
	if (p = getenv("MEMETMP")) {			/* If MEMETMP is defined, use it */
		envMEMETMP = strdup(p);
	} else if (p = getenv("TMP")) {			/* Else if TMP is defined, use it */
		envMEMETMP = strdup(p);
	} else if (p = getenv("TEMP")) {		/* Else if TEMP is defined, use it */
		envMEMETMP = strdup(p);
	} else {								/* Otherwise,    */
		envMEMETMP = strdup(envMEME);		/* MEMETMP same as MEME if not defined */
	}

    /* Get the old values for RWHOME and RWSHAPEPATH, if any. */
    if (p = getenv("RWHOME")) {
		oldRWHOME = strdup(p);
		hasRWHOME = 1;
    }
    if (p = getenv("RWSHAPEPATH")) {
		oldRWSHAPEPATH = strdup(p);
		hasRWSHAPEPATH = 1;
    }

	strcpy(destenvstr, "RWHOME=");
	strcat(destenvstr, envMEME);
	if (_putenv(strdup(destenvstr)) != 0) {
		fatal("Can't set environment variable RWHOME.", MEMEENVERRSTATUS);
    }
    if (hasRWSHAPEPATH) {
		if ( (p = malloc(512)) == NULL) {
		    fatal("Can't allocate string space in SetUpEnv().", MEMEENVERRSTATUS);
        }
		strcat(p, envMEMEFILES);
		strcat(p, ";");
		strcpy(p, oldRWSHAPEPATH);
		strcpy(destenvstr, "RWSHAPEPATH=");
		strcat(destenvstr, p);
		if (_putenv(strdup(destenvstr)) != 0) {
		    fatal("Can't set environment variable RWSHAPEPATH.", MEMEENVERRSTATUS);
		}
		free(p);
    } else {	/* RWSHAPEPATH not already defined */
		strcpy(destenvstr, "RWSHAPEPATH=");
		strcat(destenvstr, envMEMEFILES);
		if (_putenv(strdup(destenvstr)) != 0) {
		    fatal("Can't set environment variable RWSHAPEPATH.", MEMEENVERRSTATUS);
		}
    }
}
    
void
CleanUpEnv(void)
{
	/* Don't need to set RWHOME and RWSHAPEPATH to their former
	 * values under NT, since we only modify this app's local
	 * environment.  For other operating systems, we have to
	 * restore RWHOME and RWSHAPEPATH to their former values.
	 */

	/* Don't free anything in here.  If we're cold restarting, we
	 * want these vars preserved.  If we're exiting, we don't care
	 * about freeing them.
	 */
#ifdef REMOVE
	free(envMEME);
	free(envMEMEFILES);
	free(envMEMETMP);
	if (hasRWHOME) free(oldRWHOME);
	if (hasRWSHAPEPATH) free(oldRWSHAPEPATH);
#endif
}
    
int
WindowsSetUpMeme(int *argcp, char *(*argvp[]), 
					HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	char ModuleStartupMeme[256];
	char *namep;
//	HANDLE myThread;
	int setjmprtn;

L("WindowsSetUpMeme entry point")

    xup = (cell *)main_task;			/* Make sure emit can use user variables */
 	main_task_active = &XV_TASK_ACTIVE;	/* Initialize a global */

L("Calling ProcessCmdLineArgs")

	ProcessCmdLineArgs(argcp, argvp, (char *)lpszCmdLine);

L("Returned from ProcessCmdLineArgs")

//
// Initialization of the windows, etc.
//
	hPrevInstance = hPrevInstance;
	lpszCmdLine = lpszCmdLine;			/* Shut up warnings */
	
	theInstanceHandle = hInstance;

L("Calling GetVersionEx")

	// Before anything else, get Windows version information so
	// we know what services are available.
	winVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&winVersion);
	win32s = winVersion.dwPlatformId == VER_PLATFORM_WIN32s;

L("Returned from GetVersionEx")


	if (!win32s) {
		mainThreadId = GetCurrentThreadId();
		
#ifdef REMOVE
		myThread = GetCurrentThread();
		SetThreadPriority(myThread, THREAD_PRIORITY_BELOW_NORMAL);
#endif
	}	
	if ( FindWindow(GraphicsClass, GraphicsTitle) || FindWindow(GraphicsClass, GraphicsViewTitle) ) {
		fatal("There is already a copy of Meme running on this machine.", 1);
	}

    if (!hPrevInstance) {

L("Calling FirstInstance")

		if (!FirstInstance(hInstance)) return(FALSE);

L("Returned successfully from FirstInstance, calling AnyInstance");

	}
    if ( !AnyInstance(hInstance, nCmdShow) ) return(FALSE);

L("Returned from AnyInstance")
//
// End init of windows
//

L(">SetUpEnv")

    SetUpEnv();

L("<SetUpEnv")

#ifdef SIGNALS
# ifndef _DEBUG
	init_signals();
# endif
L(">setjmp")
	while ( (setjmprtn = setjmp(env)) != 0 ) {
L("<setjmp in WindowsSetUpMeme")
L(">report_signal")
		if (setjmprtn == SETJMPRTNCOLDSTART) {
			cprint("\n\nCold start\n\n");
		} else {
			report_signal();
		}
L("<report_signal")
# ifndef _DEBUG			/* Disable interception of signals in debug code, so
						 * just-in-time debugging can be started when the
						 * exception occurs.
						 */
		init_signals();
# endif
		free_dictionary();				/* Free the dictionary space		 */
		xup = (cell *)main_task;	/* Make sure emit can use user variables */
		CleanUpRFiles();
		if (!interpreterOnly) {
		    CleanUpMouse();
		    CleanUpRendering();
		    CleanUpNetwork();
		    CleanUpSerial();
		}
    }
#endif
    if (!interpreterOnly) {
L(">SetUpMouse")
		SetUpMouse();
L("<SetUpMouse >SetupSerial")
		SetUpSerial();
L("<SetUpSerial >SetupNetwork")
		SetUpNetwork();
L("<SetUpNetwork >SetupRendering")
		SetUpRendering((void *)theGraphicsWindow);
L("<SetUpRendering >SetupRFiles")
   }
L(">SetupRFiles")
	SetUpRFiles();
L("<SetUpRFiles >SetupInterpreter")
    SetUpInterpreter();
L("<SetUpInterpreter")
	if (webMode) {

		XV_STATE = (cell)INTERPRETING;
		xsp = (cell *)XV_SPZERO;
		xrp = (token_t **)XV_RPZERO;
		xfsp = (double *)XV_FPZERO;

L(">Execute the web mode command-line module argument")
		strcpy(&ModuleStartupMeme[1], "ExecuteStartupModule");
		ModuleStartupMeme[0] = (char)strlen(&ModuleStartupMeme[1]);
		namep = ModuleStartupMeme;
		if (find((cell *)&namep)) {
			*--xsp = (cell)startupModuleName;
			execute_one((token_t)namep);
			if (XV_PERTASK == 0) {
				char errMsg[256];
				sprintf(errMsg, "Couldn't start %s", startupModuleName);
				fatal(errMsg, 1);
			}
		} else {
			fatal("Module start-up internal error", MEMENOMODULESTARTUPSTATUS);
		}
		for (;;) {
			execute_one(PAUSE);
		}
L("< Should never get here--return from webmode cmd-line module argument")
	}

#ifndef MEMEVIEW

	if (*startupModuleName) {				/* If we have a module to start */

		XV_STATE = (cell)INTERPRETING;
		xsp = (cell *)XV_SPZERO;
		xrp = (token_t **)XV_RPZERO;
		xfsp = (double *)XV_FPZERO;

L(">command-line module argument")
		strcpy(&ModuleStartupMeme[1], "ExecuteStartupModule");
		ModuleStartupMeme[0] = (char)strlen(&ModuleStartupMeme[1]);
		namep = ModuleStartupMeme;
		if (find((cell *)&namep)) {
			cprint("Executing module ");
			cprint(startupModuleName);
			cprint("\n");
			*--xsp = (cell)startupModuleName;
			execute_one((token_t)namep);
			if (XV_PERTASK == 0) {
				char errMsg[256];
				sprintf(errMsg, "Couldn't start %s\n", startupModuleName);
				error(errMsg);
			}
L("<command-line module argument")
		} else {
			error("Start-up module wrapper code not found\n");
		}
L(">outer interpreter")
		outer_interpreter();	/* Never returns */
L("<outer interpreter (never gets here)")

    } else if (XV_TURNKEY) {
L(">turnkey meme")
		XV_STATE = (cell)INTERPRETING;
		xsp = (cell *)XV_SPZERO;
		xrp = (token_t **)XV_RPZERO;
		xfsp = (double *)XV_FPZERO;
		execute_one(XV_TURNKEY);	/* I think this never returns */
L("<turnkey meme--shouldn't get here")
		cprint("Meme exiting.\n");
    } else {
L(">outer interpreter...")
		outer_interpreter();	/* Never returns */
L("<outer interpreter...(never gets here)")
    }

#endif	/* notdef MEMEVIEW */

L(">ExitMeme after outer interp");
    ExitMeme(0);
L("<Return from ExitMeme after outer interp!!!!!!!!!");
}

void
CleanUpMeme(void)
{
#ifdef CMAN
    CleanUpCyberMan();
#endif
    if (!interpreterOnly) {
		CleanUpMouse();
		CleanUpSerial();
		CleanUpRendering();
		CleanUpNetwork();
		CleanUpSerial();
    }
    CleanUpEnv();
}

void
ExitMeme(int rtnval)
{
	// If we're loading from a file, make sure we leave any error message 
	// on the screen long enough for the user to see.
	if (filesCount && !webMode) {
		while (DQ) ;	/* Empty the keystroke queue */
		cprint ("Press any key...");
		do {
			MemeOften();
		} while (!KEYAVAIL) ;
	}
	if (win32s) {	// Fine if there's only one thread
		CleanUpMeme();
		exit(rtnval);
	} else {
		if (GetCurrentThreadId() == mainThreadId) {
			exitWParam = rtnval;
			longjmp(exitjmpbuff, 1);
		} else {
			PostThreadMessage(mainThreadId, WM_QUIT, rtnval, 0);
			Sleep(INFINITE);
		}
	}
}
