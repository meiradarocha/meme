\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

.( Avatar module: mtavatar.m)

\ Zap's Avatar, pre-pre-pre-alpha version
\ 
\ This module only defines the visual appearance of the avatar, 
\ and nothing else.

35000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

create avatarName 100 allot

\ VIEW RELATED STUFF

0 constant view1stPerson
1 constant viewFront
2 constant viewRight
3 constant viewBack
4 constant viewLeft
5 constant viewNorth
6 constant viewEast
7 constant viewSouth
8 constant viewWest

variable current_view

fvariable view_distance

\ END VIEW RELATED STUFF

fvariable baseTime

fvariable bandWidthTime
variable  bandWidthBytes

variable HeadTex

: SendMessage
   dup mhMsgLen @ bandWidthBytes @ + 32 + bandWidthBytes !
   SendMessage
;

variable self



variable kopf
variable halz

variable kopfCam
variable halzCam

variable puppetPin \ Holds the small head in the camera view

variable masterIP    \ The IP of the master
variable masterMH    \ The Module Handle of the master
variable masterAlive \ Is the master alive?
variable localAvatar \ The Local Avatar object (for collision detection e.t.c.)

variable panel			\ Root of the instrument panel

fload mtmsgs.mh


\ ----------- NAVIGATION CODE

variable current_speed
variable current_turnFactor
variable current_button
variable current_face

fvariable centerX   \ Turning center point
fvariable centerZ   \ -"-
fvariable radPerSec \ Turning speed in radians per second
fvariable baseAngle \ BaseAngle
fvariable distToCen \ distToTurnCenter

fvariable lastTimestamp   \ Last remote timestamp
fvariable localTimestamp  \ Last local timestamp

\ Calculate the data needed to dead-reckon the current movement

: CalcDeadReckonData
   \ Special case: Straight travel

   current_turnFactor @ 0= if
      self @ x@ centerX f!
      self @ z@ centerZ f!
      current_speed @ float 10e f/ radPerSec f!
   else      
      current_turnFactor @ float -20e f/ radPerSec f!
      self @ yaw@ baseAngle f!

      current_speed @ float 10e f/ radPerSec f@ f/ distToCen f!
      baseAngle f@ 90e f+ d>r fsin distToCen f@ f* self @ x@ f+ centerX f!
      baseAngle f@ 90e f+ d>r fcos distToCen f@ f* self @ z@ f+ centerZ f!
   then
;

: turnHeadToDesiredPos { | desiredYaw desiredPitch -- }
   desiredYaw halz @ yaw@ f- 0.5e f* halz @ dup yaw@ f+ fdup yaw! halzCam @ yaw!
   desiredPitch kopf @ pitch@ f- 0.5e f* kopf @ dup pitch@ f+ fdup pitch! kopfCam @ pitch!
;

\ Set the position of the self object based on passed elapsed time

: CalcPosition { | par -- }
   \ Special case: Straight travel

   current_button @ rightButton and if
      current_turnFactor @ float -2e f* 
      current_speed @      float  2e f* 
         turnHeadToDesiredPos
   else
      \ 0e 0e turnHeadToDesiredPos
   then

   current_button @ leftButton and if
      current_turnFactor @ 0= if
         self @ yaw@ d>r fsin radPerSec f@ f* par f* centerX f@ f+ self @ x!
         self @ yaw@ d>r fcos radPerSec f@ f* par f* centerZ f@ f+ self @ z!
      else
    baseAngle f@ radPerSec f@ r>d par f* f+ self @ yaw!
         baseAngle f@ 90e f- d>r radPerSec f@ par f* f+ fdup
         fsin distToCen f@ f* centerX f@ f+ self @ x!
         fcos distToCen f@ f* centerZ f@ f+ self @ z!
      then
   then

;

\ Object-specific messages are negative
-1    constant msgNavigate
-100  constant msgChat
-1000 constant msgFaceNorm
-1001 constant msgFaceSmile
-1002 constant msgFaceAngry
-1003 constant msgFaceSad

\ Structure of a "Navigate" message
BeginStruct
   Int   NAVmsgCode
   Float NAVtimeStamp
   Int   NAVturnFactor
   Int   NAVspeed
   Int   NAVbutt
Struct NAVMsg

create myNAVMsg SizeOf MsgHeaders SizeOf NAVMsg + dup allot
myNAVMsg mhMsgLen !

\ Structure of a "Complete update" message
BeginStruct
   Int   CUDmsgCode
   Float CUDtimeStamp
   Float CUD_X
   Float CUD_Y
   Float CUD_Z
   Float CUD_Yaw
   Float   CUD_HeadYaw
   Float   CUD_HeadPitch
   Int   CUDturnFactor
   Int   CUDspeed
   Int   CUDface
   Int   CUDbutton
Struct CUDMsg

create myCUDMsg SizeOf MsgHeaders SizeOf CUDMsg + dup allot
myCUDMsg mhMsgLen !

\ A general message structure for messages with only an integer body
create myGENMsg SizeOf MsgHeaders 4 + dup allot
myGENMsg mhMsgLen !


\ A message for the create-object call
create myCOBMsg SizeOf MsgHeaders SizeOf COBMsg + 20 + dup allot
myCOBMsg mhMsgLen !

\ Structure for the chat message
BeginStruct
   Int TXTmsgCode
   Lbl TXTChat
Struct TXTMsg

create myTXTMsg SizeOf MsgHeaders SizeOf TXTMsg + 100 + dup allot
myTXTMsg mhMsgLen !

variable cursorpos

\ Time of the most recently received message
fvariable lastMessageTime

\ # of pings sent by the slave since the master last sent the slave a msg
variable  pingCount	


variable msg

variable walkButton
variable headButton

: recv
	msg ! 

