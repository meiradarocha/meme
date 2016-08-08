\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Instrument panel root-level tool
\ by Marc de Groot
\ Copyright (c) 1995, 1996 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 28Aug96 MdG	2.69b	Code for Metatopia ball object converted for root tool.
\ 25Aug96 MdG	2.69b	Some optimization of the code.
\ 17Feb96 MdG	2.66b	Taken over by MdG.


20000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

variable masterIP		\ The IP of the master
variable masterMH		\ The Module Handle of the master
variable masterAlive	\ Is the master alive?
variable localAvatar	\ The Local Avatar object (for collision detection etc)

\ fload /meme/messages.mh
\ fload ../mtopia.mh
fload ../mtmsgs.mh

\ *************************************************************************
\ Variables holding pointers to objects
variable        rootSelected    \ TRUE if root tool selected.
variable        toolIcon        \ Visible object.
variable        iconMat         \ The icon's material.
variable        consTool        \ The construction tool.

\ *************************************************************************
\ If the window has been resized, reposition the icon.
variable resizeTask
variable oldHeight
variable oldWidth
: Resize    \ --
    begin
        WindowHeight oldHeight !
        WindowWidth oldWidth !
        WindowWidth float 3800e f/ fnegate  toolIcon @ x!
        WindowHeight float 3800e f/         toolIcon @ y!
        begin
            pause pause pause pause pause pause pause
            WindowHeight oldHeight @ <>
            WindowWidth oldWidth @ <> or
        until
    again
;
\ *************************************************************************
\ When the tool is selected, it spins.
variable vibrateTask
: vibrate
    begin
        GetTime nip 360 * 1000 /
        float toolIcon @ yaw!
        pause
    again
;
\ **************************************************************************
\ Routines that implement the behaviors of an instrument panel tool

\ Do this when the pointer is over the tool icon
: Highlight         \ --
    0.85e 0.35e 0e 0e   iconMat @   MaterialSurface
    iconMat @           toolIcon @  ShapeMaterial
;

\ Do this when the pointer is not over the tool icon
: Unhighlight       \ --
    0.4e 0.2e 0e 0e iconMat @       MaterialSurface
    iconMat @       toolIcon @      ShapeMaterial
;

\ Do this when the mouse is clicked on the selected tool
: Unselect          \ --
    vibrateTask @ ?dup
    if
        sleep
    then
    consTool @ ?dup
    if
        DestroyObject consTool off
    then
    rootSelected off
;

\ Do this when the mouse is clicked on the unselected tool
: Select            \ --

    \ If I'm already selected, toggle my state to unselected.
    rootSelected @
    if
        0 ( dummy for drop in Unselect ) Unselect
    else
        vibrateTask @ ?dup
        if
            wake
        then
        consTool @ 0=
        if
\            ModuleWorld lockedLink c" panel/constool.mm"
\            NewCompound consTool !
        then
        rootSelected on
    then
;
\ **************************************************************************
\ A general message structure for messages with only an integer body
create myGENMsg SizeOf MsgHeaders 4 + dup allot
myGENMsg mhMsgLen !

\ A message for the create-object call
create myCOBMsg SizeOf MsgHeaders SizeOf COBMsg + 20 + dup allot
myCOBMsg mhMsgLen !

-1 constant msgKickRequest
-2 constant msgKickTo
	BeginStruct
		Int   KICKmsgCode
		Float KICKStartX
		Float KICKStartZ
		Float KICKEndX
		Float KICKEndZ
	Struct KICKMsg

\ A message for the kick call
create myKICKMsg SizeOf MsgHeaders SizeOf KICKMsg + dup allot
myKICKMsg mhMsgLen !

variable rolltask

\ A kick takes 5 seconds

variable initRoll

fvariable kickEndTime

fvariable kickStartX
fvariable kickStartZ
fvariable kickEndX
fvariable kickEndZ

fvariable tNow \ For accuracy

fvariable lastAvatarX
fvariable lastAvatarZ

: rollball 
	begin
		pause
   
		initRoll @ 0= if
			initRoll on
			ModuleWorld x@ kickEndX f!
			ModuleWorld z@ kickEndZ f!
			fGetTime  kickEndTime f!
		then

		fGetTime tNow f!

		kickEndTime f@ tNow f@ f< if   
			kickEndX f@ ModuleWorld x!
			kickEndZ f@ ModuleWorld z!
		else   
			kickEndTime f@ tNow f@ f- 5e f/ fdup f* fdup \ ( -- deltaT )
			kickStartX  f@ kickEndX f@ f- f* kickEndX f@ f+ ModuleWorld x!
			kickStartZ  f@ kickEndZ f@ f- f* kickEndZ f@ f+ ModuleWorld z!
		then

		localAvatar @ x@ ModuleWorld x@ f- fabs 0.3e f<
		localAvatar @ z@ ModuleWorld z@ f- fabs 0.3e f< and if
			lastAvatarX f@ localAvatar @ x!
			lastAvatarZ f@ localAvatar @ z!

			\ Tell the avatar he has moved!
			localHost           myGENMsg mhSrcHostID !
			moduleOrigin        myGENMsg mhSrcModuleID !
			localHost           myGENMsg mhDestHostID !
			localAvatar @ ptr @ myGENMsg mhDestModuleID !
			msgAvaMoved         myGENMsg mhMsgBody !
			myGENMsg SendMessage drop
		then

		localAvatar @ dup x@ lastAvatarX f! z@ lastAvatarZ f!
	again
