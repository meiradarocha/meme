\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Cyberspace deck.
\ by Zap and MdG
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 11Apr99 MdG	2.78a	More d\ added to get rid of run-time messages.
\ 06Sep96 MdG	2.69b	Changed the timeout on connecting to the space server
\						from 10 to 5 seconds.  Reformatted the receiver meme.
\ 17Feb96 MdG	2.66b	Taken over by MdG.
\ 15Dec95 zap			File created.

.( Deck module: mtdeck.m )

17000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

fload mtmsgs.mh

\ ----------- The SERVER stuff begins here ---------------
\ Finger client.
\ by Marc de Groot.
\ Copyright (c) 1995 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 29Nov95 MdG  2.62a Changed GetHostByName to NetGetHostByName.
\ 21Jul95 MdG  2.56a File created.

variable herIP \ Place to put the IP to connect to

variable sock

0 constant noisy

\ Replace the first newline found with a null.  
: RemoveLF     \ addr --
	begin				\ For each character
		dup c@			\ -- addr char
		dup 0= not		\ -- addr char b1
		swap 10 = not	\ -- addr b1 b2
		and				\ If neither null nor newline
	while
		1+				\ -- addr+1		Bump pointer
	repeat
	0 swap c!			\ --			Store null in last char
;

\ Network-standard newline--always transmitted over the net, regardless
\ of newline convention on the local host.
create netNewline 13 c, 10 c, 0 c, 0 c,

\
: netType ( cstr count -- )
   ?dup 0<>					\ -- cstr n bool
   if
      0 do					\ -- cstr
         dup c@				\ -- cstr char
         dup 13 <>			\ -- cstr char bool
         if					\ -- cstr char
            emit			\ -- cstr
         else				\ -- cstr char
            drop pause		\ -- cstr
         then
         1+					\ -- cstr+1
      loop
   then
   drop
;

: Connect   ( hostname -- )
	NetGetHostByName ?dup 0= if
d\		." Hostname resolution failed." cr 
		0 exit
	then
	8787 5 NetConnect dup netERROR = if 
d\		." Connect failed" cr 
		0 exit 
	then
	sock !
	begin
		sock @ NetConnected? dup netPENDING =
	while
		drop pause
	repeat
	netERROR = if
d\		." Connect: error " NetError NetPrintError cr
		0 exit
	then
	1
;

: Write  { username -- }
   username strlen 
   if
      sock @ username dup strlen NetWrite
      noisy if dup ascii [ emit 0 .r ascii ] emit then
      drop
   then
   sock @ netNewline 2 NetWrite  
	noisy if dup ascii [ emit 0 .r ascii ] emit then
   netERROR = if
d\		." Write: error " NetError NetPrintError 
		abort
   then
;

: Read { ; NetBuff counter -- }
\  sock @ 32 20000 NetDispatch drop
   1000 alloc-mem to NetBuff
   begin
      pause
      sock @ NetBuff 1000 NetRead      
		noisy if dup ascii ( emit 0 .r ascii ) emit then
      dup netERROR <> 
   while
      drop \ NetBuff swap netType
   repeat

   drop
d\   cr ." Server ended connection " cr
   NetBuff \ Return the buffer
;

\ Copy until first '@' or null and null-terminate.  
: copytill@ \ src dest -- src
   begin
      over c@
      dup ascii @ <> swap 0<> and
   while
      over c@ over c! 1+ swap 1+ swap
   repeat
   0 swap c!
;

variable serverName

\ Send a command to the server
: TalkToServer { command spaceID ; fullCommand -- }
   
   \ Allocate memory for the full command
   command strlen spaceID strlen 2 + + alloc-mem to fullCommand
	fullCommand 0= if 
d\		." Out of memory connecting to server!" cr 
		0 exit 
	then

   command fullCommand strcpy
   spaceID fullCommand command strlen + strcpy
   
d\   cr ." Connecting to multiuser server: " serverName @ ctype cr
   serverName @ Connect if
d\      cr ." Sending request:" 
d\      cr ."  Command: " command ctype 
d\      cr ."  Space:   " spaceID ctype cr
      fullCommand Write
      Read  \ Read leaves netbuffer pointer on the stack
   else
      0
   then
   
   fullCommand free-mem
;

