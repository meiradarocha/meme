\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ This file loads the module header code.  It saves the module
\ header into modcode.mod.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

34000 Module: foo
foo definitions
BeginModule

: mod\ ; immediate
: main\ [compile] \ ; immediate

.( bury.m ) fload bury.m
ModuleUsed
.( uservars.m ) fload uservars.m
ModuleUsed
.( util.fth ) fload util.fth
ModuleUsed
.( floatops.fth ) fload floatops.fth
ModuleUsed
.( locals.fth ) fload locals.fth		\ Local variable support -MdG
ModuleUsed
.( tasking.m ) fload tasking.m 
ModuleUsed
.( errors.mh ) fload errors.mh
ModuleUsed
.( struct.m ) fload struct.m 
ModuleUsed
.( anomalo.m ) fload anomalo.m
ModuleUsed
.( memeutil.m ) fload memeutil.m
ModuleUsed
.( mstructs.mh ) fload mstructs.mh
ModuleUsed
.( modstuff.m ) fload modstuff.m
ModuleUsed
.( messages.mh ) fload messages.mh
ModuleUsed
.( net.m ) fload net.m
ModuleUsed
EndModule
main definitions
' foo ModuleAddr p" modcode.mod" UnloadModuleToFile
forget foo
cr .( Stack: ) .s cr
