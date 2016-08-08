/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* This file is copyright by Immersive Systems, Inc.  All rights
 * reserved.  You are permitted to use this file subject to
 * the provisions and restrictions in the license agreement, which
 * is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
 * set of files does not contain a file by one of the three
 * preceding names, please contact the Metatopia project at
 * sourceforge.net or at immersive.com.
 */

/* Debug logging routine.
 * Works with the #define in config.h.  Set the global DebugLog to 1 to enable.
 */

#include <sys/timeb.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define	DEBUGLOGFILE	"/memedbug.log"

void
DebugLog(char *str)
{
	char msg[132];

	struct _timeb theTime;
	struct tm *lt;
	int err;

	int file = _open(DEBUGLOGFILE,
		_O_APPEND | _O_CREAT | _O_SEQUENTIAL | _O_TEXT | _O_WRONLY,
		_S_IREAD | _S_IWRITE);
	if (file == -1) return;

	_ftime(&theTime);
	lt = localtime(&theTime.time);
	sprintf(msg, "%02d:%02d:%02d.%03d  %s\n", lt->tm_hour, lt->tm_min, lt->tm_sec, theTime.millitm, str);

	if (_write(file, msg, strlen(msg)) == -1) err = GetLastError();
	(void)_close(file);
}