\ Log in on a server
: serverLogin { spaceID ; buf -- }
d\   ." Logging in to space: " spaceID ctype cr
   c" A" spaceID TalkToServer to buf
   buf if
      buf RemoveLF \ Slice of at the LF      
      
      buf c@ ascii - <> if
         buf NetGetHostByName herIP !
      then
      
      buf free-mem

	\ Comment out the following two lines to let the code run
	\ when there is no space server
\	else
\		cr ." Cannot log into space!  Error...exiting." cr cold

   then
;

: serverLogout { spaceID -- }
d\   ." Logging out of space: " spaceID ctype cr
   c" D" spaceID TalkToServer ?dup if
      free-mem
   then
;


\ ----------- The SERVER stuff ends here -----------------


variable eye
variable rtask

\ The local Avatar object
variable localAvatar


\ : SendMessage 
\   ." Deck Sending message " dup mhMsgBody @ . cr
\   SendMessage
\ ;

\ A general message structure for messages with only an integer body
create myGENMsg SizeOf MsgHeaders 4 + dup allot
myGENMsg mhMsgLen !

\ Another general message structure for messages with only an integer body
create myGEN2Msg SizeOf MsgHeaders 4 + dup allot
myGEN2Msg mhMsgLen !


\ A message for the replicaton request
create myRPTMsg SizeOf MsgHeaders SizeOf RPTMsg + dup allot
myRPTMsg mhMsgLen !

\ A message for the IP list items
create myIPLMsg SizeOf MsgHeaders SizeOf IPLMsg + dup allot
myIPLMsg mhMsgLen !



\ -------------- Begin space code ------------------

\ Exit a space

create current_space 100 allot

: ExitSpace

d\	." Log out of the space" cr
        current_space serverLogout

d\	." Get off the IP list" cr
         localHost 0  DeleteIPListItem

d\	." Send msgLogOut to everybody!" cr
         msgLogOut     myGENMsg mhMsgBody !
         0             myGENMsg mhSrcModuleID !
         localHost     myGENMsg mhSrcHostID !

         myGENMsg      SynMessageToAll

d\	." Send it to the avatar (which deletes all slaves!)" cr
         localHost     myGENMsg mhDestHostID !
         localAvatar @ ptr @ myGENMsg mhDestModuleID !
         myGENMsg      SendMessage drop

d\	." Orphan the avatar" cr
         localAvatar @ Orphan drop

d\	." Kill everything else" cr
         world DestroyChildren

d\	." Reattach the avatar...." cr
         world localAvatar @ Adopts drop
;

\ Enter a space at a given location....

: EnterSpace { spaceName ; temp | inX inY inZ inYaw -- }   
   \ Add myself to the new space's IP list
   localHost 0 AddIPList

   \ Load the space object

d\	." Entering space: " spaceName ctype ."  at " inX f. inY f. inZ f. cr

   inX   localAvatar @ x!
   inY   localAvatar @ y!
   inZ   localAvatar @ z!

   inYaw localAvatar @ yaw!

   0 0 localAvatar @ world lockedLink spaceName NewCompound to temp

   temp if
      spaceName current_space strcpy

	  herIP off

      spaceName serverLogin

      \ If we're gonna visit, do it now.

      herIP @ if
         localHost    myGENMsg mhSrcHostID !
         moduleOrigin myGENMsg mhSrcModuleID !
         herIP @      myGENMsg mhDestHostID !
         0            myGENMsg mhDestModuleID ! \ The deck
         msgLogIn     myGENMsg mhMsgBody !

d\		." Sending msgLogin to " hex herIP @ u. decimal cr

         myGENMsg     SendMessage drop
      else
         localHost    myGENMsg mhSrcHostID !
         moduleOrigin myGENMsg mhSrcModuleID !
         localHost    myGENMsg mhDestHostID !
         temp ptr @   myGENMsg mhDestModuleID ! \ The space!
         msgSpaceInit myGENMsg mhMsgBody !

d\		." Nobody home - Telling space to go initialize itself!" cr

         myGENMsg     SendMessage drop
      then   
   then
;


\ ---------- End space code ---------------


\ ReplicateObject ( hostID object -- )
\ Sends a "replicate yourself to this host" message to the object.
: ReplicateObject
   ptr @     myRPTMsg mhDestModuleID !
             myRPTMsg mhMsgBody RPTHostID !
   localHost myRPTMsg mhDestHostID !
   0         myRPTMsg mhSrcModuleID !
   localHost myRPTMsg mhSrcHostID !
   msgReplicateTo myRPTMsg mhMsgBody !
   myRPTMsg SendMessage drop
