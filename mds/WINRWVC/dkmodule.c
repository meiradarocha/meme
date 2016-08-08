/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Code for registering the deck module.
 * by Marc de Groot.
 * Copyright (c) Immersive Systems, Inc.  All rights reserved.
 */

#include "config.h"

#include <windows.h>
#include "forth.h"
#include "meme.h"
#include "externs.h"
#include "winextrn.h"
#include "errors.h"

CModule *WhichModule(module *mp);

cell
RegisterDeckModule(module *mp)
{
	if (DeckModule) {			// If there is already a registered deck
		XV_ERRNO = EDECKALREADYREGISTERED;
		return -1;
	}

	if (!WhichModule(mp)) {		// If the module is not a valid module
		XV_ERRNO = EDECKINVALIDMODULE;
		return -1;
	}

	DeckModule = (unsigned long)mp;
	return 0;
}






#ifdef REMOVE

cell
RegisterDeckModule(module *mp)
{
	cell sock;

	if (DeckModule) {
		XV_ERRNO = EDECKALREADYREGISTERED;
		return -1;
	}

	if (!WhichModule(mp)) {
		XV_ERRNO = EDECKINVALIDMODULE;
	}

	if (NetFail) {		/* If net uninitialized, each instance of Meme will run in its own world anyway */
		DeckModule = mp;
		return 0;
	}

/*
 * Find out if the port is being listened on.
 */
	rtn = NetConnect(LOCALHOST, MEMEPORT, 1);
	if (rtn == NETERROR) {						/* If net error, return failure. */
		XV_ERRNO = EDECKNETERROR;
		return -1;
	}

	sock = rtn;
	while ( (rtn = NetConnected(sock)) == NETPENDING ) ;
	if (rtn == NETESTABLISHED) {				/* If we got a connection, disconnect. */

		rtn = NetDisconnect(sock, 1);
		if (rtn == NETERROR) {					/* If net error, return failure */
			XV_ERRNO = EDECKNETERROR;
			return -1;
		}
		while (NetDisconnected(sock) == NETPENDING) ;	/* Wait for disconnect */
		XV_ERRNO = EDECKPORTOCCUPIED;				/* Return failure, someone else is listening */
		return -1;
	} else if (rtn == NETERROR && XV_ERRNO == NETCONNECTREFUSED) {	/* Nobody listening, return success */
		DeckModule = mp;
		return 0;
	} else if (rtn == NETERROR && XV_ERRNO == {
		XV_ERRNO = EDECKANOTHERMEMEREGISTERED;
		return -1;
	}
}	


	// Find out whether another copy of Meme is running a deck module.  If there is,
	// exit.
	if (!win32s) {
		memeOneInstanceSem = CreateSemaphore(NULL, 0, 1, "OnlyOneInstanceOfMeme");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			MessageBox(NULL,
				"There is already a deck module running on this computer.\nDeck registration failed."
				,"Meme", 0);
		}
	}

	/* If no previous deck registration has occurred, and the module pointer is valid,
	 * register the deck
	 */
	if (!DeckModule && WhichModule(mp)) {
		DeckModule = (unsigned long)mp;
	}
}
#endif