d\	." Avatar: " moduleOrigin u. msg @ mhMsgBody ? cr

	fGetTime lastMessageTime f!
	pingCount off					\ Zero the ping count
   
	msg @ mhMsgBody @
	case
		msgPing? of  \ PING, reply with a PONG
			 localHost             myGENMsg mhSrcHostID !
			 moduleOrigin          myGENMsg mhSrcModuleID !
			 msg @ mhSrcHostID @   myGENMsg mhDestHostID !
			 msg @ mhSrcModuleID @ myGENMsg mhDestModuleID !
			 msgPong!              myGENMsg mhMsgBody !
			 myGENMsg              SendMessage drop \ Send the PONG
		endof

      msgMyState? of
         \ Add this to list of slaves - if not already there!

         msg @ mhSrcHostID @ msg @ mhSrcModuleID @ AddIPList

         \ Answer the desperate cry for state....

         msgYourState!         myCUDMsg mhMsgBody !
         msg @ mhSrcModuleID @ myCUDMsg mhDestModuleID !
         msg @ mhSrcHostID @   myCUDMsg mhDestHostID !
         moduleOrigin          myCUDMsg mhSrcModuleID !
         localHost             myCUDMsg mhSrcHostID !
         fGetTime              myCUDMsg mhMsgBody CUDtimeStamp
         self @ x@             myCUDMsg mhMsgBody CUD_X f!
         self @ y@             myCUDMsg mhMsgBody CUD_Y f!
         self @ z@             myCUDMsg mhMsgBody CUD_Z f!
         self @ yaw@           myCUDMsg mhMsgBody CUD_Yaw f!
         halz @ yaw@           myCUDMsg mhMsgBody CUD_HeadYaw f!
         kopf @ pitch@         myCUDMsg mhMsgBody CUD_HeadPitch f!

         current_speed @       myCUDMsg mhMsgBody CUDspeed !
         current_turnFactor @  myCUDMsg mhMsgBody CUDturnFactor !
         current_face @        myCUDMsg mhMsgBody CUDface !
         current_button @      myCUDMsg mhMsgBody CUDbutton !

         myCUDMsg              SendMessage drop \ Send the CUD
      endof
      msgYourState! of
         msg @ mhMsgBody CUDtimeStamp f@  lastTimestamp f!
         msg @ mhMsgBody CUD_X f@         self @ x!
         msg @ mhMsgBody CUD_Y f@         self @ y!
         msg @ mhMsgBody CUD_Z f@         self @ z!
         msg @ mhMsgBody CUD_Yaw f@       self @ yaw!
         msg @ mhMsgBody CUD_HeadYaw f@   fdup halz @ yaw! halzCam @ yaw!
         msg @ mhMsgBody CUD_HeadPitch f@ fdup kopf @ pitch! kopfCam @ pitch!

         msg @ mhMsgBody CUDspeed @       current_speed !
         msg @ mhMsgBody CUDturnFactor @  current_turnFactor !
         msg @ mhMsgBody CUDface @        current_face !
         msg @ mhMsgBody CUDbutton @      current_button !

         current_face @ HeadTex @ SetTextureFrame

         fGetTime localTimestamp f!

         CalcDeadReckonData  \ Calculate variables for dead-reckoning motion
      endof

      \ A slave has left us...
      msgSlaveLeave of
         msg @ mhSrcHostID @ 
         msg @ mhSrcModuleID @ DeleteIPListItem
      endof

      msgMasterDied of   \ The master died
         masterAlive off