;

: ReplicateSpace { host ; temp -- }
   world child @ to temp
   begin
      \ Send the message to each compound object
      temp objecttype @ compoundObject = if
         host temp ReplicateObject
      then
      temp sibling @ to temp
      temp 0= if exit then    
   again  
;

: SendIPList { host ; temp -- }
   PrintIPList
   IPListRoot @ to temp
   begin
      temp 0= if exit then
      temp .

      temp IPListHostID @ myIPLMsg mhMsgBody IPLHostID !
      msgIPList myIPLMsg mhMsgBody !
      host      myIPLMsg mhDestHostID !
      0         myIPLMsg mhDestModuleID !
      localHost myIPLMsg mhSrcHostID !
      0         myIPLMsg mhSrcModuleID !
      myIPLMsg SendMessage drop
      temp IPListNext @ to temp
   again
;

\ ----------- WORKAROUND ------------
variable newspacetask

create newsBuf 100 allot
fvariable newsX
fvariable newsY
fvariable newsZ
fvariable newsYaw

: newspacefunc begin   
   ExitSpace

   newsBuf
   newsX   f@
   newsY   f@
   newsZ   f@
   newsYaw f@

   EnterSpace
   
   newspacetask @ sleep

   pause
again ;
\ ----------- END WORKAROUND ------------

variable msg

variable repTo


variable mouseOwner

variable mouseOverObject

: recv
	msg !

d\	." Deck: " moduleOrigin u. msg @ mhMsgBody ? cr

	msg @ mhMsgBody @
	case 

		\ ********
		\ msgLogIn - Someone is attempting to visit the local space.
		\ ********
		msgLogIn of
			\ Approve the login
			msg @ mhSrcHostID @   repTo !
			msgLogInACK  myGENMsg mhMsgBody !
			localHost    myGENMsg mhSrcHostID !
			moduleOrigin myGENMsg mhSrcModuleID !
			repTo @      myGENMsg mhDestHostID !
			0            myGENMsg mhDestModuleID !
			myGENMsg SendMessage 0= if
d\				." Replicating space to visitor..." cr
				repTo @ ReplicateSpace
d\				." Transferring IP list to visitor..." cr
				repTo @ SendIPList   
				repTo @ 0 AddIPList
			then
		endof

		\ *********
		\ msgLogOut - A visitor leaves the local space
		\ *********
		msgLogOut of
			msg @ mhSrcHostID @ 0 DeleteIPListItem
		endof

		\ *********
		\ msgLogInACK - We have succeeded in logging into a space
		\ *********
		msgLogInACK of
d\			." We're in!" cr
		endof

		\ *********
		\ msgLogInNAK - We have failed in our attempt to log into a space
		\ *********
		msgLogInNAK of
			." Your login was disapproved - bye bye" key
			bye
		endof

		\ ********
		\ msgExitSpace - Owner of the space is throwing us out (flatlined!)
		\ ********
		msgExitSpace of
			ExitSpace
			localAvatar @ DestroyObject
			\ bye
		endof

		\ ********
		\ msgNewSpace - I enter a new space
		\ ********
		msgNewSpace of
			\ As usual, a tasking workaround is needed. BIG SIGH...!
			msg @ mhMsgBody ENTspaceID    newsBuf strcpy
			msg @ mhMsgBody ENTnewX   f@  newsX f!
			msg @ mhMsgBody ENTnewY   f@  newsY f!
			msg @ mhMsgBody ENTnewZ   f@  newsZ f!
			msg @ mhMsgBody ENTnewYaw f@  newsYaw f!
			newspacetask @ wake
		endof

		\ ********
		\ msgIPList - Another IP has come online.
		\ ********
		msgIPList of

			\ When we get the IP 'localHost' that 
			\ really means the sender's IP...
			msg @ mhMsgBody IPLHostID @ 		\ Get the new host's IP
			localHost = if						\ If it's 127.0.0.1
				msg @ mhSrcHostID @				\ Get the sender's IP
			else								\ Otherwise,
				msg @ mhMsgBody IPLHostID @		\ Use the new host's IP
			then

			\ Don't replicate my own avatar
			dup IsMyHostID 0= if				\ If it's not my own hostID
                dup 0 AddIPList					\ Add it to the list
                localAvatar @ ReplicateObject	\ Rep my avatar to that host
			else								\ If it's my own hostID
				drop							\ Just ignore it
			then
		endof

		\ ********
		\ msgCreateOBJ - Create an object
		\ ********
		msgCreateOBJ of

			\ Put masterIP on the stack.

			\ (Fix: If source hostID=0, then it means the originating host)
			msg @ mhMsgBody COBmasterHostID @ 	\ -- masterIP
			?dup 0= if 							\ If masterIP = 0
                msg @ mhSrcHostID @ 			\ use the sender's hostID
             then 

			\ Put the mastermoduleID on the stack

			msg @ mhMsgBody COBmasterModuleID @	\ -- mstrhostID mstrmoduleID

			\ Put the localAvatar objaddr on the stack
			localAvatar @ 	\ -- mstrhostID mstrmoduleID localavatar

			\ Create the new object, passing masterhostID, mastermoduleID,
			\ and localAvatar as arguments to the go meme
			world lockedLink msg @ mhMsgBody COBslaveURL NewCompound drop

		endof

		\ ********
		\ msgDeleteObject - Delete an object
		\ ********
		msgDeleteObject of
