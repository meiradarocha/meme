/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* config.h for Meme.
 * by Marc de Groot.
 * Portions copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/***********************************
 *
 * THIS FILE EVENTUALLY NEEDS TO MIGRATE TO THE PARENT DIRECTORY.
 * IT SHOULD BE MADE TO HOLD THE DEFINITIONS FOR ALL PLATFORMS.
 *
 **********************************/

#ifndef	__CONFIG_H__
# define	__CONFIG_H__

/* Debug logging */
/* To enable, set DebugLogging to be non-zero in GLOBALS.c
 * or use the command-line option.  See WinMain()in the Win32 version.
 */
# ifdef	_DEBUG	/* MSVC++ defines this flag when compiling the Win32 Debug target */
#  define	L(x)	{ if (DebugLogging) DebugLog( (x) ); }
# else
#  define	L(x)
# endif

/* Define SIGNALS for the DOS/WATCOM version */
# define	SIGNALS

/* Set the rendering engine from this list */
# define	NO_RENDERING	0
# define	RENDERWARE		1
# define	REND386			2
# define	RENDERING		RENDERWARE

/* If your system does not have the "syscall()" function, un-comment the
 * following line.
 */
/* #define NOSYSCALL */

# define COMOUT

# define	RELOCATE

/*
 * This file contains definitions which configure C Forth 83 for specific
 * processors, operating systems, compilers, and memory models.
 *
 * IMPORTANT NOTE: The preferred place to set the basic configuration
 * parameters (the OS name and the number of bits) is in the Makefile
 * for the particular machine.  However, some C development systems
 * don't have "make", and some C compilers don't allow defines (-D)
 * on the command line.  Consequently, it may be necessary to edit this
 * file to enable the appropriate #defines.
 */

/*
 * Select the operating system.  
 */
/* #define UNIX */
/* #define DOS */
/* #define VMS */
/* #define MAC */
# define WINDOWS

/* Select the endianness of the processor.  Intel (80x86 and Pentium) are little-endian,
 * VAX is little-endian, 68k is big-endian, SPARC is big-endian, and damn near everything else
 * is big-endian
 */
# define LITTLE_ENDIAN
/* # define BIG_ENDIAN */


/* Include support for Logitech CyberMan under DOS */
# ifdef	DOS
#  define	CMAN
# endif

/*
 * If ALLOCDICT is defined, the space for the Forth dictionary will
 * be dynamically allocated at startup time.  Otherwise, the dictionary
 * will be a static array in the BSS section of the program.
 * On a system where the BSS does not take up space in the program
 * file (e.g. Unix), it is often better to leave ALLOCDICT undefined.
 * On a system where the BSS occupies file space (e.g. DOS), it is
 * often better to define ALLOCDICT.
 */
/* #define ALLOCDICT */

/* #define void int */	/* Define this for compilers without "void" */

# ifdef UNIX

/* Define SIGNALS to catch Unix signals (keyboard interrupts, errors, etc) */
/* If left undefined, signals will cause the Forth process to terminate */
#  define SIGNALS

/*
 * Note: It is okay to define neither BSD nor SYSV.  The only result will
 * be that the word "key?" will always return false.  This is not a serious
 * problem; all of the utilities supplied with C Forth will still work.
 * The down side is that you won't be able to stop "words" by typing a key.
 */
/* Define BSD to make key? work right on 4.2BSD systems. */
/* #define BSD */

/* Define SYSV to make key? work right on System V systems */
/* #define SYSV */

# endif

# ifndef VMS
#  define EXITSTATUS 0
# else
#  define EXITSTATUS
/* Mods reported by Norman Smith */
#  define system vms_system
#  define chdir  vms_chdir
# endif

/* Allocate this number of bytes for the dictionary */
# define DICT_SIZE (0x40000L)	/* XXX should be somewhere else */


/******** FILE NAMES **********/

/* INIT_FILENAME is the name of the file from which the dictionary
 * initialization definitions are read the first time that forth is
 * started.  On some systems, it may be necessary to set a specific pathname.
 * The first character in the name is the number of remaining characters.
 * Remember that in a C string, " \nn " represents an *octal* number.
 *
 * DEFAULT_DIC is the name of the default dictionary file.
 * This file is created by the build procedure after the extensions are
 * loaded.  New dictionary files are produced by the SAVE-DICTIONARY command.
 *
 * DEFAULT_EXE is the name of the default executable file
 */
# if defined(DOS) || defined(WINDOWS)			/* Microsoft's offerings	*/

#  define	INIT_FILENAME	"\11..\\init.x"
#  define	DEFAULT_DIR		"c:\\meme"

