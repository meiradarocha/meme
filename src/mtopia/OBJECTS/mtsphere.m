\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Ball Module for Metatopia
\ by Zap
\ Copyright (c) 1995, 1996 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 25Aug96 MdG	2.69b	Some optimization of the code.
\ 17Feb96 MdG	2.66b	Taken over by MdG.


.( Object module: mtsphere.m)

11000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

variable masterIP		\ The IP of the master
variable masterMH		\ The Module Handle of the master
variable masterAlive	\ Is the master alive?
variable localAvatar	\ The Local Avatar object (for collision detection etc)

variable sphe			\ The object

variable Mat1
variable Mat2

fload ../mtmsgs.mh

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

			msgAvaMoved         myGENMsg mhMsgBody !
			localHost           myGENMsg mhSrcHostID !
			moduleOrigin        myGENMsg mhSrcModuleID !
			localHost           myGENMsg mhDestHostID !
			localAvatar @ ptr @ myGENMsg mhDestModuleID !
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
variable  msg

: recv
	msg ! 

d\		." Ball: " moduleOrigin . msg @ mhMsgBody ? cr

	fGetTime lastMessageTime f!
	pingCount off
   
	msg @ mhMsgBody @ case
		msgPing? of  \ PING, reply with a PONG
			msgPong!              myGENMsg mhMsgBody !
			msg @ mhSrcModuleID @ myGENMsg mhDestModuleID !
			msg @ mhSrcHostID @   myGENMsg mhDestHostID !
			moduleOrigin          myGENMsg mhSrcModuleID !
			localHost             myGENMsg mhSrcHostID !
			myGENMsg              SendMessage drop \ Send the PONG
		endof
		msgMyState? of
			\ Add this to list of slaves - if not already there!

			msg @ mhSrcHostID @   SearchIPList 0= if 
				msg @ mhSrcHostID @ msg @ mhSrcModuleID @ AddIPList
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
			msg @ mhSrcHostID @ 
			msg @ mhSrcModuleID @ DeleteIPListItem
		endof

		msgMasterDied of   \ The master died
			masterAlive off  
		endof      

		\ Replicate yourself to another host
		msgReplicateTo of
			msgCreateOBJ                myCOBMsg mhMsgBody !
			localHost                   myCOBMsg mhSrcHostID !
			moduleOrigin                myCOBMsg mhSrcModuleID !
			msg @ mhMsgBody RPTHostID @ myCOBMsg mhDestHostID !
			0                           myCOBMsg mhDestModuleID ! \ Deck module
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
         
			c" mtsphere.mm" myCOBMsg mhMsgBody COBslaveURL strcpy
         
			\ (MAKE SURE THE MESSAGE IS LARGE ENOUGH)

			\ Sanity check: An object will refuse to replicate to its 
			\ master's machine.

			myCOBMsg mhMsgBody COBmasterHostID @ 
			msg @ mhMsgBody RPTHostID @ cr = 0= if
				myCOBMsg SendMessage drop
			then
		endof

		\ If we get a msgTouched, inform the deck we care about it!
		msgTouched of
			Mat2 @ sphe @ ShapeMaterial

			msgHandleTouched  myGENMsg mhMsgBody !
			moduleOrigin      myGENMsg mhSrcModuleID !
			localHost         myGENMsg mhSrcHostID !
			\ Send to local deck
			0                 myGENMsg mhDestModuleID !
			localHost         myGENMsg mhDestHostID !
			myGENMsg          SendMessage drop
		endof

		msgNotTouched of
			Mat1 @ sphe @ ShapeMaterial
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
			msgKickRequest    myKICKMsg mhMsgBody !
			moduleOrigin      myKICKMsg mhSrcModuleID !
			localHost         myKICKMsg mhSrcHostID !

			localAvatar @ ModuleWorld ObjDistance fdup f* fdup

			localAvatar @ x@ ModuleWorld x@ f- fswap f/ -18e f* 
			ModuleWorld x@ f+
			myKICKMsg mhMsgBody KICKEndX f!

			localAvatar @ z@ ModuleWorld z@ f- fswap f/ -18e f* 
			ModuleWorld z@ f+
			myKICKMsg mhMsgBody KICKEndZ f!

         
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

		\ Ball kick request from slave
		msgKickRequest of
			\ Convert to a "KickTo" message
			msgKickTo			msg @ mhMsgBody !
			ModuleWorld x@		msg @ mhMsgBody KICKStartX f!
			ModuleWorld z@		msg @ mhMsgBody KICKStartZ f!
			moduleOrigin		msg @ mhSrcModuleID !

			\ Send it again to all slaves!
			msg @ SynMessageToAll
		endof

		\ Ball was kicked
		msgKickTo of
			fGetTime 5e f+ kickEndTime f!
			msg @ mhMsgBody KICKStartX f@ kickStartX f!
			msg @ mhMsgBody KICKStartZ f@ kickStartZ f!
			msg @ mhMsgBody KICKEndX f@ kickEndX f!
			msg @ mhMsgBody KICKEndZ f@ kickEndZ f!
		endof
	endcase
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

		ModuleWorld lockedLink plainVisible c" sphere.rwx" NewVisible sphe !

		sphe @ 0.25e y!
		0.25e sphe @ rescale

		NewMaterial Mat1 !
		NewMaterial Mat2 !

		1e 1e 1e Mat1 @ MaterialColor
		1e 1e 1e Mat2 @ MaterialColor

		smoothShaded Mat1 @ MaterialShading
		smoothShaded Mat2 @ MaterialShading

		0.2e 0.5e 0.1e 0e Mat1 @ MaterialSurface
		0.5e 0.2e 0.1e 0e Mat2 @ MaterialSurface

		Mat1 @ sphe @ ShapeMaterial

		\ The "roll the ball" task
		['] rollball NewTask ?dup if dup rolltask ! wake then

		\ If we are a master then:
		masterIP @ 0= if
			localHost moduleOrigin AddIPList

			\ TO BE FILLED IN BY THE OBJECT
			\ LAUNCH TASKS ONLY FOR MASTERS (brains)

		else \ If we are a slave, then:
			masterIP @   myGENMsg mhDestHostID !
			masterMH @   myGENMsg mhDestModuleID !
			moduleOrigin myGENMsg mhSrcModuleID !
			localHost    myGENMsg mhSrcHostID !
			msgMyState?  myGENMsg mhMsgBody !

			\ Ask the master our state!
			myGENMsg SendMessage drop 

			\ FIX: the avatar module sets masterAlive to "on"
			\ at this spot in the code.

			\ Launch the ping task
			['] ping NewTask ?dup if dup pingtask ! wake then
		then
	else

d\		." Sphere destructor" cr

		rolltask @ ?dup if DestroyTask then
		sphe @ ?dup if DestroyObject then

		\ If there is a master, inform him this slave has died.
		masterIP @ if 
			\ And kill the ping task
			pingtask @ ?dup if DestroyTask then
			masterAlive @ if
				moduleOrigin   myGENMsg mhSrcModuleID !
				localHost      myGENMsg mhSrcHostID !
				masterIP @     myGENMsg mhDestHostID !
				masterMH @     myGENMsg mhDestModuleID !
				msgSlaveLeave  myGENMsg mhMsgBody !
				myGENMsg       SendMessage drop \ Let the master know we died
			then
		else \ Else inform all slaves that the MASTER has died!

			\ Tell all slaves to die!
			KillAllSlaves

		then
d\		." END Sphere destructor" cr
	then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr p" mtsphere.mm" UnloadModuleToFile
forget foo