d\       cr ." Got 'MasterDied' message" cr
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
         
         c" mtavatar.mm" myCOBMsg mhMsgBody COBslaveURL strcpy

         \ Sanity check: An object will refuse to replicate to it's masters machine.

         myCOBMsg mhMsgBody COBmasterHostID @ msg @ mhMsgBody RPTHostID @ cr = 0= if
       myCOBMsg SendMessage drop
         then
      endof

      msgLogOut of
         KillAllSlaves \ Get rid of all slaves
      endof

      msgNavigate of
         msg @ mhMsgBody NAVtimeStamp f@  lastTimestamp f@ f- CalcPosition

         msg @ mhMsgBody NAVtimeStamp f@  lastTimestamp f! 
         msg @ mhMsgBody NAVspeed @       current_speed !
         msg @ mhMsgBody NAVturnFactor @  current_turnFactor !

         msg @ mhMsgBody NAVbutt @        current_button !

         fGetTime localTimestamp f!

         CalcDeadReckonData  \ Calculate variables for dead-reckoning motion
      endof

      msgChat of
         cursorpos @ if cursorpos @ -1 do 8 emit loop then 
         ." <" avatarName ctype ." > " msg @ mhMsgBody TXTChat ctype cr
         cursorpos @ if myTXTMsg mhMsgBody TXTChat ctype then
      endof

      msgFaceNorm of
         0 dup current_face ! HeadTex @ SetTextureFrame
      endof

      msgFaceSmile of
         1 dup current_face ! HeadTex @ SetTextureFrame
      endof

      msgFaceAngry of
         2 dup current_face ! HeadTex @ SetTextureFrame
      endof

      msgFaceSad of
         3 dup current_face ! HeadTex @ SetTextureFrame
      endof   

      msgKeyPress of
         msg @ mhMsgBody KEYascii @ case  
            2 of \ View change
               current_view @ 1+ 9 mod current_view !               
            endof       

            4 of
               view_distance f@ 1.2e f* view_distance f!
            endof
            
            5 of
               view_distance f@ 1.2e f/ view_distance f!
            endof

            19 of \ Smile
               msgFaceSmile myGENMsg mhMsgBody !
               localHost    myGENMsg mhSrcHostID ! 
               moduleOrigin myGENMsg mhSrcModuleID !
               myGENMsg     SynMessageToAll   
            endof       
            3 of \ Cry
               msgFaceSad   myGENMsg mhMsgBody !
               localHost    myGENMsg mhSrcHostID ! 
               moduleOrigin myGENMsg mhSrcModuleID !
               myGENMsg     SynMessageToAll   
            endof       
            1 of \ Angry
               msgFaceAngry myGENMsg mhMsgBody !
               localHost    myGENMsg mhSrcHostID ! 
               moduleOrigin myGENMsg mhSrcModuleID !
               myGENMsg     SynMessageToAll   
            endof       
            24 of \ Neutral
               msgFaceNorm  myGENMsg mhMsgBody !
               localHost    myGENMsg mhSrcHostID ! 
               moduleOrigin myGENMsg mhSrcModuleID !
               myGENMsg     SynMessageToAll   
            endof       
            13 of
               cr
               cursorpos @ if
                  msgChat      myTXTMsg mhMsgBody !
                  localHost    myTXTMsg mhSrcHostID ! 
                  moduleOrigin myTXTMsg mhSrcModuleID !
                  localHost    myTXTMsg mhDestHostID !
                  moduleOrigin myTXTMsg mhDestModuleID !
                  [SizeOf] MsgHeaders cursorpos @ + 5 + 
                  myTXTMsg mhMsgLen !

                  cursorpos @ if cursorpos @ -1 do 8 emit loop then 
                  0            cursorpos !
                  myTXTMsg SynMessageToAll
               then
            endof
            8 of
               8 emit 
               cursorpos @ 1 - cursorpos !
               0 myTXTMsg mhMsgBody TXTChat cursorpos @ + c!
            endof
            dup dup of
               dup emit 
                   myTXTMsg mhMsgBody TXTChat cursorpos @ + c!
               cursorpos @ 1 + cursorpos !
               0 myTXTMsg mhMsgBody TXTChat cursorpos @ + c!
            endof
         endcase
      endof

      msgLMouseDown of
         leftButton walkButton !
      endof

      msgLMouseUp of
         0 walkButton !

         \ No longer walking - send my position!

         msgYourState!         myCUDMsg mhMsgBody !
         moduleOrigin          myCUDMsg mhDestModuleID !
         localHost             myCUDMsg mhDestHostID !
         moduleOrigin          myCUDMsg mhSrcModuleID !
         localHost             myCUDMsg mhSrcHostID !
         fGetTime              myCUDMsg mhMsgBody CUDtimeStamp f!
         self @ x@             myCUDMsg mhMsgBody CUD_X f!
         self @ y@             myCUDMsg mhMsgBody CUD_Y f!
         self @ z@             myCUDMsg mhMsgBody CUD_Z f!
         self @ yaw@           myCUDMsg mhMsgBody CUD_Yaw f!
         halz @ yaw@           myCUDMsg mhMsgBody CUD_HeadYaw f!
         kopf @ pitch@         myCUDMsg mhMsgBody CUD_HeadPitch f!

         0                     myCUDMsg mhMsgBody CUDspeed !
         0                     myCUDMsg mhMsgBody CUDturnFactor !
         current_face @        myCUDMsg mhMsgBody CUDface !
         walkButton @ headButton @ + myCUDMsg mhMsgBody CUDbutton !

         myCUDMsg SynMessageToAll \ Send the CUD to everybody
      endof

      msgRMouseDown of
         rightButton headButton !
      endof

      msgRMouseUp of
         0 headButton !
      endof

      \ Avatar was repositioned by an external force!
      msgAvaMoved of         

         \ No longer walking - send my position!

         msgYourState!         myCUDMsg mhMsgBody !
         moduleOrigin          myCUDMsg mhDestModuleID !
         localHost             myCUDMsg mhDestHostID !
         moduleOrigin          myCUDMsg mhSrcModuleID !
         localHost             myCUDMsg mhSrcHostID !
         fGetTime              myCUDMsg mhMsgBody CUDtimeStamp f!
         self @ x@             myCUDMsg mhMsgBody CUD_X f!
         self @ y@             myCUDMsg mhMsgBody CUD_Y f!
         self @ z@             myCUDMsg mhMsgBody CUD_Z f!
         self @ yaw@           myCUDMsg mhMsgBody CUD_Yaw f!
         halz @ yaw@           myCUDMsg mhMsgBody CUD_HeadYaw f!
         kopf @ pitch@         myCUDMsg mhMsgBody CUD_HeadPitch f!

         \ Zero speed!
         0                     myCUDMsg mhMsgBody CUDspeed !
         current_turnFactor @  myCUDMsg mhMsgBody CUDturnFactor !
         current_face @        myCUDMsg mhMsgBody CUDface !
         walkButton @ headButton @ + myCUDMsg mhMsgBody CUDbutton !

         myCUDMsg SynMessageToAll \ Send the CUD to everybody
      endof

   endcase

; TheReceiverMemeIs recv

variable mtask \ The mouse task

fvariable nextMouseTime

variable oldMouseX
variable oldMouseY

\ Save the mouse position

: StoreMousePos
  MouseX oldMouseX !
  MouseY oldMouseY !
;

\ ( amount -- didit? )

: HasMouseChanged { dist -- }
  walkButton @ headButton @ or if
    dist MouseX oldMouseX @ - abs <   \ (  -- toBigX? )
    dist MouseY oldMouseY @ - abs <   \ (  -- toBigY? )
    or                                \ ( toBigX? toBigY? -- Changed? )
  else
    0
  then
;

: nonLinear
   float float fswap f/ 2e f* 1e f- fdup fabs f* 50e f* fix
;

\ Navigate according to mouse
: DoNavigation 

     MouseX WindowWidth  nonLinear  myNAVMsg mhMsgBody NAVturnFactor !
     MouseY WindowHeight nonLinear  myNAVMsg mhMsgBody NAVspeed !

     msgNavigate   myNAVMsg mhMsgBody !

     walkButton @ headButton @ + myNAVMsg mhMsgBody NAVbutt !
     moduleOrigin  myNAVMsg mhSrcModuleID !
     localHost     myNAVMsg mhSrcHostID !
     moduleOrigin  myNAVMsg mhDestModuleID !
     localHost     myNAVMsg mhDestHostID !
     fGetTime      myNAVMsg mhMsgBody NAVtimeStamp f!

    \ Send the message to all slaves
    myNAVMsg      MessageToAll
;

: mouse begin
	fGetTime bandWidthTime f@ f- 60e f> if
		bandWidthBytes @ float 
		fGetTime bandWidthTime f@ f- f/
d\		bandWidthBytes @ . ." bytes, " f. ." bytes/sec" cr      
		bandWidthBytes off 
		fGetTime bandWidthTime f!
	then

   \ Position the puppetPin, if existing

   puppetPin @ if
      WindowWidth  float 10000e f/  -0.01e  f+ puppetPin @ x!
      WindowHeight float -10000e f/  0.005e f+ puppetPin @ y!
   then

   \ Do navigational stuff if 0.5 seconds has passed since last time,
   \ or the mouse has moved more than 20 pixels


   fGetTime nextMouseTime f@ f> 
   10 HasMouseChanged or
   if
      fGetTime 0.5e f+ nextMouseTime f!

      1 HasMouseChanged 
      if 
        DoNavigation
        StoreMousePos
      then      
   then

   pause 
again ;