;

\ Calculate distance between two objects
: ObjDistance { o1 o2 -- }
	o1 x@ o2 x@ f- fdup f*
	o1 y@ o2 y@ f- fdup f*
	o1 z@ o2 z@ f- fdup f*
	f+ f+ fsqrt
;

fvariable lastMessageTime
variable  pingCount
variable  pingtask

: recv		\ msgaddr --

	>r		\ Put the msgaddr on the return stack

d\	." Roottool: " moduleOrigin . r@ mhMsgBody ? cr

	fGetTime lastMessageTime f!
	pingCount off
   
	r@ mhMsgBody @ 
	case

		msgPing? of  \ PING, reply with a PONG
			msgPong!				myGENMsg mhMsgBody !
			dup mhSrcModuleID @		myGENMsg mhDestModuleID !
			dup mhSrcHostID @		myGENMsg mhDestHostID !
			moduleOrigin			myGENMsg mhSrcModuleID !
			localHost				myGENMsg mhSrcHostID !
			myGENMsg				SendMessage drop \ Send the PONG
		endof

		msgMyState? of

			\ Add this to list of slaves - if not already there!
			r@ mhSrcHostID @   SearchIPList 0= if 
				r@ mhSrcHostID @ r@ mhSrcModuleID @ AddIPList
			then

			\ Answer the desperate cry for state....



			\ TO BE FILLED IN BY THE OBJECT:
			\ SEND A "msgYourState!" MESSAGE BACK TO
			\ THE hmID THAT SENT THE "msgMyState?"


		endof

		msgYourState! of

			\ TO BE FILLED IN BY THE OBJECT:
			\ RECEIVE A COMPLETE STATE MESSAGE
			\ SET ALL VARIABLES, UPDATE EVERYTHING 
			\ ACCORDINGLY

		endof

		\ Slave has left us....
		msgSlaveLeave of
			r@ mhSrcHostID @ r@ mhSrcModuleID @ DeleteIPListItem
		endof

		msgMasterDied of   \ The master died
			masterAlive off  
		endof      

		\ Replicate yourself to another host
		msgReplicateTo of
			msgCreateOBJ                myCOBMsg mhMsgBody !
			localHost                   myCOBMsg mhSrcHostID !
			moduleOrigin				myCOBMsg mhSrcModuleID !
			r@ mhMsgBody RPTHostID @	myCOBMsg mhDestHostID !
			deckID						myCOBMsg mhDestModuleID !
			masterIP @ if
				masterIP @   myCOBMsg mhMsgBody COBmasterHostID !
				masterMH @   myCOBMsg mhMsgBody COBmasterModuleID !
			else
				0            myCOBMsg mhMsgBody COBmasterHostID !
				moduleOrigin myCOBMsg mhMsgBody COBmasterModuleID !
			then

			\ TO BE FILLED IN BY THE OBJECT:         
			\ PUT THE URL OF THE SLAVE IN THE MESSAGE
			\ SOMETHING LIKE:
         
			c" panel/roottool.mm" myCOBMsg mhMsgBody COBslaveURL strcpy
         
			\ (MAKE SURE THE MESSAGE IS LARGE ENOUGH)

			\ Sanity check: An object will refuse to replicate to its 
			\ master's machine.

			myCOBMsg mhMsgBody COBmasterHostID @ 
			r@ mhMsgBody RPTHostID @ cr = 0= if
				myCOBMsg SendMessage drop
			then
		endof

		\ If we get a msgTouched, inform the deck we care about it!
		msgTouched of
			Highlight

			msgHandleTouched  myGENMsg mhMsgBody !
			moduleOrigin      myGENMsg mhSrcModuleID !
			localHost         myGENMsg mhSrcHostID !
			\ Send to local deck
			0                 myGENMsg mhDestModuleID !
			localHost         myGENMsg mhDestHostID !
			myGENMsg          SendMessage drop
		endof

		msgNotTouched of
			Unhighlight
		endof

		\ Deck queries me, do I want the mouse?
		msgWantMouse? of
			\ Own the mouse
			msgGrabMouse      myGENMsg mhMsgBody !
			moduleOrigin      myGENMsg mhSrcModuleID !
			localHost         myGENMsg mhSrcHostID !
			\ Send to local deck
			0                 myGENMsg mhDestModuleID !
			localHost         myGENMsg mhDestHostID !
			myGENMsg          SendMessage drop        
		endof

		msgLMouseDown of         
			Select

			\ If I am the master
			masterIP @ 0= if
				\ Send to myself
				moduleOrigin      myKICKMsg mhDestModuleID !
				localHost         myKICKMsg mhDestHostID !
			else
				\ Send to the master
				masterMH @        myKICKMsg mhDestModuleID !
				masterIP @        myKICKMsg mhDestHostID !
			then

			myKICKMsg          SendMessage drop
		endof

		msgLMouseUp of
			\ Disown the mouse...

			msgReleaseMouse   myGENMsg mhMsgBody !
			moduleOrigin      myGENMsg mhSrcModuleID !
			localHost         myGENMsg mhSrcHostID !
			\ Send to local deck
			0                 myGENMsg mhDestModuleID !
			localHost         myGENMsg mhDestHostID !
			myGENMsg          SendMessage drop        
		endof


		\ TO BE FILLED IN BY THE OBJECT:
		\ ANY OBJECT SPECIFIC MESSAGES GOES HERE

	endcase
	r> drop		\ Drop msgaddr