d\			." Got msgDeleteObject for " msg @ mhMsgBody DELmoduleID @ . cr
			msg @ mhMsgBody DELmoduleID @ WhichObject ?dup if 
				DestroyObject
			then
		endof

		\ ********
		\ msgDeleteSlave - Delete slave object
		\ ********
		msgDeleteSlave of
d\			." Got msgDeleteSlave for " msg @ mhMsgBody DELmoduleID @ . cr
			localHost						myGEN2Msg mhSrcHostID !
			moduleOrigin					myGEN2Msg mhSrcModuleID !
			localHost						myGEN2Msg mhDestHostID !
			msg @ mhMsgBody DELmoduleID @	myGEN2Msg mhDestModuleID !
			msgMasterDied					myGEN2Msg mhMsgBody !
			myGEN2Msg SendMessage drop

			msg @ mhMsgBody DELmoduleID @ WhichObject ?dup if 
				DestroyObject
			then
		endof

		\ ************
		\ msgGrabMouse - Someone requested permission to own the mouse
		\ ************
		msgGrabMouse of
         msg @ mhSrcModuleID @ mouseOwner !
      endof
	\ ************
	\ msgReleaseMouse - Someone released ownership of the mouse
	\ ************
      msgReleaseMouse of
         0 mouseOwner !
      endof

      \ An object that cares about the "touched" flag tells the deck this
      \ by returning this message.
	\ ************
	\ msgHandleTouched - 
	\ ************
      msgHandleTouched of
         msg @ mhSrcModuleID @ WhichObject mouseOverObject !
      endof

   endcase
;

TheReceiverMemeIs recv 


: r begin render pause again ;

\ A message structure for the "do you need the mouse" handler
create myRATMsg SizeOf MsgHeaders 4 + dup allot
myRATMsg mhMsgLen !

: DoesNeedMouse? { objToAsk -- }
   begin
      objToAsk 0= if
         exit \ return
      else
         objToAsk objecttype @ compoundObject = if
            localHost      myRATMsg mhSrcHostID !
            0              myRATMsg mhSrcModuleID !
            localHost      myRATMsg mhDestHostID !
            objToAsk ptr @ myRATMsg mhDestModuleID !
            msgWantMouse?  myRATMsg mhMsgBody !

            myRATMsg SendMessage drop               
         then

         \ If we got someone in need of mouse, exit!
         mouseOwner @ if exit then

         \ ...otherwise, check parent recursively
         objToAsk parent @ to objToAsk
      then
   again
;

\ This function is used to signal to objects that the mouse is over them or not.
\ It is called with an object pointer.

