\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Interior of the ISI HQ building
\ by Zap and MdG
\ Copyright (c) 1995-1998 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 11Aug98	MdG		Took out unnecessary material initialization.

.( Space module: mtisihq.m)

10000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

fload ../mtmsgs.mh

variable eye
variable lite
variable lite2

variable self
variable localAvatar

\ A general message structure for messages with only an integer body
create myGENMsg SizeOf MsgHeader 4 + dup allot
myGENMsg mhMsgLen !

\ A message for the replicaton request
create myENTMsg SizeOf MsgHeader SizeOf ENTMsg + 20 + dup allot
myENTMsg mhMsgLen !


\ This task loads all "dead" stuff, walls, and whatnot that doesn't need to
\ communicate with anybody or anything ever. Plain unmovable obstacles
\ could be created here, for instance....

variable deadtask

variable aMat

: load_dead begin 

		NewMaterial aMat !

		aMat @ ModuleWorld lockedLink plainVisible c" rwx\isi-hq.rwx" 
		NewVisible ShapeMaterial
   
		deadtask @ sleep
		pause 
	again ;


\ This is where everything else is loaded - all moveable objects e.t.c.
\ This is only called when this space is the first space loaded, i.e. the
\ current user is alone in the space. Otherwize, these things are *all*
\ created via replication!

variable alivetask

: load_alive begin 
   alivetask @ sleep
   pause 
again ;


\ Collision detection task...

fvariable oldX
fvariable oldZ
variable doneIt
variable collide

variable colltask

: coll begin
   collide off


   doneIt @ if
      \ The immersive systems HQ

      \ The stairs...
      localAvatar @ x@ 1e f<
      localAvatar @ z@ -0.5e f<
      localAvatar @ x@ -2.5e f> and and if
         localAvatar @ x@ 0.5e f<
         localAvatar @ z@ -1e f<
         localAvatar @ x@ -2e f> and and if
            localAvatar @ 0.5e y!
         else
            localAvatar @ 0.25e y!
         then
      else
         localAvatar @ 0e y!
      then



      localAvatar @ x@ fdup  0e f<  7e f> or
      localAvatar @ z@ fdup  0e f> -7e f< or or if          

         localAvatar @ x@ 0e f< 
         localAvatar @ z@ -1e f<
         localAvatar @ z@ -2e f> and and if
            ." Teleporting...."

            colltask @ sleep \ Make sure it doesnt happen again!

            \ Teleport to other place!

            msgNewSpace    myENTMsg mhMsgBody !
            localHost      myENTMsg mhSrcHostID !
            moduleOrigin   myENTMsg mhSrcModuleID !
            localHost      myENTMsg mhDestHostID !
            0              myENTMsg mhDestModuleID ! \ Deck!

            7e             myENTMsg mhMsgBody ENTnewX f!
            0e             myENTMsg mhMsgBody ENTnewY f!
            -7e            myENTMsg mhMsgBody ENTnewZ f!
            135e           myENTMsg mhMsgBody ENTnewYaw f!

            c" spaces\mtstreet.mm"  myENTMsg mhMsgBody ENTspaceID strcpy

            myENTMsg SendMessage drop         
         else
            collide on
         then
      then

      collide @ if
         localAvatar @ dup oldX f@ x! oldZ f@ z!
      
         localHost           myGENMsg mhSrcHostID !
         moduleOrigin        myGENMsg mhSrcModuleID !
         localHost           myGENMsg mhDestHostID !
         localAvatar @ ptr @ myGENMsg mhDestModuleID !
         msgAvaMoved         myGENMsg mhMsgBody !
         myGENMsg SendMessage drop
      then
   then

	doneIt on

	localAvatar @ dup x@ oldX f! z@ oldZ f!   
	pause
again ;

variable msg

: recv
	msg !

d\	." mtisihq.mm: " msg @ mhMsgBody ? cr

	msg @ mhMsgBody @
	case 
		msgSpaceInit of
			alivetask @ wake
		endof
   endcase
;

TheReceiverMemeIs recv 

: go
   if
      localAvatar ! 
      drop 
      drop

      moduleOrigin WhichObject self !
      ModuleWorld lockedLink directionalLight NewLight lite  !
      lite @ 75e pitch!
      lite @ 0.8e 0.8e 0.8e LightColor

      ModuleWorld lockedLink directionalLight NewLight lite2 !
      lite2 @ -55e pitch!
      lite2 @ -20e roll!
      lite2 @ 0.6e 0.6e 0.7e LightColor

      ['] coll       NewTask ?dup if dup colltask ! wake then
      ['] load_dead  NewTask ?dup if dup deadtask ! wake then
      ['] load_alive NewTask alivetask ! 
   else
d\	." Begin mtisihq destructor!" cr
      deadtask  @ ?dup if DestroyTask then
      alivetask @ ?dup if DestroyTask then
      colltask  @ ?dup if DestroyTask then

      lite      @ DestroyObject
      lite2     @ DestroyObject

      self      @ DestroyChildren
d\	." End mtisihq destructor!" cr
   then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr ScribbleModule
' foo ModuleAddr p" mtisihq.mm" UnloadModuleToFile
forget foo