; TheReceiverMemeIs recv

\ If we are a slave, then send pings at regular intervals

: ping 
	begin
		fGetTime lastMessageTime f@ f-

		\ No message for more than ten seconds? Then send a ping!
		10e f> if
			msgPing?     myGENMsg mhMsgBody !
			moduleOrigin myGENMsg mhSrcModuleID !
			localHost    myGENMsg mhSrcHostID !
			masterMH @   myGENMsg mhDestModuleID !
			masterIP @   myGENMsg mhDestHostID !
			myGENMsg     SendMessage drop \ Send the PING
			fGetTime lastMessageTime f!
			pingCount @ 1 + pingCount !

			\ No message for one minute? Commit suicide!
			pingCount @ 6 > if
				masterAlive off		\ The master has died!

				msgDeleteObject  myDELMsg mhMsgBody !
				moduleOrigin     myDELMsg mhSrcModuleID !
				localHost        myDELMsg mhSrcHostID !
				\ Send to local deck
				0                myDELMsg mhDestModuleID !
				localHost        myDELMsg mhDestHostID !

				moduleOrigin     myDELMsg mhMsgBody DELmoduleID !

				myDELMsg         SendMessage drop \ Send the KILL ME
			then
		then

		pause 
	again 
;

: go
	if
		localAvatar ! \ Store the local avatar object
		masterMH !    \ Store the master's module handle
		masterIP !    \ Store the master's IP (0 if we are the master)

		\ DO ALL INITIALIZATION THAT'S COMMON TO BOTH MASTERS
		\ AND SLAVES

		rootSelected off    \ Root tool unselected at first
		consTool off        \ Must be zero or we can't create the tool

		ModuleWorld lockedLink plainVisible c" rwx/octozip.rwx"
		NewVisible toolIcon !

		NewMaterial iconMat !
		0.4e 0.2e 0e 0e iconMat @   MaterialSurface
		-1e -1e -1e     iconMat @   MaterialColor
		iconMat @       toolIcon @  ShapeMaterial

		-0.1e   toolIcon @      x!
		0.12e   toolIcon @      y!
		-0.3e   toolIcon @      z!
		0.01e   toolIcon @      rescale

		['] vibrate NewTask ?dup if vibrateTask ! then

		masterIP @ 0= if							\ If we are a master

			\ MASTER-SPECIFIC INITIALIZATION 

			localHost moduleOrigin AddIPList

			['] Resize NewTask ?dup if dup resizeTask ! wake then

		else										\ If we are a slave

			\ SLAVE-SPECIFIC INITIALIZATION 

			\ Ask the master our state!
			masterIP @   myGENMsg mhDestHostID !
			masterMH @   myGENMsg mhDestModuleID !
			moduleOrigin myGENMsg mhSrcModuleID !
			localHost    myGENMsg mhSrcHostID !
			msgMyState?  myGENMsg mhMsgBody !
			myGENMsg SendMessage drop 

			\ Launch the ping task
			['] ping NewTask ?dup if dup pingtask ! wake then

		then

	else	\ Tear down the root tool

d\		." roottool destructor" cr

		\ If there is a master, inform him this slave has died.
		masterIP @ if 
			\ And kill the ping task
			pingtask @ ?dup if DestroyTask then
			masterAlive @ if
				msgSlaveLeave  myGENMsg mhMsgBody !
				masterIP @     myGENMsg mhDestHostID !
				masterMH @     myGENMsg mhDestModuleID !
				moduleOrigin   myGENMsg mhSrcModuleID !
				localHost      myGENMsg mhSrcHostID !
				myGENMsg       SendMessage drop \ Let the master know we died
			then
		else \ Else inform all slaves that the MASTER has died!

			\ Tell all slaves to die!
			KillAllSlaves

		then
d\		." END roottool destructor" cr
	then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr p" roottool.mm" UnloadModuleToFile
forget foo


