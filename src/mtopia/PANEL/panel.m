\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ The instrument panel for Metatopia
\ by Marc de Groot.
\ Copyright (c) 1995, 1996 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 28Aug96 MdG	2.69b	Reintegrated into Metatopia.
\ 30Jul96 MdG	2.69a	Integrated into Metatopia.
\ 16Sep95 MdG	2.6b	Panel code has no intelligence about SELECTED
\						state now; simply passes Select msg to
\						hilighted item at down-to-up edge of mouse
\						button.
\ 02Sep95 MdG	2.6		Modified for messaging rewrite.
\ 22Aug95 MdG	2.57c	File created.

6000 Module: foo

only
main also
std.mlContext! also
foo also
definitions

BeginModule

\ fload /meme/messages.mh
\ fload ../mtopia.mh
fload ../mtmsgs.mh

\ *************************************************************************
\ Variables
variable		rootTool	\ Compound object.

\ **************************************************************************
\ The receiver meme
: rcvr			\ msgaddr --
	mhMsgBody	\ -- msgbodyaddr
	dup @
	case
		dup				of				drop		endof
	endcase
; TheReceiverMemeIs rcvr

\ **************************************************************************
\ Go meme
: go
	if
		ModuleWorld lockedLink c" panel/roottool.mm" NewCompound rootTool !
	else
		rootTool @ ?dup if DestroyObject then
	then
; TheGoMemeIs go

\ **************************************************************************

ModuleUsed

EndModule

only main also definitions

' foo ModuleAddr p" panel.mm" UnloadModuleToFile

forget foo
