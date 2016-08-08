\ High-level routines for the network.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

: NetPrintError	( err -- )
	case
		netCONNECTREFUSED	of ." Connection refused"				endof
		netCLOSEDEARLY		of ." Connection closed"				endof
		netTIMEDOUT			of ." Timed out"						endof
		netUNRESOLVEDHOST	of ." Unknown host"						endof
		netUNRESOLVEDARP	of ." Host isn't answering"				endof
		netUNINITIALIZED	of ." Network software isn't running"	endof
		netNOSOCKSPACE		of ." Can't allocate socket"			endof
		netINVALIDSOCK		of ." Invalid socket"					endof
		dup dup ( default )	of ." Unknown error: " .				endof
	endcase
;

: NetGetS	{ socket buffer maxlen -- }	\ socket buffer maxlen -- err
	begin
		socket buffer maxlen 1- NetRead dup netERROR =
		if
			exit
		then
		pause
		dup buffer + to buffer
		maxlen swap - to maxlen
		buffer 1- c@ newline =
		maxlen 0<= or
		if
			0 buffer c! 0 exit
		then
	again
;

\ Write a null-terminated cstring on the socket. Doesn't write the null.
: NetPutS	{ socket buffer ; remaining -- }	\ socket buffer -- err
	buffer strlen to remaining
	begin
		remaining
	while
		socket buffer remaining NetWrite dup netERROR =
		if
			exit
		then
		remaining over - to remaining
		buffer + to buffer
		pause
	repeat
	0
;

\ Write a network-standard newline on the socket.
create netCR 13 c, 10 c, 0 c, align
: NetPutCR 	\ socket -- err
	netCR NetPutS
;