\ The following code is screwed up somehow.  It is only supposed to
\ run on a slave, but if no message is heard from the master for
\ one minute, it tells the local deck to destroy the module
\ this code is running in.  It uses msgDeleteObject which is
\ for destroying masters.
\ Why does the avatar act like both a slave and a master?

\ If we are a slave, then send pings at regular intervals
: ping begin
  fGetTime lastMessageTime f@ f-

  \ No message for more than ten seconds?  Then send a ping!
  10e f> if
     localHost    myGENMsg mhSrcHostID !
     moduleOrigin myGENMsg mhSrcModuleID !
     masterIP @   myGENMsg mhDestHostID !
     masterMH @   myGENMsg mhDestModuleID !
     msgPing?     myGENMsg mhMsgBody !
     myGENMsg     SendMessage drop \ Send the PING

	\ Record the time of the ping message, and bump the count of
	\ pings sent since a message was heard from the master.
     fGetTime lastMessageTime f!
     1 pingCount +!

     \ No message from the master for one minute? Commit suicide!
     pingCount @ 6 > if
        masterAlive off  \ Set local flag indicating the master is dead

		\ Send a message to the local deck to delete the master..
		\ ...which is a suicide...why is this module both master and slave?
        localHost        myDELMsg mhSrcHostID !
        moduleOrigin     myDELMsg mhSrcModuleID !
        localHost        myDELMsg mhDestHostID !
        deckID           myDELMsg mhDestModuleID !
        msgDeleteObject  myDELMsg mhMsgBody !
        moduleOrigin     myDELMsg mhMsgBody DELmoduleID !
        myDELMsg         SendMessage drop \ Send the KILL MYSELF message
     then
  then

  pause 
again ;


\ ----------- APPEARANCE CODE





variable eye

variable BallMat
variable BallTex

variable HeadMat

variable stask
variable atask

BeginStruct
   Int HoldObj
   Int Obj
   Int Mat
   Int Tex
   Int Rwx
Struct BodyParts

create pelv SizeOf BodyParts allot
create body SizeOf BodyParts allot
create neck SizeOf BodyParts allot
create head SizeOf BodyParts allot

create neckCam SizeOf BodyParts allot
create headCam SizeOf BodyParts allot

create lth SizeOf BodyParts allot
create lan SizeOf BodyParts allot
create lft SizeOf BodyParts allot
create rth SizeOf BodyParts allot
create ran SizeOf BodyParts allot
create rft SizeOf BodyParts allot


create rbc SizeOf BodyParts allot
create rwr SizeOf BodyParts allot
create rhn SizeOf BodyParts allot

create lbc SizeOf BodyParts allot
create lwr SizeOf BodyParts allot
create lhn SizeOf BodyParts allot



fvariable legSeparation
fvariable thLength
fvariable anLength
fvariable footHeight

fvariable armSeparation
fvariable pelvHeight
fvariable bodyHeight
fvariable armHeight
fvariable neckHeight

fvariable uarmLength
fvariable larmLength

fvariable totLeg       \ The length of the entire leg
fvariable walkHeight   \ Height of pelvis from ground when walking

fvariable stepAmpl     \ Amplitude of steps (i.e. step-length / 2)
fvariable liftFactor   \ How much the feet is lifted in fraction of stepAmpl

: SetSkelPositions
   pelv HoldObj @ 180e yaw! \ Look North, you asshole!

   rth HoldObj @ legSeparation f@  x!
   lth HoldObj @ 0e legSeparation f@ f-  x!

   ran HoldObj @ 0e thLength f@ f-  y!
   lan HoldObj @ 0e thLength f@ f-  y!

   rft HoldObj @ 0e anLength f@ f-  y!
   lft HoldObj @ 0e anLength f@ f-  y!   

   body HoldObj @ pelvHeight f@  y!
   neck HoldObj @ bodyHeight f@  y!
   head HoldObj @ neckHeight f@  y!

   neckCam HoldObj @ bodyHeight f@  y!
   headCam HoldObj @ neckHeight f@  y!

   rbc HoldObj @ armHeight f@  y!
   lbc HoldObj @ armHeight f@  y!

   lbc HoldObj @ armSeparation f@  x!
   rbc HoldObj @ 0e armSeparation f@ f-  x!

   rwr HoldObj @ 0e uarmLength f@ f-  y!
   lwr HoldObj @ 0e uarmLength f@ f-  y!

   \ Calculate some needed walk factors
   footHeight f@ thLength f@ anLength f@ f+ f+ totLeg f!

   pelv HoldObj @ totLeg f@ y!

   totLeg f@ fdup f* stepAmpl f@ fdup f* f- fsqrt walkHeight f!
;

: CreateSkeleton
   ModuleWorld lockedLink NewImaginary    pelv HoldObj !
   pelv HoldObj @ lockedLink NewImaginary rth  HoldObj !
   pelv HoldObj @ lockedLink NewImaginary lth HoldObj !
   lth  HoldObj @ lockedLink NewImaginary lan HoldObj !
   rth  HoldObj @ lockedLink NewImaginary ran HoldObj !
   lan  HoldObj @ lockedLink NewImaginary lft HoldObj !
   ran  HoldObj @ lockedLink NewImaginary rft HoldObj !   

   pelv HoldObj @ lockedLink NewImaginary body HoldObj !   
   body HoldObj @ lockedLink NewImaginary neck HoldObj !   
   neck HoldObj @ lockedLink NewImaginary head HoldObj !   

   \ The Camera holding objects
   body    HoldObj @ lockedLink NewImaginary neckCam HoldObj !   
   neckCam HoldObj @ lockedLink NewImaginary headCam HoldObj !   

   body HoldObj @ lockedLink NewImaginary rbc HoldObj !   
   rbc  HoldObj @ lockedLink NewImaginary rwr HoldObj !   

   body HoldObj @ lockedLink NewImaginary lbc HoldObj !   
   lbc  HoldObj @ lockedLink NewImaginary lwr HoldObj !   
;


\ Destroy the skeleton and it's child objects

