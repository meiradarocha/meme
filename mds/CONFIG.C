/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Meme command-line parameter defaults configuration.
 * By Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sernum.h"
#include "config.h"
#ifndef BSD
#include "malloc.h"
#endif

void	Usage();

#define	UNUSED	(-3)
int	Net = UNUSED;
int	NoMouse = UNUSED;
unsigned long dummyIP = 0;

char FileName[80];				/* Name of the executable file to configure */

void
ModifyConfigBlock(file)
char *file;
{
	char leader[LEADERLEN] = LEADER;
	long size;
	char *buff;
	char *buffp;
	FILE *f;
	int i;

	if ( (f = fopen(file, "r+b")) == NULL ) {
		printf("Can't open %s, exiting\n", file);
		exit(1);
	}
	fseek(f,0L,2); size = ftell(f); fseek(f, 0L, 0);
	buffp = buff = (char *)malloc(size);
	if (buff == NULL) {
		printf("Can't malloc enough space for %s, exiting.\n",
							file);
		exit(1);
	}
	fread(buff, (size_t)1, (size_t)size, f);
	if (ferror(f)) {
		printf("Can't read %s, exiting.\n", file);
		exit(1);
	}
	for (i = 0; i < size - LEADERLEN + 1; i++, buffp++) {
		if (!memcmp(buffp, leader, LEADERLEN)) {
			if (Net != UNUSED) {
				((ConfigBlock *)buffp)->defaultNet = Net;
				if (Net == netNoNetwork) {
					((ConfigBlock *)buffp)->dummyIP = dummyIP;
				}
			}
			if (NoMouse != UNUSED) {
				((ConfigBlock *)buffp)->defaultNoMouse = 
								NoMouse;
			}
			fseek(f, 0L, 0);
			fwrite(buff, (size_t)1, (size_t)size, f);
			if (ferror(f)) {
				printf("Error writing file, exiting\n");
				exit(1);
			}
			fclose(f);
			printf("Configuration successful.\n");
			exit(0);
		}
	}
	printf("Didn't find the config block leader, exiting.\n");
	exit(1);
}

void
ProcessCmdLine(argc, argv)
int argc;
char *argv[];
{
    int i;
	cell ip1, ip2, ip3, ip4;

#if defined(DOS) || defined(WINDOWS)
    strcpy(FileName, "meme.exe");	/* Default name */
#else
    strcpy(FileName, "meme");		/* Default name */
#endif
    if (argc < 2) {
		Usage();
		exit(1);
    }
    argv++;
    for (i = 1; i < argc; i++, argv++) {
		if (*argv[0] == '-') {
#ifdef DOS
		    if (!strcmp(*argv, "-nomouse")) {
				NoMouse = -1;
		    } else if (!strcmp(*argv, "-mouse")) {
				NoMouse = 0;
		    } else 
#endif
			if (!strcmp(*argv, "-f")) {
				argv++; i++;
				if (i >= argc) {
				    printf("No filename argument to -f option.  Config failed.\n");
				    exit(1);
				}
				strcpy(FileName, *argv);
		    } else if (!strcmp(*argv, "-n")) {
				if (!strcmp(argv[1], "none")) {
				    Net = netNoNetwork;
#ifdef DOS
				} else if (!strcmp(argv[1], "ftp")) {
				    Net = netFTPPacketDriver;
				} else if (!strcmp(argv[1], "ndis")) {
				    Net = netNDISDriver;
				} else if (!strcmp(argv[1], "odi")) {
				    Net = netODIDriver;
#endif
#ifdef WINDOWS
				} else if (!strcmp(argv[1], "winsock")) {
					Net = netWinsockDriver;
#endif
				} else {
				    printf("Bad network type: %s. Configuration failed.\n",
									argv[1]);
				    exit(1);
			   	}
				argv+=2; i+=2;
				if (i >=argc) {
					printf("No dummy IP number argument to -n option.  Config failed.\n");
					exit(1);
				}
				if (sscanf(*argv, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) != 4) {
					printf("Dummy IP number %s is invalid.  Config failed.\n",
														*argv);
					exit(1);
				}
/* The dummy IP number is stored in host byte order */
#ifdef BIG_ENDIAN
				dummyIP = (ip1 << 24 & 0xff000000) |
								(ip2 << 16 & 0xff0000) |
								(ip3 << 8 & 0xff00) |
								(ip4 & 0xff);
#else
				dummyIP = (ip4 << 24 & 0xff000000) |
								(ip3 << 16 & 0xff0000) |
								(ip2 << 8 & 0xff00) |
								(ip1 & 0xff);
#endif
		    } else {
				printf("Bad configuration option: %s. Configuration failed.\n",
									argv[1]);
				exit(1);
		    }
		} else {
			printf("Bad configuration option: %s. Configuration failed.\n",
									argv[1]);
			exit(1);
		}
    }
}

void
Usage()
{
#ifdef DOS
printf("-n { ftp|ndis|odi|none } dummy-ip#    Set default network type\n");
printf("-nomouse                              Default is no mouse support\n");
printf("-mouse                                Default is mouse supported\n");
#endif
#ifdef WINDOWS
printf("-n { winsock|none } dummy-ip#         Set default network type\n");
printf("-f filename                           Name of executable to configure\n");
#endif
}

void
main(argc, argv)
int argc;
char *argv[];
{
	printf("Meme configuration utility v 1.0\n");
	ProcessCmdLine(argc, argv);
	ModifyConfigBlock(FileName);
}