#  ifdef	MEMEVIEW
#   define	DEFAULT_DIC		"memeview.dic"
#   define	DEFAULT_EXE		"memeview.exe"
#  else
#   define	DEFAULT_DIC		"meme.dic"
#   define	DEFAULT_EXE		"meme.exe"
#  endif

# endif



# ifdef UNIX									/* Thompson and Ritchie's	*/

#  define	INIT_FILENAME	"\11../init.x"
#  define	DEFAULT_DIR		"/usr/local/lib/meme"

#  ifdef	MEMEVIEW
#   define	DEFAULT_DIC		"memeview.dic"
#   define	DEFAULT_EXE		"memeview"
#  else
#   define	DEFAULT_DIC		"meme.dic"
#   define	DEFAULT_EXE		"meme"
#  endif

# endif



# ifdef VMS										/*     DEC's				*/

#  define	INIT_FILENAME  "\14disk$usr0:[forth.source]init.x"
#  define	DEFAULT_DIR		"disk$usr0:meme"	/* I don't know if this is right */

#  ifdef	MEMEVIEW
#   define	DEFAULT_DIC		"memeview.dic"
#   define	DEFAULT_EXE		"memeview"
#  else
#   define	DEFAULT_DIC		"meme.dic"
#   define	DEFAULT_EXE		"meme"
#  endif

# endif



# ifndef	INIT_FILENAME						/* Everything else			*/

#  define	INIT_FILENAME	"\11../init.x"
#  define	DEFAULT_DIR		"/usr/local/lib/meme"

#  ifdef	MEMEVIEW
#   define	DEFAULT_DIC		"memeview.dic"
#   define	DEFAULT_EXE		"memeview"
#  else
#   define	DEFAULT_DIC		"meme.dic"
#   define	DEFAULT_EXE		"meme"
#  endif

# endif



/*
 * Both token_t and cell should be big enough to hold an absolute
 * address on your machine.
 */
# ifdef __ZTC__
/*
 * Zortech C tries to copy the segment register too when
 * asked to assign a pointer value to a "long" lvalue.
 */
#  define token_t unsigned int
#  define cell int
#  define int32 long
#  define int16 short
#  define	uint32 unsigned long
#  define uint16 unsigned short
# else
#  define token_t unsigned long
#  define cell long
#  define int32 long
#  define int16 short
#  define	uint32 unsigned long
#  define uint16 unsigned short
# endif

/*
 * What boundary to align a "cell" to when it is stored in memory.
 * On a 68000, this has to be at least 2; on a VAX or a 68020,
 * it can be 1 if you want to conserve memory at the expense of
 * speed (VAXen and 68020's will fetch unaligned data without complaint,
 * but it takes longer), or 4 if you want maximum speed.
 * If in doubt, use sizeof(cell).
 */
/*
 * Right now this has to be a multiple of sizeof(cell), because the
 * branch code in the kernel uses branch displacements measured in
 * units of 1 cell.
 */

# define ALIGN_BOUNDARY (sizeof(cell))

/*
 * SNEWLINE is the end-of-line sequence for the operating system's files.
 * CNEWLINE is the LAST character in the end-of-line sequence
 */
# ifdef UNIX
#  define SNEWLINE "\n"	/* Unix */
#  define CNEWLINE '\n'
# endif

# ifdef MAC
#  define SNEWLINE "\r"	/* MAC, OS-9, UNIFLEX, ... */
#  define CNEWLINE '\r'
# endif

# ifndef SNEWLINE
#  define SNEWLINE "\r\n"	/* MS-DOS, RT-11, VMS? */
#  define CNEWLINE '\n'
# endif

/*
 * File-related stuff.
 */

# ifdef UNIX
/* Some Unix systems don't allow the "b" modifier; no Unix system needs it */
#  define READ_MODE   "r"
#  define WRITE_MODE  "w"
#  define MODIFY_MODE "r+"
# endif

# ifndef READ_MODE
#  define READ_MODE   "rb"
#  define WRITE_MODE  "wb"
#  define MODIFY_MODE "rb+"
# endif

/* NOTE - #defines below were removed for MSVC.  Associated code is in MEMEFILE.c */
// /* Because Watcom has a problem with fread() */
// #ifdef WINDOWS
// # define	fread		mfread
// # define	fwrite		mfwrite
//
/* These are the file modes passed to mfopen() */
// # define	M_RD		(0)
// # define	M_WR		(1)
// # define	M_RDWR		(2)
// #endif

/*
 * Network-related stuff
 */
typedef		unsigned long	HOSTID;		/* Datatype for an IP number or other host ID number */

#endif	/* #ifdef __CONFIG_H__ */