: DeleteSkeleton
   pelv HoldObj @ DestroyChildren
   rth  HoldObj @ DestroyChildren
   ran  HoldObj @ DestroyChildren
   rft  HoldObj @ DestroyChildren
   lth  HoldObj @ DestroyChildren
   lan  HoldObj @ DestroyChildren
   lft  HoldObj @ DestroyChildren

   body  HoldObj @ DestroyChildren
   neck  HoldObj @ DestroyChildren
   head  HoldObj @ DestroyChildren

   rbc  HoldObj @ DestroyChildren
   rwr  HoldObj @ DestroyChildren  

   lbc  HoldObj @ DestroyChildren
   lwr  HoldObj @ DestroyChildren  

   \ New the skeleton itself

   pelv HoldObj @ DestroyObject
   rth  HoldObj @ DestroyObject
   ran  HoldObj @ DestroyObject
   rft  HoldObj @ DestroyObject
   lth  HoldObj @ DestroyObject
   lan  HoldObj @ DestroyObject
   lft  HoldObj @ DestroyObject

   body  HoldObj @ DestroyObject
   neck  HoldObj @ DestroyObject
   head  HoldObj @ DestroyObject

   rbc  HoldObj @ DestroyObject
   rwr  HoldObj @ DestroyObject  

   lbc  HoldObj @ DestroyObject
   lwr  HoldObj @ DestroyObject  
;


variable spam

fvariable walkphase
fvariable stepX
fvariable stepY
fvariable dist2foot
fvariable ang2foot
fvariable Gdist
fvariable Xdist

: linsin
  2e fmod fdup
  1e f> if
     2e fswap f- 
  then
;  

: fneg
  0e fswap f-
;

: fsign
  0e f< if 1e else -1e then
;

variable  autoView
fvariable autoView_base_angle

: SetViewStuff   
	\ This code must only execute on a master!!
	masterIP @ if exit then

	autoView off
	current_view @ view1stPerson = if

		\ Attach the eye to the camera wielding object
		eye @ Orphan drop
		kopfCam @ eye @ Adopts drop

		eye @ 180e yaw!		\ Compensate for the feet pointing backwards
		eye @ 10e  pitch!
		eye @ 0e roll!
		eye @ 0.2e y!         
		eye @ 0e z!
		eye @ 0e x!
	
		\ Now orphan the REAL head, and child it to puppetPin!
		halz @ Orphan drop
		0e halz @ y! \ Place the neck on zero
		puppetPin @ halz @ Adopts drop

	else

		\ Put the camera onto the body object
		eye @ Orphan drop
		body HoldObj @ eye @ Adopts drop

		\ Put head back on the body....
		halz @ Orphan drop
		body HoldObj @ halz @ Adopts drop
		bodyHeight f@  halz @ y! \ Place the neck at the rite pleyz

		eye @ 0e pitch!
		eye @ 0e roll!
		eye @ 0e y!

	then

	\ Attach the instrument panel to the camera
	eye @ panel @ Adopts drop

	current_view @ case

		view1stPerson of
			." *First person view" cr
		endof

		viewFront of
			." Frontal view" cr
			eye @ 0e yaw!
			eye @ view_distance f@ z!
			eye @ 0e x!
			eye @ view_distance f@ 3e f/ y!
			eye @ -10e pitch!
		endof

		viewBack of
			." Behind view" cr
			eye @ 180e yaw!
			eye @ view_distance f@ fneg z!
			eye @ 0e x!
			eye @ view_distance f@ 3e f/ y!
			eye @ 10e pitch!
		endof

      viewLeft of
         ." Left view" cr
         eye @ 90e yaw!
         eye @ 0e pitch!
         eye @ 0e z!
         eye @ view_distance f@ x!
         eye @ view_distance f@ 3e f/ y!
         eye @ 10e roll!
      endof

      viewRight of
         ." Right view" cr
         eye @ 270e yaw!
         eye @ 0e z!
         eye @ view_distance f@ fneg x!
         eye @ view_distance f@ 3e f/ y!
         eye @ -10e roll!
      endof

      viewNorth of
         ." North view" cr
         1  autoView !
         0e autoView_base_angle f!
      endof
      viewSouth of
         ." South view" cr
         1    autoView !
         180e autoView_base_angle f!
      endof
      viewEast of
         ." East view" cr
         1    autoView !
         270e autoView_base_angle f!
      endof
      viewWest of
         ." West view" cr
         1    autoView !
         90e  autoView_base_angle f!
      endof

   endcase
;


fvariable peddleFactor

variable old_view
fvariable old_view_distance