: MouseTouchHandler { overNow ; overBefore temp -- }
   \ If there is an object that is handling this already, then first
   \ of all check if the mouse pointer is still above it.

   \ If a mousebutton is down, then consider overNow to always be nothing!

   mouseOverObject @ if
      overNow to temp

      begin
         temp if
            mouseOverObject @ temp = 
            temp parent @ to temp
         else \ End the loop
            1
         then
      until

      \ We get here for one of two reasons:
      \ Either we found the object <temp is nonzero>
      \ or we didn't <temp is zero>
      
      temp 0= if
         \ If we didnt find it, we need to inform that object that the mouse
         \ is no longer above it!

         localHost               myRATMsg mhSrcHostID !
         0                       myRATMsg mhSrcModuleID !
         localHost               myRATMsg mhDestHostID !
         mouseOverObject @ ptr @ myRATMsg mhDestModuleID !
         msgNotTouched           myRATMsg mhMsgBody !

         myRATMsg SendMessage drop               

         0 mouseOverObject !
      else
         \ If we *did* find it, then inform it that we are now over another sub-entity
         \ (As if he cares :-)

         localHost               myRATMsg mhSrcHostID !
         0                       myRATMsg mhSrcModuleID !
         localHost               myRATMsg mhDestHostID !
         mouseOverObject @ ptr @ myRATMsg mhDestModuleID !
         msgNewTouched           myRATMsg mhMsgBody !

         myRATMsg SendMessage drop               
      then            
   then   

   \ Note: This must NOT be in an else-clause of the above if statment
   \       it must be it's own statement

   \ If the mouse is not currently over any object:
   mouseOverObject @ 0= if
      overNow to temp

      \ Send "touched" message to the objects
      \ an object that CARES, replies with "msgHandleTouched"
      \ wich sets mouseOverObject in the receiver meme above!

      begin
         temp if
            temp objecttype @ compoundObject = if
               localHost      myRATMsg mhSrcHostID !
               0              myRATMsg mhSrcModuleID !
               localHost      myRATMsg mhDestHostID !
               temp ptr @     myRATMsg mhDestModuleID !
               msgTouched     myRATMsg mhMsgBody !

               myRATMsg SendMessage drop

            then

            temp parent @ to temp

            \ If one replied, exit the loop
            mouseOverObject @ 
         else \ End the loop
            1
         then
      until
   then
;

\ A message structure for the keyboards
create myKEYMsg SizeOf MsgHeaders SizeOf KEYMsg + dup allot
myKEYMsg mhMsgLen !


variable mtask

variable mouseOver
variable oldmouseOver

variable oldMouseB
variable thisMouseB

variable sendMouseTo  \ Module that actually receives the events...

create myPickRecord SizeOf PickRecords allot

: m 
	begin   
	   \ It is important to sample the mousebuttons at only ONE time!!
	   \ This is the ONLY place in time where MouseB is actually read!
	   MouseB thisMouseB !

	   \ Find the object the pointer is over...

	   MouseX MouseY myPickRecord PickObject if      
		  myPickRecord prShape @ WhichObject mouseOver !

		  \ If it's too far away, we can't select it!

		  myPickRecord prWX f@ localAvatar @ x@ f- fdup f*
		  myPickRecord prWY f@ localAvatar @ y@ f- fdup f*
		  myPickRecord prWZ f@ localAvatar @ z@ f- fdup f*
		  f+ f+ 75e f> if
			 0 mouseOver !
		  then

	   else
		  0 mouseOver !
	   then            

	   \ If the mouse coverage has changed...and no button is down...
	   mouseOver @ oldmouseOver @ <> thisMouseB @ 0= and if      

		  \ Inform the object the mouse is over it...      
		  mouseOver @ MouseTouchHandler
		  mouseOver @ oldmouseOver !
	   then

	   \ If we have an owner
	   mouseOwner @ if
		  mouseOwner @ sendMouseTo ! \ Use that!
	   else
		  localAvatar @ ptr @ sendMouseTo ! \ Default to local Avatar
	   then

		\ If no button has changed, nothing needs to be done!
		oldMouseB @ thisMouseB @ <> if	\ If mouse buttons have changed
			mouseOwner @ 0= if			\ If nobody owns the mouse

				\ Check if it was a downward transition of 
				\ the left OR right button
				oldMouseB @ leftButton and 0= 
				thisMouseB @ leftButton and 0<> and  \ -- leftDownTransition?
				oldMouseB @ rightButton and 0= 
				thisMouseB @ rightButton and 0<> and \ -- rightDownTransition?
				or if            
					mouseOver @ DoesNeedMouse?

					\ If one was found....
					mouseOwner @ if
						mouseOwner @ sendMouseTo !		\ ...use that!
					then
				then
			then

			\ Calculate which event(s) did happen...

			\ Left mouse went DOWN
			oldMouseB @ leftButton and 0= 
			thisMouseB @ leftButton and 0<> and if
				localHost      myRATMsg mhSrcHostID !
				0              myRATMsg mhSrcModuleID !
				localHost      myRATMsg mhDestHostID !
				sendMouseTo @  myRATMsg mhDestModuleID !
				msgLMouseDown  myRATMsg mhMsgBody !
				myRATMsg SendMessage drop               
			then

			\ Left mouse went UP
			oldMouseB @ leftButton and 0<> 
			thisMouseB @ leftButton and 0= and if
				localHost      myRATMsg mhSrcHostID !
				0              myRATMsg mhSrcModuleID !
				localHost      myRATMsg mhDestHostID !
				sendMouseTo @  myRATMsg mhDestModuleID !
				msgLMouseUp    myRATMsg mhMsgBody !
				myRATMsg SendMessage drop               
			then

			\ Right mouse went DOWN
			oldMouseB @ rightButton and 0= 
			thisMouseB @ rightButton and 0<> and if
				localHost      myRATMsg mhSrcHostID !
				0              myRATMsg mhSrcModuleID !
				localHost      myRATMsg mhDestHostID !
				sendMouseTo @  myRATMsg mhDestModuleID !
				msgRMouseDown  myRATMsg mhMsgBody !
				myRATMsg SendMessage drop               
			then

			\ Right mouse went DOWN
			oldMouseB @ rightButton and 0<> 
			thisMouseB @ rightButton and 0= and if
				localHost      myRATMsg mhSrcHostID !
				0              myRATMsg mhSrcModuleID !
				localHost      myRATMsg mhDestHostID !
				sendMouseTo @  myRATMsg mhDestModuleID !
				msgRMouseUp    myRATMsg mhMsgBody !
				myRATMsg SendMessage drop               
			then
		then

		\ Keyboard messages...
   
		begin
