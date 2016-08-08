/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Interface routines for the serial comm library.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

/* These routines are the "standard connector" routines that plug
 * the platform-specific serial library into Meme.
 */

#include "config.h"

#include <windows.h>
#include <stdio.h>

#include "forth.h"
#include "externs.h"
#include "errors.h"

#define	SERNOERROR		(0)
#define	SERERROR		(-1)

// The two states for a read or write piece of a comm port.
// IDLE means no action on the port.
// PENDING means an I/O operation has been initiated and we're waiting
// for the asynchronous part to finish.
#define	IDLE		(0)
#define	PENDING		(1)

// The port descriptor.  The cTaskActive field is first so the Meme
// task can write its task_active address in it without computing
// the offset into the struct.  This feature is not yet implemented.
typedef struct _portdesc {
	cell *			cTaskActive;	// Ptr to task_active for the task
	HANDLE			hCom;			// Handle to the serial port
	OVERLAPPED		rdOv;			// The OVERLAPPED struct for reading
	cell			rdState;		// Read side of port is IDLE/PENDING
	OVERLAPPED		wrOv;			// The OVERLAPPED struct for writing
	cell			wrState;		// Write side of port is IDLE/PENDING
	cell			wrActual;		// # bytes requested for writing
} PortDesc;


void
SetUpSerial(void)
{
}

void
CleanUpSerial(void)
{
}

/* Return a port descriptor if successful, which
 * is a unique 32-bit number that refers to this
 * comm connection.
 * On error, SERERROR is returned, and XV_ERRNO
 * is set to the error code.
 */
cell
SerialOpen(comport, baudrate, parity, word_length, stop_bits)
cell	comport;		/* 0 = COM1, 1 = COM2, etc.	*/
cell	baudrate;		/* 9600 = 9600 baud, etc.	*/
cell	parity;			/* 'N', 'E', or 'O'			*/
cell	word_length;	/* 7 or 8					*/
cell	stop_bits;		/* 1 or 2					*/
{
	HANDLE			hCom;
	char			szPortname[24];
	BOOL			fSuccess;
	DCB				dcb;
	DWORD			fdwEventMask;
	PortDesc *		lpPD;
	COMMTIMEOUTS	Timeouts;

	if (comport < 0 || comport > 7) {
		XV_ERRNO = SERBADPARAMS;
		return SERERROR;
	}
	sprintf( szPortname, "COM%d", comport+1 );
	hCom = CreateFile(szPortname,
		GENERIC_READ | GENERIC_WRITE,
		0,				/* comm devices must be opened w/exclusive-access	*/
		NULL,					/* no security attrs						*/
		OPEN_EXISTING,			/* comm devices must use OPEN_EXISTING		*/
		FILE_FLAG_OVERLAPPED,	/*  overlapped I/O							*/
		NULL					/* hTemplate must be NULL for comm devices	*/
	);
	if (hCom == INVALID_HANDLE_VALUE) { 
		XV_ERRNO = SERBADOPEN;
		return SERERROR;
	}

	/*
	 * Omit the call to SetupComm to use the default queue sizes.
	 * Get the current configuration.
	 */
	fSuccess = GetCommState(hCom, &dcb);
	if (!fSuccess) {
		CloseHandle(hCom);
		XV_ERRNO = SERBADOPEN;
		return SERERROR;
	}

	dcb.BaudRate = baudrate;

	switch (word_length) {
		case 7:
		case 8:
			dcb.ByteSize = (unsigned char)word_length;
			break;
		default:
			CloseHandle(hCom);
			XV_ERRNO = SERBADPARAMS;
			return SERERROR;
	}

	if (islower(parity)) {
		parity = toupper(parity);
	}
	switch (parity) {
		case 'N':
			dcb.Parity = NOPARITY;
			dcb.fParity = 0;
			break;
		case 'E':
			dcb.Parity = EVENPARITY;
			dcb.fParity = 1;
			break;
		case 'O':
			dcb.Parity = ODDPARITY;
			dcb.fParity = 1;
			break;
		default:
			CloseHandle(hCom);
			XV_ERRNO = SERBADPARAMS;
			return SERERROR;
	}

	switch (stop_bits) {
		case 1:
			dcb.StopBits = ONESTOPBIT;
			break;
		case 2:
			dcb.StopBits = TWOSTOPBITS;
			break;
		default:
			CloseHandle(hCom);
			XV_ERRNO = SERBADPARAMS;
			return SERERROR;
	}

	fSuccess = SetCommState(hCom, &dcb);
	if (!fSuccess) {
		CloseHandle(hCom);
		XV_ERRNO = SERBADPARAMS;
		return SERERROR;	
	}

	/* Set the event mask. */
	fdwEventMask = EV_RXCHAR;			// Character received
					// EV_BREAK |		// Break char detected
					// EV_CTS |			// CTS changed
					// EV_DSR |			// DSR changed
					// EV_ERR |			// Framing, overrun, or parity err
					// EV_RING |		// Ring indicator asserted
					// EV_RLSD |		// Received-line-signal-detect changed
					// EV_RXFLAG |		// Event character received
					// EV_TXEMPTY;		// Xmtr buffer has emptied

	fSuccess = SetCommMask(hCom, fdwEventMask);
	if (!fSuccess) {
		CloseHandle(hCom);
		XV_ERRNO = SERCANTSETEVENTS;
		return SERERROR;
	}

	/* Set the timeouts for the comm link */
	/* A value of MAXDWORD for ReadIntervalTimeout, combined with zero values for
	 * both the ReadTotalTimeoutConstant and ReadTotalTimeoutMultiplier, specifies
	 * that the read operation is to return immediately with the characters
	 * that have already been received, even if no characters have been
	 * received. 
	 */
	Timeouts.ReadIntervalTimeout = MAXDWORD;
	Timeouts.ReadTotalTimeoutMultiplier = 0;
	Timeouts.ReadTotalTimeoutConstant = 0;
	Timeouts.WriteTotalTimeoutMultiplier = 0;
	Timeouts.WriteTotalTimeoutConstant = 20;	/* Constant delay */
	fSuccess = SetCommTimeouts(hCom, &Timeouts);
	if (!fSuccess) {
		CloseHandle(hCom);
		XV_ERRNO = SERCANTSETEVENTS;
		return SERERROR;
	}

	/* Create a port descriptor */
	lpPD = (PortDesc *)malloc(sizeof(PortDesc));
	if (lpPD == NULL) {
		CloseHandle(hCom);
		XV_ERRNO = SERNOSPACE;
		return SERERROR;
	}
	lpPD->rdOv.hEvent = CreateEvent(
		NULL, 					// no security attributes
	    FALSE,					// auto-reset event
	    FALSE,					// initially not signaled
	    NULL 					// no name
	);

	/* If creating the read event object failed */
	if (lpPD->rdOv.hEvent == NULL || GetLastError() != 0) {
		CloseHandle(hCom);
		free(lpPD);
		XV_ERRNO = SERCANTSETEVENTS;
		return SERERROR;
	}
	lpPD->wrOv.hEvent = CreateEvent(
		NULL, 					// no security attributes
	    FALSE,					// auto-reset event
	    FALSE,					// initially not signaled
	    NULL 					// no name
	);

	/* If creating the write event object failed */
	if (lpPD->wrOv.hEvent == NULL || GetLastError() != 0) {
		CloseHandle(hCom);
		free(lpPD);
		XV_ERRNO = SERCANTSETEVENTS;
		return SERERROR;
	}

	/* We've set up the port; let's init the descriptor */
	lpPD->hCom = hCom;			// Set handle
	lpPD->cTaskActive = NULL;	// Clear the task_active ptr
	lpPD->rdState = IDLE;		// Initial state of the port
	lpPD->wrState = IDLE;		// Initial state of the port

	/* Set these two fields to zero or ReadFile will fail */
	lpPD->rdOv.Offset = lpPD->rdOv.OffsetHigh = 
	lpPD->wrOv.Offset = lpPD->wrOv.OffsetHigh = 0;

	return (cell)lpPD;
}