: step begin
   current_view @ old_view @ <> 
   view_distance f@ old_view_distance f@ f<> or if
      SetViewStuff
      current_view @ old_view !
      view_distance f@ old_view_distance f!
   then

   fGetTime localTimestamp f@ f- CalcPosition

   autoView @ if
      autoView_base_angle f@ self @ yaw@ f- fdup eye @ yaw! d>r 
      fsincos view_distance f@ f* eye @ z!
              view_distance f@ f* eye @ x!
   then

   current_button @ leftButton and if
      current_speed @ float 50e f/ 
      current_turnFactor @ float fabs -200e f/ f+ 
      stepAmpl f!
   else
      0e stepAmpl f!
   then

   \ If running REALLY fast, peddle faster, not longer!
   stepAmpl f@ totLeg f@ f/ fabs fdup 0.7e f> if
      0.7e f- 3e f* 2e f+ peddleFactor f!
      
      stepAmpl f@ f0> if
         totLeg f@  0.7e f* stepAmpl f!
      else
         totLeg f@ -0.7e f* stepAmpl f!
      then
   else
      fdrop 2e peddleFactor f!
   then

   totLeg f@ fdup f* stepAmpl f@ fdup f* f- fdup f0> if 
      fsqrt walkHeight f! 
   else
      fdrop 0.0e walkHeight f!
   then

   fGetTime peddleFactor f@ f* baseTime f@ f+ walkphase f!

   walkphase f@ linsin 2e f* 1e f- stepAmpl f@ fdup fsign f* f* stepX f!
   walkphase f@ 2e fmod pi f* fsin stepAmpl f@ f* liftFactor f@ f* 
   fdup 0e f> if 
      stepY f! 
      \ Calculate distance to foot
      walkHeight f@ stepY f@ f- fdup f*
      stepX f@ fdup f* f+  fsqrt dist2foot f!

      \ Calculate the G distance

      thLength f@ fdup f* dist2foot f@ fdup f* f- anLength f@ footHeight f@ f+ fdup f* f-
      -2e dist2foot f@ f* f/ Gdist f!

      \ Get the X distance from it:
      thLength f@ fdup f* dist2foot f@ Gdist f@ f- fdup f* f- fsqrt Xdist f!

      \ Calculate the angles...

      0e Xdist f@ Gdist f@ fatan2 pi f/ 180e f* rth HoldObj @ f- fdup pitch!
      Xdist f@ dist2foot f@ Gdist f@ f- fatan2 pi f/ 180e f* f- 0e fswap f- ran HoldObj @ pitch!
   else 
      fdrop 0e stepY f! 
      \ totLeg f@ fdup f* stepX f@ fdup f* f- fsqrt walkHeight f!
      totLeg f@ dist2foot f!
      0e rth HoldObj @ pitch!
      0e ran HoldObj @ pitch!
   then


   \ Add the base angle...
   0e stepX f@ walkHeight f@ stepY f@ f- fatan2 pi f/ 180e f* f- fdup rth HoldObj @ pitch+! lbc HoldObj @ pitch!


   walkphase f@ 1e f+ linsin 2e f* 1e f- stepAmpl f@ fdup fsign f* f* stepX f!
   walkphase f@ 1e f+ 2e fmod pi f* fsin stepAmpl f@ f* liftFactor f@ f* 

   fdup 0e f> if 
      stepY f! 
      \ Calculate distance to foot
      walkHeight f@ stepY f@ f- fdup f*
      stepX f@ fdup f* f+ fsqrt dist2foot f!

      \ Calculate the G distance

      thLength f@ fdup f* dist2foot f@ fdup f* f- anLength f@ footHeight f@ f+ fdup f* f-
      -2e dist2foot f@ f* f/ Gdist f!

      \ Get the X distance from it:
      thLength f@ fdup f* dist2foot f@ Gdist f@ f- fdup f* f- fsqrt Xdist f!

      \ Calculate the angles...

      0e Xdist f@ Gdist f@ fatan2 pi f/ 180e f* lth HoldObj @ f- fdup pitch!
      Xdist f@ dist2foot f@ Gdist f@ f- fatan2 pi f/ 180e f* f- 0e fswap f- lan HoldObj @ pitch!
   else 
      fdrop 0e stepY f! 
      \ totLeg f@ fdup f* stepX f@ fdup f* f- fsqrt walkHeight f!
      totLeg f@ dist2foot f!
      0e lth HoldObj @ pitch!
      0e lan HoldObj @ pitch!
   then

   \ Add the base angle...
   0e stepX f@ walkHeight f@ stepY f@ f- fatan2 pi f/ 180e f* f- fdup lth HoldObj @ pitch+! rbc HoldObj @ pitch!

   pelv HoldObj @ totLeg f@ y!

   stepAmpl f@ fabs -150e f* fdup rwr HoldObj @ pitch! lwr HoldObj @ pitch!

   pause 
again ;

\ The pstrfilename returned by this meme must be freed with free-mem.
: URL2File  ( pstrURLname -- pstrfilename.or.NULL )
    100 alloc-mem dup >r
    1+ swap 1+ swap
    OpenURLAsFile
    r@ 1+ c@ 0=
    if
        r> free-mem 0 exit
    then
    r@ 1+ strlen r@ c!
    r>
;

\ Used by FileOnHost
: append { stringdata len copyto ; pos -- }
   0 to pos

   copyto dup strlen + to copyto

   begin
      pos len <
   while
      stringdata pos + c@ copyto pos + c!
      pos 1 + to pos
   repeat
   0 copyto pos + c!
;