p\			key?
d\			0
		while
			key dup myKEYMsg mhMsgBody KEYascii !

			27 = if
				ExitSpace
				localAvatar @ DestroyObject
				bye
			else
				localHost      myKEYMsg mhSrcHostID !
				0              myKEYMsg mhSrcModuleID !
				localHost      myKEYMsg mhDestHostID !
				sendMouseTo @  myKEYMsg mhDestModuleID !
				msgKeyPress    myKEYMsg mhMsgBody !

				myKEYMsg SendMessage drop                  
			then

		repeat
   
		\ Remember the values for next loop...

		thisMouseB @ oldMouseB !
		pause
	again
;

\ Login to the server. Put in a separate task to run asynchronously....
: loginfunc
\   c" spaces\mtstreet.mm" 0e 0e 30e 0e EnterSpace
\   c" spaces\mtisihq.mm" 2e 0e -2e 0e EnterSpace
   c" spaces\mtstreet.mm" -50e 0e 0e -90e EnterSpace
;

: go
	if
d\		." Deck loaded, moduleID " moduleOrigin . cr

		herIP off

\		c" 192.168.172.193" serverName !
		c" 192.168.172.197" serverName !
\		c" 204.156.156.143" serverName !

		moduleOrigin RegisterDeckModule if
			abort" There is already a deck registered - aborting "
		then

		localHost 0 AddIPList \ Add a marker for youself in the IP list

		world lockedLink monoCamera
		0 0 WindowWidth WindowHeight NewCamera eye !
      
		eye @ 0e 0.0e 0.8e SetCameraBackColor

		\ The Eye will become child of the localAvatar's head object.

		0 0 eye @		\ We pass the eye as the "localAvatar" parameter 
						\ to the localAvatar, since it wants to know about 
						\ that, instead of itself...
		world lockedLink c" mtavatar.mm" NewCompound localAvatar !            


		\ If we are a visitor, then we start at another location.
		herIP @ if
			localAvatar @ -2e z!
			localAvatar @ 180e yaw!
		then                

p\		maintask sleep

		['] r NewTask ?dup if dup rtask ! wake then
		['] m NewTask ?dup if dup mtask ! wake then
		['] loginfunc NewTask ?dup if wake then
		['] newspacefunc NewTask ?dup if newspacetask ! then

	else
		rtask     @ ?dup if DestroyTask then
		mtask     @ ?dup if DestroyTask then
		newspacetask @ ?dup if DestroyTask then
		eye       @ DestroyObject
	then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr ScribbleModule
' foo ModuleAddr p" mtdeck.mm" UnloadModuleToFile
forget foo

