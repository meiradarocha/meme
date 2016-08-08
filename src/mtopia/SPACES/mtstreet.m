\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Street Module for Metatopia
\ by Zap
\ Copyright (c) 1995, 1996 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 17Feb96 MdG	2.66b	Taken over by MdG.

.( Space module: mtstreet.m)

\ Test the startup module code

10000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

fload ..\mtmsgs.mh

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


      aMat @ ModuleWorld lockedLink plainVisible c" rwx\street0.rwx" NewVisible ShapeMaterial
      aMat @ ModuleWorld lockedLink plainVisible c" rwx\fountain.rwx" NewVisible ShapeMaterial


      aMat @ ModuleWorld lockedLink plainVisible c" rwx\isi0.rwx" NewVisible ShapeMaterial
      smoothShaded aMat @ MaterialShading
      aMat @ ModuleWorld lockedLink plainVisible c" rwx\isi1.rwx" NewVisible ShapeMaterial      
	ModuleWorld lockedLink plainVisible c" rwx/wings.rwx" NewVisible 
	?dup if
		10e dup x! 10e dup z! 3e dup rescale 45e dup yaw!
		drop
	then
	ModuleWorld lockedLink plainVisible c" rwx/dompyrmd.rwx" NewVisible
	?dup if
		-10e dup x! -10e dup z! 3e dup rescale 45e dup yaw!
		drop
	then 
	ModuleWorld lockedLink plainVisible c" rwx/slabs.rwx" NewVisible
	?dup if
		-10e dup x! 10e dup z! 3e dup rescale 45e dup yaw!
		drop
	then
   
   deadtask @ sleep
   pause 
again ;


\ This is where everything else is loaded - all movable objects e.t.c.
\ This is only called when this space is the first space loaded, i. e. the
\ current user is alone in the space. Otherwize, these things are *all*
\ created via replication!

: load_alive 
	0 0 localAvatar @ world lockedLink c" objects/mtsphere.mm" NewCompound 
	5e z!
;


\ Collision detection task...

fvariable oldX
fvariable oldZ
variable doneIt
variable collide

variable colltask

: coll begin
   collide off

   doneIt @ if
      \ The fountain in the middle
      localAvatar @ dup x@ fdup f* z@ fdup f* f+ 4e f< if collide on then

      \ The immersive systems HQ
      localAvatar @ x@ fdup 8e f> 19e f< and
      localAvatar @ z@ fdup -8e f< -19e f> and and if          
         \ The chamfered edge of ISI
         localAvatar @ dup x@ z@ f- 18e f> if 
            localAvatar @ dup x@ 9e f- fdup f* z@ -9e f- fdup f* f+ 2e f< if
               ." Teleporting...."

               colltask @ sleep \ Make sure it doesnt happen again!

               \ Teleport to other place!

               msgNewSpace    myENTMsg mhMsgBody !
               localHost      myENTMsg mhSrcHostID !
               moduleOrigin   myENTMsg mhSrcModuleID !
               localHost      myENTMsg mhDestHostID !
               0              myENTMsg mhDestModuleID ! \ Deck!

               \ Entry pos in ISI-HQ

               0.25e          myENTMsg mhMsgBody ENTnewX f!
               0.5e           myENTMsg mhMsgBody ENTnewY f!
               -1.5e          myENTMsg mhMsgBody ENTnewZ f!
               270e           myENTMsg mhMsgBody ENTnewYaw f!

               c" spaces\mtisihq.mm"  myENTMsg mhMsgBody ENTspaceID strcpy

               myENTMsg SendMessage drop
            else                        
               collide on 
            then
         then
      then

      collide @ if
         localAvatar @ dup oldX f@ x! oldZ f@ z!
      
         msgAvaMoved         myGENMsg mhMsgBody !
         localHost           myGENMsg mhSrcHostID !
         moduleOrigin        myGENMsg mhSrcModuleID !
         localHost           myGENMsg mhDestHostID !
         localAvatar @ ptr @ myGENMsg mhDestModuleID !
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

d\           ." mtstreet.mm: " msg @ mhMsgBody @ . cr

	msg @ mhMsgBody @
	case 
		msgSpaceInit of
			['] load_alive NewTask ?dup if wake then
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
   else
d\ ." Begin street destructor!" cr
      deadtask  @ ?dup if DestroyTask then
      colltask  @ ?dup if DestroyTask then

      lite      @ DestroyObject
      lite2     @ DestroyObject

      self      @ DestroyChildren

d\ ." End street destructor!" cr
   then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr ScribbleModule
' foo ModuleAddr p" mtstreet.mm" UnloadModuleToFile
forget foo