\ Returns the URL of a file (given in cstr format) on a given host.
\ The return is in pstr format (but zero terminated so buf+1 is a valid cstr.
\ The string should be freed with free-mem.

: FileOnHost { string hostID ; buf temp -- }
   100 alloc-mem to buf   
   buf if
      buf 1 + to buf
      \ Workaround: Skip the crap if it's localhost!
      localHost hostID = 0= if
         c" meme://" buf strcpy
         hostID 24 >> 255 and (.) buf append
         " ." buf append
         hostID 16 >> 255 and (.) buf append
         " ." buf append
         hostID 8 >> 255  and (.) buf append
         " ." buf append
         hostID 255 and (.) buf append
         " /" buf append
      else
         0 buf !
      then
      string string strlen buf append
      buf strlen buf 1 - dup to buf c!
      buf
   else
      0
   then
;

create line-buf 100 allot 
variable currObj
variable currMat
variable currTex

fvariable currX
fvariable currY
fvariable currZ
fvariable currXs
fvariable currYs
fvariable currZs
fvariable currRoll
fvariable currPitch
fvariable currYaw

: DoSettings { theObj -- }
   theObj if
      currMat @ ?dup if theObj ShapeMaterial then
      currX f@ theObj x! 
      currY f@ theObj y!
      currZ f@ theObj z!
      currXs f@ theObj xscale! 
      currYs f@ theObj yscale!
      currZs f@ theObj zscale!
      currRoll  f@ theObj roll! 
      currPitch f@ theObj pitch!
      currYaw   f@ theObj yaw!

\      0e currX f!
\      0e currY f!
\      0e currZ f!

\      1e currXs f!
\      1e currYs f!
\      1e currZs f!

\      0e currRoll  f!
\      0e currPitch f!
\      0e currYaw   f!

   then
;

: IsToken { teststr ; temp foo -- }
   line-buf 1 + to temp

   teststr to foo

   begin
      temp c@
      temp c@ teststr c@ = and
   while
      temp 1+    to temp
      teststr 1+ to teststr
   repeat 

   teststr c@ 0=
;

: NextToken { in spaces -- }
   begin
      in c@ 32 = if
         spaces 1- to spaces
         spaces 0= if
            in exit
         then
      then      
      1
   while
      in 1+ to in
   repeat      
   0
;

\ Input three floats in a string, output the floats 
: ThreeFloats { str -- }   
   str atof
   str 1 NextToken atof
   str 2 NextToken atof  
;

: RemoveCR     \ addr --
   begin dup c@ dup 0= not swap 13 = not and while 1+ repeat 0 swap c!
;

: LoadAvatarFile { hostID ; name fd syntaxOK temp -- }
   c" myavatar.cfg" hostID FileOnHost to name

   pelv HoldObj @ currObj !

d\   ." File is: " name ". cr

   0 currMat !
   0 currTex !
   0 HeadTex !

   0e currX f!
   0e currY f!
   0e currZ f!

   0e currRoll  f!
   0e currPitch f!
   0e currYaw   f!

   1e currXs f!
   1e currYs f!
   1e currZs f!

   \ If the URL resolution worked, then use that.
   name URL2File ?dup if
      name free-mem to name
   then

   name if 
      name read fopen to fd

      fd if
        1 if 
d\			." Sucessfully opened avatar CFG file " name ". cr 
		then

   begin
      line-buf fd fgetline swap drop
        while
           line-buf 1+ RemoveCR

           \ pause

           \ line-buf 1+ ctype cr

           0 to syntaxOK

           line-buf 1+ c@ ascii # = if 1 to syntaxOK then
           line-buf 1+ c@ 0 = if 1 to syntaxOK then

           \ All the different body part thingies....
           c" [Settings]" IsToken if
              1 to syntaxOK
           then
           c" [Pelv]" IsToken if
              1 to syntaxOK
              pelv HoldObj @ currObj !
           then
           c" [Body]" IsToken if
              1 to syntaxOK
              body HoldObj @ currObj !
           then
           c" [Neck]" IsToken if
              1 to syntaxOK
              neck HoldObj @ currObj !
           then
           c" [Head]" IsToken if
              1 to syntaxOK
              head HoldObj @ currObj !
           then
           c" [LUArm]" IsToken if
              1 to syntaxOK
              lbc HoldObj @ currObj !
           then
           c" [RUArm]" IsToken if
              1 to syntaxOK
              rbc HoldObj @ currObj !
           then
           c" [LLArm]" IsToken if
              1 to syntaxOK
              lwr HoldObj @ currObj !
           then
           c" [RLArm]" IsToken if
              1 to syntaxOK
              rwr HoldObj @ currObj !
           then
           c" [LULeg]" IsToken if
              1 to syntaxOK
              lth HoldObj @ currObj !
           then
           c" [RULeg]" IsToken if
              1 to syntaxOK
              rth HoldObj @ currObj !
           then
           c" [LLLeg]" IsToken if
              1 to syntaxOK
              lan HoldObj @ currObj !
           then
           c" [RLLeg]" IsToken if
              1 to syntaxOK
              ran HoldObj @ currObj !
           then
           c" [LFoot]" IsToken if
              1 to syntaxOK
              lft HoldObj @ currObj !
           then
           c" [RFoot]" IsToken if
              1 to syntaxOK
              rft HoldObj @ currObj !
           then

           c" LegSep:" IsToken if
              1 to syntaxOK
              line-buf 9 + atof legSeparation f!
              SetSkelPositions
           then

           c" FootHgt:" IsToken if
              1 to syntaxOK
              line-buf 10 + atof footHeight f!
              SetSkelPositions
           then

           c" ULegLength:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof thLength f!
              SetSkelPositions
           then

           c" LLegLength:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof anLength f!
              SetSkelPositions
           then

           c" ArmSep:" IsToken if
              1 to syntaxOK
              line-buf 9 + atof armSeparation f!
              SetSkelPositions
           then

           c" PelvHgt:" IsToken if
              1 to syntaxOK
              line-buf 10 + atof pelvHeight f!
              SetSkelPositions
           then

           c" ArmHeight:" IsToken if
              1 to syntaxOK
              line-buf 12 + atof armHeight f!
              SetSkelPositions
           then

           c" BodyHeight:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof bodyHeight f!
              SetSkelPositions
           then

           c" NeckHeight:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof neckHeight f!
              SetSkelPositions
           then

           c" UArmLength:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof uarmLength f!
              SetSkelPositions
           then

           c" LArmLength:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof larmLength f!
              SetSkelPositions
           then

           c" LiftFactor:" IsToken if
              1 to syntaxOK
              line-buf 13 + atof liftFactor f!
              SetSkelPositions
           then

           c" Default!" IsToken if
              1 to syntaxOK
              0 currTex !
              currMat @ ?dup if DestroyMaterial 0 currMat ! then

              0 currMat !
              0 currTex !

              0e currX f!
              0e currY f!
              0e currZ f!

              0e currRoll  f!
              0e currPitch f!
              0e currYaw   f!

              1e currXs f!
              1e currYs f!
              1e currZs f!
           then

           c" Pos:" IsToken if
              1 to syntaxOK
              line-buf 6 + ThreeFloats currZ f! currY f! currX f!
           then              

           c" Sca:" IsToken if
              1 to syntaxOK
              line-buf 6 + ThreeFloats currZs f! currYs f! currXs f!
           then              

           c" Rot:" IsToken if
              1 to syntaxOK
              line-buf 6 + ThreeFloats currYaw f! currPitch f! currRoll f!
           then              

           c" Col:" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              line-buf 6 + ThreeFloats currMat @ MaterialColor
           then              

           c" Mat:" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              line-buf 6 + ThreeFloats 0e currMat @ MaterialSurface
           then              

           c" Smooth!" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              smoothShaded currMat @ MaterialShading
           then              

           c" Flat!" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              flatShaded currMat @ MaterialShading
           then              

           c" FlatWrap!" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              flatWrap currMat @ MaterialWrap
           then              

           c" SphrWrap!" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              sphericalWrap currMat @ MaterialWrap
           then              

           c" CylnWrap!" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              cylindricalWrap currMat @ MaterialWrap
           then              

           c" StdRwx:" IsToken if
              1 to syntaxOK
              currObj @ lockedLink plainVisible line-buf 9 + NewVisible DoSettings
           then              

           c" OwnRwx:" IsToken if
              1 to syntaxOK
              line-buf 9 + hostID FileOnHost to temp

              temp if 
                 currObj @ lockedLink plainVisible temp 1+ NewVisible DoSettings
                 temp free-mem
              then              
           then              

           c" StdMm:" IsToken if
              1 to syntaxOK
              currObj @ lockedLink line-buf 9 + NewCompound DoSettings
           then              

           c" OwnMm:" IsToken if
              1 to syntaxOK
              line-buf 9 + hostID FileOnHost to temp

              temp if 
                 currObj @ lockedLink temp 1+ NewCompound DoSettings
                 temp free-mem
              then              
           then              

           c" StdTex:" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              0 currTex ! 
              line-buf 9 + NewTexture currTex !
              currTex @ ?dup if currMat @ MaterialTexture then
           then              

           c" OwnTex:" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              0 currTex !

              line-buf 9 + hostID FileOnHost to temp

              temp if 
                 temp 1+ NewTexture currTex !
                 currTex @ ?dup if currMat @ MaterialTexture then
                 
                 temp free-mem
              then              
           then              

           c" StdFace:" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then
              line-buf 10 + NewTexture HeadTex !
              HeadTex @ ?dup if currMat @ MaterialTexture then
           then              

           c" OwnFace:" IsToken if
              1 to syntaxOK
              currMat @ 0=   if NewMaterial currMat ! then

              line-buf 10 + hostID FileOnHost to temp

              temp if 
                 temp 1+ NewTexture HeadTex !
                 HeadTex @ ?dup if currMat @ MaterialTexture then
                 
                 temp free-mem
              then              
           then              


           c" NoTex!" IsToken if
              1 to syntaxOK
              0 currTex !
              currMat @ if 
                 0 currMat @ MaterialTexture 
              then
           then              

           c" NoMat!" IsToken if
              1 to syntaxOK
              currMat @ ?dup if DestroyMaterial 0 currMat ! then
           then              


           c" Name:" IsToken if
              1 to syntaxOK
              line-buf 7 + avatarName strcpy
           then              

           syntaxOK 0= if
              ." Avatar CFG file syntax error in line:" cr
              line-buf ". cr
           then
        repeat

        fd fclose
      else
         ." Error opening avatar CFG file " name ". cr
      then
      name free-mem
   then
;

\ Put this in a separate task so it happens asynchronously!

: appear
	begin

		masterIP @ if
			masterIP @ LoadAvatarFile
		else
			localHost  LoadAvatarFile
		then

		SetViewStuff

		atask @ sleep \ Go to sleep
		pause

	again
;

: go
	if
		localAvatar !	\ Store the localAvatar variable. Note:
						\ Since this *IS* an avatar, it is the eye
						\ object for the Avatar _MASTER_
		masterMH !		\ Store the master's module handle
		masterIP !		\ Store the master's IP

		fGetTime baseTime f!
		moduleOrigin WhichObject self !

d\		cr ." MyHost   " localHost hex u. decimal  ." MyModule " moduleOrigin . 
d\		cr ." MstrHost " masterIP @ hex u. decimal ." MstrMdle " masterMH ? 
d\		cr

		\ Default values...

		0.1e  footHeight f!
		0.4e  thLength f!
		0.4e  anLength f!
		0.15e legSeparation f!

		0.3e  armSeparation f!
		0.1e  pelvHeight f!
		0.4e  armHeight f!
		0.45e bodyHeight f!
		0.05e neckHeight f!

		0.3e  larmLength f!
		0.3e  uarmLength f!

		0.6e  stepAmpl f!
		0.3e  liftFactor f!

		\ Create the skeleton, on which to put yourself...
		CreateSkeleton

		\ Show it in default positions for now...
		SetSkelPositions

		\ If you're a master...
		masterIP @ 0= if 
			\ Add yourself to the IP list
			localHost moduleOrigin AddIPList
		then           

		head HoldObj @ kopf !
		neck HoldObj @ halz !

		headCam HoldObj @ kopfCam !
		neckCam HoldObj @ halzCam !


		\ If the master IP is zero, we are the master
		\ and if so, the localAvatar (which would be US!)
		\ is instead the eye object,
		\ which we then take charge of!

		masterIP @ 0= localAvatar @ and if
			localAvatar @ eye !

			\ Create the instrument panel as child to the camera
			0				\ Master HostID
			0				\ Master ModuleID
			ModuleWorld		\ localAvatar
			eye @ lockedLink c" panel/panel.mm" NewCompound
			panel !

			\ Now create the puppetPin object as child to the camera
			eye @ lockedLink NewImaginary puppetPin !

			\ Scale it down, rotate it, move it...
			0.05e puppetPin @ scale
			0e  puppetPin @ yaw!
			-0.1e  puppetPin @ z!

			3.5e  view_distance f!

			\ Set the stuff....
			view1stPerson current_view !         
			SetViewStuff
		else
			panel off
			puppetPin off
		then     

		\ Wear the default face
		0 dup current_face ! HeadTex @ SetTextureFrame

		0e radPerSec f!
		current_speed off
		current_turnFactor off
		current_button off
		cursorpos off

		fGetTime bandWidthTime f!
		bandWidthBytes off

		['] appear NewTask ?dup if dup atask ! wake then
		['] step   NewTask ?dup if dup stask ! wake then

      \ If we are a master then:
      masterIP @ 0= if
         ['] mouse NewTask ?dup if dup mtask ! wake then
      else \ If we are a slave, then:
         masterIP @ myGENMsg mhDestHostID !
         masterMH @ myGENMsg mhDestModuleID !
         moduleOrigin myGENMsg mhSrcModuleID !
         localHost    myGENMsg mhSrcHostID !
         msgMyState?  myGENMsg mhMsgBody !

			\ FIX: This piece of code is not present in the sphere.
			\ The sphere never sets masterAlive to "on".
			myGENMsg SendMessage 0= if 
				masterAlive on 
			then

         ['] ping NewTask ?dup if dup mtask ! wake then
      then
   else
      stask    @ ?dup if DestroyTask then
      mtask    @ ?dup if DestroyTask then
      atask    @ ?dup if DestroyTask then

      DeleteSkeleton

      \ If we had the eye, orphan it!

      eye @ ?dup if Orphan drop then

      \ If there is a master, inform him this slave has died.
      masterIP @ if 
d\	." Slave death" cr
         \ If the master is alive, send this message
         masterAlive @ if
d\	." Slave informs master: Death!" cr           
            msgSlaveLeave  myGENMsg mhMsgBody !
            masterIP @     myGENMsg mhDestHostID !
            masterMH @     myGENMsg mhDestModuleID !
            moduleOrigin   myGENMsg mhSrcModuleID !
            localHost      myGENMsg mhSrcHostID !
            myGENMsg       SendMessage drop \ Let the master know we died
         then
d\	." End slave death" cr
      else \ Else inform all slaves that the MASTER has died!
d\	." Master death!" cr

         \ Tell all slaves to die!
         KillAllSlaves

d\	." End master death" cr
      then
   then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr ScribbleModule
' foo ModuleAddr p" mtavatar.mm" UnloadModuleToFile
forget foo
