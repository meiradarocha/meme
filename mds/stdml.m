\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Source code to build the standard library (formerly modcode.mod) in the dictionary.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

cr cr .( Starting compilation of standard library. ) cr
\ **********
here								\ Point dp here later.
2000 allot							\ Put the definitions up high, out of our way
Internal							\ Get ready to make these definitions disappear
swap								\ Put the old here on top.
\ **********

50000	constant	smRelTblOff		\ Fake relocation table offset for the module header

\ This meme allocates the space for the threads.  Even though Meme can handle
\ conditionals outside a colon definition, I want to put this here.  (Could be
\ the conditionals compile temporarily at HERE anyway.)
: threads,	( -- )		#threads 0 do origin reloc, loop ;

\ These variables hold an addresses that gets patched with addrs of fields in the module.
variable patch-addr					\ Address to patch with address of module vocab
variable patch-addr2				\ Address to patch with origin of module

\ **********
dp !								\ Move the dp back down where it was
External							\ Following definitions can see the ones in high mem
\ **********

\ This meme sets the context vocabulary to be our pseudo-vocabulary
: std.mlContext!	( -- )			0 [ here /n -  patch-addr ! ] context ! ;

\ This meme returns the base address of the std library module
: std.mlOrigin		( -- addr )		0 [ here /n - patch-addr2 ! ]			;

\ This meme returns the number of bytes of dictionary in the std lib
: std.mlSize		( -- #bytes )	\ Return the number of bytes in the library
	std.mlOrigin
	dup ModuleDP @
	swap ModuleDict -
;

\ Create the dictionary entry.  Allocate a struct NamedModules, and then a struct Modules, after it.

create std.ml
	0 		,					\ MLink field
	0		reloc,				\ MAddr field
	0		,					\ MSize field

here patch-addr @ reloc!		\ Patch the spot in std.ml-context! with the module's voc address

here std.ml MAddr reloc!		\ This is the base addr of the module.  Store it in the std.ml struct.

here patch-addr2 @ reloc!		\ Patch the spot in std.ml-origin with the module's origin.

	ModuleMagicNumber ,			\ pseudo CFA for a vocabulary.  This is where CONTEXT will point.

	threads,					\ Allocate the vocabulary threads.
	0			,				\ Allocate the fake voc-link field. (should stay zero, not relocated)
	0			,				\ Allocate ModuleMaxFileSize
	0			,				\ Allocate ModuleDP (will be patched and made relocatable below)
	0			,				\ Allocate 1stTaskPtr (not relocated if zero)
	0			,				\ Allocate LastTaskLinkPtr (not relocated if zero)
	smRelTblOff	,				\ Allocate RelTableOffset (an offset, not relocated)
	-1			,				\ Allocate ModuleLoaded (a flag, not relocated)
	0			,				\ Allocate CFAOfGo (should only be relocated if non-zero)
	0			,				\ Allocate CFAOfReceiver (should only be relocated if non-zero)
	std.mlOrigin reloc,			\ Allocate CompilationOrigin (relocatable)
	std.mlOrigin reloc,			\ Allocate CompilationLibOrigin (relocatable, though meaningless)
	0			,				\ Allocate ModuleCreator (not relocatable if zero)
	0			,				\ Allocate ModuleTagPointer (not relocatable if zero)
	0			,				\ Allocate ModuleAuxPointer (not relocatable if zero)

\ This is the start of the module's dictionary

here std.ml MAddr @ ModuleDP reloc!		\ Set ModuleDP to point here

								\ context points at the CFA of a vocabulary.  CFA /n + is the thread
								\ pointer.  It points at the link field of the topmost meme in the
								\ vocabulary.  The link field of the bottommost meme in the vocabulary
								\ points at the origin, which contains a zero.  When a vocabulary is newly
								\ created, its thread pointer points at origin--which becomes the first
								\ meme's link field.

only main also					\ Make sure we can see the main vocabulary
std.mlContext!					\ Set context to point to our pseudo vocabulary

\ **********
Bury							\ Cut those high-memory definitions loose; we don't need 'em anymore
\ **********

definitions						\ Get ready to compile code in the new vocabulary

here							\ Clear the relocation table where the high mem definitions were
dup 3000 + maxdp min
swap >relbit drop
swap >relbit drop
over - 1+ erase


\ FIX: rename mod\ to stdlib\
: mod\						;	immediate		\ Does nothing
: main\		[compile] \		;	immediate		\ Acts like a comment

.( bury.m ) fload bury.m
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( uservars.m ) fload uservars.m
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr
 
.( util.fth ) fload util.fth
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( floatops.fth ) fload floatops.fth
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( locals.fth ) fload locals.fth		\ Local variable support -MdG
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( tasking.m ) fload tasking.m 
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( errors.mh ) fload errors.mh
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( struct.m ) fload struct.m 
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( anomalo.m ) fload anomalo.m
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( memeutil.m ) fload memeutil.m
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( mstructs.mh ) fload mstructs.mh
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( modstuff.m ) fload modstuff.m
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( messages.mh ) fload messages.mh
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

.( net.m ) fload net.m
here std.ml MAddr @ ModuleDP ! .( -- ) std.mlSize . .( bytes in std library. ) cr

only main also definitions

.( Stack: ) .s 
cr .( Done with standard library compilation )
cr

\ *************** NOTE!  The code in forth.c that checks whether a value is inside
\ the standard library depends on the above code to set the ModuleDP field of the
\ std library pseudo module.