/* Close the serial port.  SerialClose returns
 * SERNOERROR if successful, and SERERROR if
 * the call failed.
 */
cell
SerialClose(portdesc)
cell portdesc;
{
	CloseHandle( ((PortDesc *)portdesc)->rdOv.hEvent );
	CloseHandle( ((PortDesc *)portdesc)->wrOv.hEvent );
	if ( !CloseHandle( ((PortDesc *)portdesc)->hCom) ) {
		free((void *)portdesc);
		XV_ERRNO = SERBADCLOSE;
		return SERERROR;
	}
	free((void *)portdesc);
	return SERNOERROR;
}

/* Request a read operation on the serial port.
 * Pass a port descriptor, a character buffer address,
 * and a maximum number of characters to read.
 * SerialRead returns the actual number of characters
 * read, or on error, SERERROR is returned.
 */
cell
SerialRead(portdesc, buffer, len)
cell portdesc;
cell buffer;
cell len;
{
	PortDesc *lpPD = (PortDesc *)portdesc;	// Convert to a real ptr to PortDesc
	BOOL fSuccess;
	cell IOActual;

	fSuccess = ReadFile(
		lpPD->hCom,					// Serial port handle
		(LPVOID)buffer,				// Character buffer
		(DWORD)len,					// Max # of bytes to read
		(LPDWORD)&IOActual,			// On rtn, the actual # bytes read
		&lpPD->rdOv );				// Ptr to OVERLAPPED struct
	if (!fSuccess) {
		XV_ERRNO = SERREADFAILED;
		return SERERROR;
	}
	return IOActual;
}

cell
SerialWrite(portdesc, buff, len)
cell portdesc;
cell buff;
cell len;
{
	PortDesc *lpPD = (PortDesc *)portdesc;	// Convert to a real ptr to PortDesc
	BOOL fSuccess;
	cell IOActual;

	switch (lpPD->wrState) {
		case IDLE:
			fSuccess = WriteFile(
				lpPD->hCom,				// Serial port handle
				(LPCVOID)buff,			// Char buff
				(DWORD)len,				// # characters to write
				(LPDWORD)&IOActual,		// Actual # of chars written
				&lpPD->wrOv );			// Ptr to OVERLAPPED struct

			if (!fSuccess) {
				if (GetLastError() == ERROR_IO_PENDING) {
					lpPD->wrState = PENDING;
					lpPD->wrActual = len;
					return 0;
				} else {
					XV_ERRNO = SERWRITEFAILED;
					return SERERROR;
				}
			}
			return IOActual;
			break;
		case PENDING:
			fSuccess = WaitForSingleObject(lpPD->wrOv.hEvent, 0);
			if (fSuccess == WAIT_TIMEOUT) {			// Write not completed yet
				return 0;
			} else if (fSuccess == WAIT_OBJECT_0) {	// Write completed
				lpPD->wrState = IDLE;
				return lpPD->wrActual;
			} else if (fSuccess == WAIT_FAILED) {	// Failure!
				XV_ERRNO = SERWRITEFAILED;
				return SERERROR;
			}
			break;
		default:
			XV_ERRNO = SERWRITEFAILED;
			return SERERROR;
	}
}
