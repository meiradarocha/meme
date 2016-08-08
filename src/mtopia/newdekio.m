\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ *************************************************************************
\ New I/O event task

\ A message structure for the keyboards
create myKEYMsg SizeOf MsgHeaders SizeOf KEYMsg + dup allot
myKEYMsg mhMsgLen !


variable iotask

variable mouseOver		\ objaddr of the visible object that the mouse is over
variable oldMouseOver	\ previous value of mouseOver

variable thisMouseB		\ current mouse buttons
variable oldMouseB		\ previous mouse buttons

variable sendMouseTo  	\ Module that actually receives the events...
variable oldSendMouseTo	\ Previous value

create myPickRecord SizeOf PickRecords allot

\ Find the closest compound object ancestor in the tree to the given object
\ Return 0 if none found
: AncestorModule	\ objaddr -- moduleaddr
	begin
		dup
	while
		dup objecttype @ compoundObject = if
			ptr @ exit
		then
		parent @
	repeat
;

: i/o-loop	\ --
	mouseOver off
	thisMouseB off
	begin

		\ Store previous values of vars, and read mouse buttons
		mouseOver @ oldMouseOver !
		thisMouseB @ oldMouseB !
		sendMouseTo @ oldSendMouseTo !
		MouseB thisMouseB !

		\ Find out if the mouse is over an object
		MouseX MouseY myPickRecord PickObject 
		if      						\ If an object was picked
			
			\ Find the object containing the shape
			myPickRecord prShape @ WhichObject mouseOver !

			mouseOwner @ if				\ If a module owns the mouse
				mouseOwner @ sendMouseTo !
			else						\ If no module owns the mouse
				\ Find the closest ancestor in the tree that's a module
				\ If there is none, default to the avatar's module
				mouseOver @ AncestorModule ?dup 0= if
					localAvatar @ ptr @
				then
				sendMouseTo !
			then
		else							\ If no object was picked
			mouseOver off						\ mouse over no object
			localAvatar @ ptr @ sendMouseTo !	\ mouse data sent to avatar
		then

		\ If the mouse is over a different object, send a msgNotTouched
		\ to the old object, and a msgTouched to the new object.
		\ If the msg goes to the same module (because it is another
		\ child that is now touched) combine the two messages by sending
		\ msgNewTouched.
		\ If the mouse is over the same object and the state of the
		\ mouse buttons has changed, send a mouse button message.
		\ If the mouse is over the same object and the state of the
		\ mouse buttons hasn't changed, send no message.
		mouseOver @ oldMouseOver @ <> if
			sendMouseTo @ oldSendMouseTo @ <> if
				oldSendMouseTo @ msgNotTouched oldMouseOver @ SendTouchMsg
				sendMouseTo @ msgTouched mouseOver @ SendTouchMsg
			else
				sendMouseTo @ msgNewTouched mouseOver @ SendTouchMsg
			then
		then
		
	again
;
		



