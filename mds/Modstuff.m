\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ modstuff - These definitions are part of the module code, but are
\ the only definitions needed in the modcode.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.
\
\ *****************************************************************************
\ NewCompound looks up its counterpart in the main dictionary and runs
\ it.  This is the definition for a module; the main dict's definition is in
\ module.m.
mod\ : NewCompound
mod\ 	p" main" find 0= abort" Can't find xaddr of main"
mod\ 	p" NewCompound" 
mod\ 	swap
mod\ 	vfind ?execute
mod\ ;
\ *****************************************************************************
\ *** This code doesn't work if it isn't compiled into the module. See below
\ where I define ModuleWorld in terms of moduleOrigin and MyModuleObject.
\
\ ModuleWorld is used instead of world as the parent object of objects
\ in modules.
\ : ModuleWorld	( -- objaddr )
\ 	(lit) [ 0 , current @ ( >body ) here /n - reloc! ]
\ 	WhichObject
\ 	?dup 0= abort" ModuleWorld: Couldn't find the module's object address."
\ ;
\ *****************************************************************************
\ This is a redefinition of DestroyObject that knows how to deallocate
\ compound objects.

\ FIX: This needs to be changed.  DestroyObject will be too slow like this.
\ The best thing is to make UnloadModule, etc. into primitives.

\ Don't define DestroyObject like this!  It will find itself when run from
\ a background task.
\ : DestroyObject ( obj -- )
\	p" DestroyObject" find ?execute
\ ;

: DestroyObject	( obj -- )
	dup objecttype @ compoundObject =
		if								\ -- obj
		dup ptr @ dup 					\ -- obj ptr ptr
main\	UnloadModule DestroyModule
mod\	p" UnloadModule" find ?execute	\ -- obj ptr

\ free-mem doesn't work here cuz it doesn't deallocate the module struct.
\ mod\	free-mem						\ -- obj
mod\	DestroyModule

		then
	(DestroyObject)						\ --
;
\ *****************************************************************************
\ moduleOrigin allows access to the data structure at the start of the
\ module.

\ Neither of these works if the modcode isn't compiled into the module with the
\ running code!
\ mod\ ThisModule      constant moduleOrigin	here /n - set-relocation-bit drop
\ mod\ current @ ( >body ) constant moduleOrigin	here /n - set-relocation-bit drop

\ THIS works either way
mod\ : moduleOrigin
mod\	module-compiling? @
mod\	if
mod\		current @ ( >body )		\ Get the module's base addr
mod\		state @
mod\		if					\ if compiling
mod\			compile (lit)		\ compile reference to (lit)	
mod\			reloc,				\ compile the value & set its reloc flag
mod\		then				\ if executing, just leave it on the stack
mod\	else
mod\		." Warning: moduleOrigin called, not module compilation. " where
mod\	then
mod\ ; immediate

mod\ : MyModuleObject ( -- addr )
mod\ 	[compile] moduleOrigin compile WhichObject
mod\ ; immediate

\ Like WhichObject, but has a reference to abort and is willing to use it.
mod\ : _ModuleWorld
mod\ 	WhichObject
mod\ 	?dup 0= abort" ModuleWorld: Couldn't find the module's object address."
mod\ ;

\ Like WhichObject but returns world instead of 0
mod\ : __ModuleWorld
mod\	WhichObject ?dup 0= if
mod\		world
mod\	then
mod\ ;

mod\ : ModuleWorld
mod\ 	[compile] moduleOrigin compile __ModuleWorld
mod\ ; immediate

\ *****************************************************************************
\ Utility memes
: ObjVoc	( objaddr -- )	\ Set context to compound object's module voc
	dup objecttype @ compoundObject = if
		ptr @ context !
	else
		drop
	then
;

: cload	\ Load the file whose name follows in the input stream
	>in @ >r
	world lockedLink bl word count drop NewCompound ?dup if
		r> >in ! constant
	else
		r> drop abort" cload failed "
	then
;
