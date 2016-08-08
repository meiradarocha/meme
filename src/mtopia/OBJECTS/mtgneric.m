\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ The Generic Object
\ by Zap Anderson
\ Copyright (c) 1995, 1996 by Immersive Systems, Inc.  All rights reserved.
\
\ 17Feb96 MdG	2.66b	MdG takes over the code.

.( Object module: mtsphere.m)

10000 Module: foo

only main also
std.mlContext! also
foo also definitions
BeginModule

variable masterIP    \ The IP of the master
variable masterMH    \ The Module Handle of the master
variable masterAlive \ Is the master alive?
variable localAvatar \ The Local Avatar object (for collision detection e.t.c.)

fload ../mtmsgs.mh

\ A general message structure for messages with only an integer body
create myGENMsg SizeOf MsgHeaders 4 + dup allot
myGENMsg mhMsgLen !

\ A message for the create-object call
create myCOBMsg SizeOf MsgHeaders SizeOf COBMsg + 20 + dup allot
myCOBMsg mhMsgLen !

fvariable lastMessageTime
variable  pingCount
variable  pingtask
variable  msg

: recv
   msg ! 

d\      ." AnObj:" moduleOrigin . msg @ mhMsgBody @ . cr

   fGetTime lastMessageTime f!
   0        pingCount !
   
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
         msg @ mhSrcHostID DeleteIPListItem
      endof

      msgMasterDied of   \ The master died
         0 masterAlive !  
         msgKillMe    myGENMsg mhMsgBody !
         moduleOrigin myGENMsg mhSrcModuleID !
         localHost    myGENMsg mhSrcHostID !
         \ Send to local deck
         0            myGENMsg mhDestModuleID !
         localHost    myGENMsg mhDestHostID !
         myGENMsg     SendFinalMessage \ Send the KILL ME
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
         \     c" mtavatar.mm" myCOBMsg mhMsgBody COBslaveURL strcpy
         \ (MAKE SURE THE MESSAGE IS LARGE ENOUGH)

         \ Sanity check: An object will refuse to replicate to it's masters machine.

         myCOBMsg mhMsgBody COBmasterHostID @ msg @ mhMsgBody RPTHostID @ cr = 0= if
       myCOBMsg SendMessage drop
         then
      endof

      \ TO BE FILLED IN BY THE OBJECT:
      \ ANY OBJECT SPECIFIC MESSAGES GOES HERE

   endcase

; TheReceiverMemeIs recv


\ If we are a slave, then send ping's at regular intervals

: ping begin
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
        msgKillMe    myGENMsg mhMsgBody !
        moduleOrigin myGENMsg mhSrcModuleID !
        localHost    myGENMsg mhSrcHostID !
        \ Send to local deck
        0            myGENMsg mhDestModuleID !
        localHost    myGENMsg mhDestHostID !
        myGENMsg     SendFinalMessage \ Send the KILL ME
     then
  then

  pause 
again ;



: go
   if
                localAvatar ! \ Store the local avatar object
      masterMH !    \ Store the masters module handle
      masterIP !    \ Store the masters IP (0 if we are the master)





      \ TO BE FILLED IN BY THE OBJECT:
      \ STANDARD CONSTRUCTOR STUFF






      \ If we are a master then:
      masterIP @ 0= if
         localHost moduleOrigin AddIPList

         \ TO BE FILLED IN BY THE OBBJECT
         \ LAUNCH TASKS ONLY FOR MASTERS (brains)

      else \ If we are a slave, then:
         masterIP @   myGENMsg mhDestHostID !
         masterMH @   myGENMsg mhDestModuleID !
         moduleOrigin myGENMsg mhSrcModuleID !
                   localHost    myGENMsg mhSrcHostID !
                   msgMyState?  myGENMsg mhMsgBody !

         \ Ask the master our state!
         myGENMsg SendMessage drop 

         \ Launch the ping task
         ['] ping NewTask ?dup if dup pingtask ! wake then
      then
   else


      \ TO BE FILLED IN BY OBJECT:
      \ NORMAL DESTRUCTOR STUFF



      \ If there is a master, inform him this slave has died.
      masterIP @ if 
         \ And kill the pingask
         pingtask @ ?dup if DestroyTask then
         if masterAlive @ if
            msgSlaveLeave  myGENMsg mhMsgBody !
            masterIP @     myGENMsg mhDestHostID !
            masterMH @     myGENMsg mhDestModuleID !
            moduleOrigin   myGENMsg mhSrcModuleID !
            localHost      myGENMsg mhSrcHostID !
            myGENMsg       SendMessage drop \ Let the master know we died
         then
      else \ Else inform all slaves that the MASTER has died!
         msgMasterDied  myGENMsg mhMsgBody !
         moduleOrigin   myGENMsg mhSrcModuleID !
         localHost      myGENMsg mhSrcHostID !
         myGENMsg       MessageToAllButSelf  \ Tell everybody the master died
      then
   then
; TheGoMemeIs go

ModuleUsed
EndModule
only main also definitions

' foo ModuleAddr p" mtgneric.mm" UnloadModuleToFile
forget foo


