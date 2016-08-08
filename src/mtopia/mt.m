\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Metatopia load file.
\ by Marc de Groot
\ Copyright (c) 1996-1998 by Immersive Systems, Inc.  All rights reserved.
\
\ Modification log:
\ 17Feb96 MdG	2.66b	File created.

: task ;			\ Forgettable marker

\ *****************************************************************************
\ Conditional compilation for debugging vs. production versions

0 constant debugVersion?	\ Non-zero if debugging version

\ Conditional compilation for debugging version
\ Code to end-of-line after d\ is ignored if not debugging
: d\	( -- ) debugVersion? 0= if [compile] \ then ; immediate

\ Conditional compilation for production version
\ Code to end-of-line after p\ is ignored if not production
: p\	( -- ) debugVersion? if [compile] \ then ; immediate

\ *****************************************************************************
\ Compile the modules for Metatopia

.( Spaces: ) cr
cd spaces
fload mtstreet.m
fload mtisihq.m

.( Objects: ) cr
cd ../objects
fload mtsphere.m
\ fload mtgneric.m

.( Instrument panel: ) cr
cd ../panel
fload panel.m
fload roottool.m

cd ..

.( The avatar: ) cr
fload mtavatar.m

.( The deck: ) cr
fload mtdeck.m

\ *****************************************************************************

forget task			\ Take only snapshots, leave only side effects
