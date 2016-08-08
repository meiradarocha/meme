\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Relocatable and transmissible code modules.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

\ How to use a module:
\ 5000 Module: xyz		\ Make module with 5000 bytes of dictionary
\ xyz definitions		\ Set context and current
\ BeginModule			\ Point DP into the module's dictionary
\ ... definitions ...		\ These memes compile into the module
\ EndModule			\ Point DP back into the main dictionary
\ main definitions		\ Set context and current back 

\ ***************************************************************************
\ URL utility memes

\ The pstrfilename returned by this meme must be freed with free-mem.
: URL2File	( pstrURLname -- pstrfilename.or.NULL )
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

: UOpen	( pstrURLname -- fp.or.NULL )
	URL2File dup 0= if exit then
	dup 0 fopen			\ pstrfilename -- pstrfilename fd
	swap free-mem		\ pstrfilename fd -- fd
;
	
\ ***************************************************************************
\ Module utility memes

base @ hex
: OtherEndian	\ n1 -- n2   Byte-reverse the 32-bit number
	dup >r 18 << 
	r@ ff00 and 8 << +
	r@ ff0000 and 8 >> +
	r> ff000000 and 18 >> +
;
base !

\ Return true if the input arg is a bad magic number
: BadMagic?	\ magic# -- bool
	ModuleMagicNumber over = if drop 0 exit then
	ModuleMagicNumber OtherEndian <>
;
	
\ Given a module address, return the address of the
\ module's relocation table.
: RelTable	( module.addr -- reloc.table.addr )
	dup RelTableOffset @ +
;

\ Returns the size, in bytes, of the Module structure and dictionary
: #ModuleBytes	( size -- #bytes )
	[SizeOf] Modules +		\ Add size of the Modules struct
	aligned				\ Round up to next cell
;

\ Returns the size, in bytes, of the relocation table
: #RelocationBytes ( size -- #bytes )
	#ModuleBytes			\ Find size of module and dict
	/n / ;				\ Divide by cell size

\ Return the address of the current module
: ThisModule ( -- module.adr )
	current @ ( >body )	\ MAGIC commented out
;

\ Given the tick addr of the module, return its base addr.
: ModuleAddr ( tick.addr -- module.addr )
	>body MAddr @
	dup 0= abort" ModuleAddr: null module address. "
;

\ This meme has been moved to util.fth. -MdG
\ This meme is like token! but does not check whether a primitive
\ is stored at addr.  The relocation bit is set unconditionally.
\ : reloc! ( n addr -- )
\ 	set-relocation-bit !
\ ;

\ This meme replaces the (set-relocation-bit) called by token! and reloc!
\ It is patched into the deferred meme set-relocation-bit
\ by BeginModule
\ : (module-set-relocation-bit)  ( adr -- adr )
\ 	ThisModule >r				\ Module addr on rtn stk
\ 	dup					\ ( -- adr adr )
\ 	r@ 	 				\ ( -- adr adr module.addr )
\ 	r@ RelTable 1 -				\ ( -- adr adr lo.adr hi.adr )
\ 	between					\ ( -- adr flag )
\ 		if				\ ( -- adr )
\ 		dup r@ - /n /			\ ( -- adr reltablebyteoffset )
\ 		r@ RelTable +			\ ( -- adr reltableadr )
\ 		dup c@ 1 or swap c!		\ Set the bit in the table
\ 		then
\ 	r> drop ;				\ Clear return stack

\ This is a convenience meme that computes the relocation table
\ byte address from the address given.  It returns zero if the address
\ is outside this module.
: >relbyte ( adr -- reltbladr )
	dup				( -- adr adr )
	ThisModule dup RelTable 1 -	( -- adr lo.adr hi.adr )
	between
		if			\ If adr is inside the module
		ThisModule - /n /	( -- wordoffset )
		ThisModule RelTable +	( -- reltableadr )
		else
		drop 0			( -- 0 )
		then ;
		
\ Allocate a buffer for the module.  Return its address.
: AllocateModule	( size -- addr )
	aligned				\ Round up the supplied size
	dup >r
	dup #ModuleBytes 
	swap #RelocationBytes +		\ Add # relocation bytes to size
\	[SizeOf] LinkFieldPtrs -	\ Less the link field pointers
\	[SizeOf] VocLinkPtr -		\ Less the voc-link pointer
	dup				\ ( totalsize -- totalsize totalsize )
	NewModule 			\ ( totsize totsize -- totsize addr )
	dup 0= abort" Can't allocate a module buffer."
	dup 3 and abort" Allocated module buffer isn't longword-aligned. "
	dup rot 			\ ( totsize addr -- addr addr totsize )
	0 fill				\ ( addr addr totsize -- addr )
	r> swap >r			\ ( -- size ) module adr on rstk

	\ Write the pointers to the tops of the dictionary threads
	r@ dup ModuleDict		\ ( -- size module.org dict.org )
	swap				\ ( -- size dict.org module.org )
	ModuleMagicNumber	\ ( -- size dict.org module.org magic )
	over ModuleMagic !	\ ( -- size dict.org module.org )
	LinkFieldPtrs		\ ( -- size dict.org lfptrs.org )
	#threads 0
		do
		2dup !			\ ( -- size dict.org module.org )
		/n +			\ ( -- size dict.org module.org+4 )
		loop
\	swap drop			\ ( -- size module.org+4 )
	2drop				\ ( -- size )
	\ Write the vocabulary link
\	voc-link link@ over ! 		\ ( -- size module.org+4 )
\	voc-link link!			\ ( -- size )
	

	dup #ModuleBytes	\ ( size -- size struct+dict.size )
	over #RelocationBytes +	\ ( struct+dict.size -- struct+dict+rel.size )
	r@ ModuleMaxFileSize !	\ Set the total # of bytes in the module

	#ModuleBytes		\ ( size -- module+dict.size )
	r@ RelTableOffset !	\ Set rel table offset ( -- )
	0 r@ ModuleDict !	\ Put a zero in the first cell
	r@ ModuleDict /n + 
	r@ ModuleDP !		\ Set module's DP ( -- )
	r@ ModuleDict		\ ( -- module.origin )
	dup r@ 1stTaskPtr !		\ task ptrs default to point at the dict base
	    r@ LastTaskLinkPtr !
	0 r@ ModuleCreator !	\ Not relocatable if zero
	0 r@ ModuleTagPointer !	\ Not relocatable if zero
	0 r@ ModuleAuxPointer !	\ Not relocatable if zero


	\ Mark the vocabulary link field pointers as relocatable
	r@ RelTable #threads
	over + swap
	do
		3 i c!		\ 3 signifies full-cell byte-order sensitivity
					\ and primary relocatability
	loop

	\ Mark some of the fields in the module struct as relocatable.
	3 r@ RelTable [OffsetOf] ModuleDP /n / + c!
	3 r@ RelTable [OffsetOf] 1stTaskPtr /n / + c!		\ Don't relocate unless non-zero
	3 r@ RelTable [OffsetOf] LastTaskLinkPtr /n / + c!	\ Don't relocate unless non-zero

	r> 			\ Leave the base addr
;
	
\ Module defining meme.
: "Module:	( size name -- )
	swap AllocateModule			\ ( size name -- name addr )
	swap "create				\ ( name addr -- addr )
	here >r
	[SizeOf] NamedModules allot
	dup r@ MAddr !			\ ( addr -- addr ) Store addr in the dic

	\ Set the total size in the dictionary.  ModuleMaxFileSize has
	\ been initialized by AllocateModule.

	ModuleMaxFileSize @			\ ( addr -- totalsize )
	r@ MSize !					\ ( totalsize -- ) Store size in dic

	\ Link the module into the linked list

	mod-link @					\ Get mod-link
	r@ MLink mod-link !			\ New mod-link points at MLink field
	r> MLink !					\ MLink field points at last module

	does>						\ ( -- baseaddr )
	MAddr @						\ ( baseaddr -- module.addr )
\	body>						\ ( module.addr -- pseudo-acf )
	context reloc!				\ Set the context vocab ( -- )
;	

: Module:	bl word "Module: ;

\ This meme swaps the main dictionary DP with the module's local DP.
\ This makes the compiler compile code into a module.
: ModuleSwitchDP	( -- )
	here				\ ( -- current.dp )
	ThisModule ModuleDP	\ ( -- current.dp ModuleDPaddr )
	dup @ dp !			\ Set DP ( -- current.dp ModuleDPaddr )
	!					\ Save old DP in ModuleDP
;

\ ***************************************************************************
\ BeginModule and EndModule bracket code that is to be compiled
\ into the module.
: BeginModule	( -- )
	\ Swap the module's DP with the main dictionary's DP
	ModuleSwitchDP

	\ Make sure that tokstore() calls module_set_relocation_bit()
	module-compiling? on

	\ Mark the module as loaded
	ThisModule ModuleLoaded on

	\ Set the CompilationOrigin and CompilationLibOrigin fields.
	ThisModule dup CompilationOrigin reloc!	
	std.mlOrigin ThisModule CompilationLibOrigin reloc!
;

: EndModule ( -- )
	\ Swap the module's DP with the main dictionary's DP.
	ModuleSwitchDP

	\ Make sure that tokstore() calls main_set_relocation_bit().
	module-compiling? off
;
\ ***************************************************************************
\ The following memes unlink (and relink) the tasks inside the module from the
\ tasks in the rest of the dictionary.  NOTE: Tasks within a single module are 
\ expected to be in a contiguous block of task-list nodes.  Tasks must be
\ defined all at one time for a single module, without mixing definitions
\ of tasks in another module or in the main dictionary.  NOTE: These memes
\ rely on the meme ThisModule, which means that the module address has to
\ be stored in current for these memes to work.

\ 
\ This meme returns true if the supplied address is in the current module's
\ dictionary space.  It will not return TRUE if the addr is the module
\ origin, which always has a zero in it, and is not an accessible part
\ of the dictionary.
: InsideModuleDict? 			( addr -- boolean					)
	ThisModule ModuleDict /n +	( -- addr.of.1st.dict.byte			)
	ThisModule RelTable 1-  	( -- 1st.dict.addr last.dict.addr	)
	between						( -- bool							)
;

\ FIX: The main task's link only contains the latest created task if the
\ task was created by the main task.  If a module is compiled by a background
\ task THIS CODE FAILS.
\ This meme sets the 1stTaskPtr and LastTaskLinkPtr.
\ The main task's link contains the UP of the most recently defined task, 
\ so the 1stTaskPtr is set to the main task's link contents.  The task list
\ is then walked until the task pointer is outside the module.  The address
\ of the previous task pointer is the LastTaskLinkPtr.
\
\ Note that this meme depends on memes defined in tasking.m.
: SetModuleTaskPtrs ( -- )
	\ Set the 1stTaskPtr to the UP of the most recently defined task.
	maintask link local @
	dup InsideModuleDict?
	if		\ If a task has been defined inside the module
		dup ThisModule 1stTaskPtr reloc!
		\ Walk the task list looking for a reference
		\ that's outside the module
		begin
			link local
			dup @ InsideModuleDict?
		while
			@
		repeat
		ThisModule LastTaskLinkPtr reloc!
	else
		drop		\ Drop the task pointer
	then
;

\ This meme removes the tasks in the current module from the circular linked
\ list of tasks.  It MUST ONLY be executed after SetModuleTaskPtrs which sets
\ 1stTaskPtr and LastTaskLinkPtr.
: UnlinkModuleTasks ( -- )
	ThisModule LastTaskLinkPtr @
	ThisModule ModuleDict - if		\ If there are tasks in the module
		ThisModule 1stTaskPtr @
		dup link local				\ -- taskptr linkptr

		\ Loop looking for the task link that points at module's
		\ first task.
		begin
			2dup @ <>
		while
			@ link local
		repeat
		swap drop						\ Drop the taskptr
		ThisModule LastTaskLinkPtr @ @	\ Get ptr to task outside module
		swap !							\ Store ptr to outside task in the link that
										\ pointed to me.
	then
;

\ This meme links the module's tasks into linked list of tasks
: LinkModuleTasks ( -- )
	ThisModule 1stTaskPtr @ 
	ThisModule ModuleDict - if			\ If there are tasks in the module
		maintask link local @
		ThisModule 1stTaskPtr @ 
		maintask link local !
		ThisModule LastTaskLinkPtr @
		!
		multi							\ Make sure multitasking is turned on
	then
;
\ *********************************************************************
\ The following routines implement relocatable arrays.  Unrelocate
\ converts absolute addresses in the array to addresses that are
\ relative to the base of the array.  Relocate does the opposite,
\ converting array-relative addresses to absolute addresses.
\ The array consists of 32-bit cells.  The relocation table consists
\ of one-byte relocation flags.  Each byte contains a true or false
\ indicating whether the corresponding 32-bit cell is relocatable.

\ *********************
\ NOTE.  As of 2.56b the module code no longer performs an unrelocate.
\ Instead, the compilation origin is stored in the header and is used
\ at load time to relocate the code.
\ *********************

Internal
: newrel\						; immediate
: oldrel\		[compile] \		; immediate
External

\ Set all relocatable cells in the array to be relative to the
\ beginning of the array.
\ Reltable.addr is the addr of the relocation table. Array.addr is the
\ addr of the relocatable array.  #bytes is the number of bytes in the
\ relocatable array.
: Unrelocate ( reltable.addr array.addr #bytes -- )
oldrel\		>r					( -- reltable.addr array.addr )
oldrel\		dup negate -rot		( -- -array.addr reltable.addr array.addr )
oldrel\		r>					( -- -array.addr reltable.addr array.addr #bytes )
oldrel\		over + swap			( Set up args for "do" )
oldrel\		do					( -- -array.addr reltable.addr )
oldrel\			dup c@			( Fetch the relocation flag for this cell )
oldrel\			if				( -- -array.addr reltable.addr )
oldrel\				over		( -- -array.addr reltable.addr -array.addr )
oldrel\				i +!		( -- -array.addr reltable.addr )
oldrel\			then
oldrel\			1+				( Bump reltable addr )
oldrel\		/n +loop			( Bump addr by size of cell )
oldrel\		2drop				( Drop -array.addr and reltable.addr )
newrel\		drop drop drop
;
			
\ Set all relocatable cells in the array to be absolute addresses.
\ Reltable.addr is the addr of the relocation table. Array.addr is the
\ addr of the relocatable array.  #bytes is the number of bytes in the
\ relocatable array--NOT in the relocation table!
oldrel\	: Relocate ( reltable.addr array.addr #bytes -- )
oldrel\		>r				( -- reltable.addr array.addr )
oldrel\		dup -rot		( -- array.addr reltable.addr array.addr )
oldrel\		r>				( -- array.addr reltable.addr array.addr #bytes )
oldrel\		over + swap		( Set up args for "do" )
oldrel\		do				( -- array.addr reltable.addr )
oldrel\			dup c@		( Fetch the relocation flag for this cell )
oldrel\			if			( -- array.addr reltable.addr )
oldrel\				over	( -- array.addr reltable.addr -array.addr )
oldrel\				i +!	( -- array.addr reltable.addr )
oldrel\			then
oldrel\			1+			( Bump reltable addr )
oldrel\		/n +loop		( Bump addr by size of cell )
oldrel\		2drop			( Drop -array.addr and reltable.addr )
oldrel\	;

newrel\	: Relocate		{ table array oldorigin oldliborigin #bytes ; reloffset liboffset -- }
newrel\		array oldorigin - to reloffset
newrel\		std.mlOrigin oldliborigin - to liboffset
newrel\		begin
newrel\			table c@ 15 and				\ only look at lower nybble (for now)
newrel\			case
newrel\				0 of	\ 0 does nothing, but leave the empty case here, or
newrel\				endof	\ you'll get spurious err msgs from the default case
newrel\				3 of
newrel\					array @ reloffset + array !
newrel\				endof
newrel\				4 of
newrel\					array @ liboffset + array !
newrel\				endof
newrel\				dup of
newrel\					\ cr ." Addr " table u. ." in the relocation table is " table c@ u. 3 spaces
newrel\				endof
newrel\			endcase
newrel\			array /n + to array
newrel\			table 1+ to table
newrel\			#bytes /n - dup to #bytes 0=
newrel\		until
newrel\	;

newrel\	: Reflag	{ table array #bytes ; libbase libtop -- }
newrel\		std.mlOrigin dup to libbase						\ base of std library
newrel\		ModuleDP @ to libtop							\ top of std library
newrel\		array #bytes over + swap
newrel\		do												\ for each cell in the module
newrel\			table c@ 3 =								\ if it's marked relocatable
newrel\			if
newrel\				i @ dup libbase >= swap libtop < and	\ if it points to std lib
newrel\				if
newrel\					4 table c!							\ set relocation flag to 4
newrel\				then
newrel\			then
newrel\			table 1+ to table							\ bump relocation table ptr
newrel\		/n +loop										\ loop count by sizeof cell
newrel\	;
\ *********
\ Unrelocation is no longer done.  Instead, all relocatable words in the module
\ dictionary are examined to see whether they point into the std library, and if so,
\ their relocation flag is changed from 3 to 4.
\ ********
		
\ Change all relocatable cells in the module to be relative to the
\ module dictionary's start.
oldrel\ : UnrelocateModule 		( module.addr -- )
oldrel\	dup >r				( -- module.addr )
oldrel\	RelTable			( -- reltable.addr )
oldrel\	r@					( -- reltable.addr array.addr )
oldrel\	r> RelTableOffset @	( -- reltable.addr array.addr #bytes )
oldrel\	Unrelocate			( -- )
oldrel\ ;

\ Change all cells that point at the standard library to have a different relocation
\ flag.  Note that #bytes is the # of bytes in the array, not in the relocation table.
newrel\ : ReflagModule 		( module.addr -- )
newrel\	dup >r				( -- module.addr )
newrel\	RelTable			( -- reltable.addr )
newrel\	r@					( -- reltable.addr array.addr )
newrel\	r> RelTableOffset @	( -- reltable.addr array.addr #bytes )
newrel\	Reflag				( -- )
newrel\ ;

\ Change all relocatable cells in the module to be absolute addresses.
: RelocateModule 				( module.addr -- )
	dup >r						( -- module.addr )
	RelTable					( -- reltable.addr )
	r@							( -- reltable.addr array.addr )
	r@ CompilationOrigin @		( -- reltable.addr array.addr compile.addr )
	r@ CompilationLibOrigin @	( -- reltable.addr array.addr compile.addr oldlib.addr )
	r> RelTableOffset @			( -- reltable.addr array.addr compile.addr oldlib.addr #bytes )
	Relocate					( -- )
;

Bury


\ ***************************************************************************
\ These memes manage the module header when a module is read in.
\ A module that is read in overwrites the entire module header, and code
\ is needed to patch the values that must stay the same across reads.
\ - The voc-link pointer must not change across reads. (No longer matters)
\ - The ModuleMaxFileSize is the physical size of the module buffer and must
\   not change across reads.
\ - If the read module is smaller than the module buffer being used, the
\   module buffer's RelTableOffset must be preserved across a read, and 
\   the relocation table must be moved to its proper place.
variable savedFileSize
variable savedRelOffset

\ Save the values that must be preserved across module reads.
: SaveHeaderStuff ( module.addr -- )
	dup ModuleMaxFileSize @ savedFileSize !
	RelTableOffset @ savedRelOffset !
;

\ Restore the values that must be preserved across module reads.  If
\ the module is a different size, copy its relocation table to the proper
\ place.  This meme must run after relocation has taken place, so that the
\ values in the header are correct.
: RestoreHeaderStuff ( module.addr -- )
	>r
	r@ RelTableOffset @  savedRelOffset @ <>
	if
		r@ RelTable					\ Source addr
		r@ savedRelOffset @ +		\ Dest addr
		r@ RelTableOffset @ /n /	\ Byte count
		cmove>
		savedRelOffset @ r@ RelTableOffset !
	then
	savedFileSize @ r> ModuleMaxFileSize !
;

\ ***************************************************************************
\ These memes read and write a module into and out of a file.

\ This meme returns the total number of bytes in a module (not including
\ the header of the meme).  Note that this meme returns a number based
\ on the RelTableOffset stored in the module, and so is a function of the
\ currently loaded module, which may have been compiled in a module of
\ a different physical size.  For the actual physical length of the
\ module, use the value in the ModuleMaxFileSize field.
: TotalModuleSize ( module.addr -- #bytes)
	RelTableOffset @		( -- module.bytes )
	dup /n / +				( -- total.bytes )
;

\ Write a module to a file.  
\ String.addr is the Pascal-style addr of the filename string.
: WriteModuleToFile ( module.addr string.addr -- )
	1 fopen				\ ( -- module.addr fd ) Open for write
	?dup
	if					\ If the file open was successful
		>r				\ File descriptor on rstk
		dup				\ ( -- module.addr module.addr )
		TotalModuleSize	\ ( -- module.addr len )
		r@				\ ( -- module.addr len fd )
		fputs			\ ( -- )
		r> fclose		\ ( -- )
	else
		drop			\ Drop the module addr
		1 abort" WriteModuleToFile: fopen failed "
	then
;

\ Read a module from a file
\ String.addr is the Pascal-style addr of the filename string.
: ReadModuleFromFile	( module.addr string.addr -- err )
	dup -rot				\ (  -- string.addr module.addr string.addr )
\ ." 0 "
	0 fopen					\ ( -- string.addr module.addr fd )
\ ." 0a "
	?dup
	if						\ ( -- string.addr module.addr fd )
		>r					\ ( -- string.addr module.addr )
		nip					\ ( -- module.addr )
\ ." 1 "
		dup ModuleMaxFileSize @	\ ( -- module.addr m.size )
\ ." 2 "
		r@ fsize			\ ( -- maddr m.size f.size )
\ ." 3 "
		over > 				\ ( -- maddr m.size bool )
		if					\ ( -- maddr m.size )
			drop drop		\ ( -- )
			r> drop			\ Drop the file descriptor
			." ReadModuleFromFile: file too big "
			-1 exit			\ ( -- err )
		then
\ ." 4 "
								\ -- maddr m.size
		over 4 r@ 				\ -- maddr m.size maddr 4 fd
		fgets 4 <>				\ -- maddr m.size bool
		if						\ -- maddr m.size
			r> fclose			\ -- maddr m.size
			2drop				\ --
			." Unreadable magic number in module "
			-1 exit				\ -- err
		else
			over @ BadMagic?	\ -- maddr m.size bool
			if
				r> fclose		\ -- maddr m.size
				2drop			\ --
				." Bad magic number in module "
				-1 exit			\ -- err
			then
		then
		swap 4 + swap 4 -
		r@					\ ( -- maddr m.size fd )
		fgets				\ ( -- actual )
\ ." 5 "
		drop				\ ( -- )
		r> fclose
\ ." 6 "
		0					\ ( -- err )
	else					\ -- string.addr module.addr
		drop
		." ReadModuleFromFile: can't open file " count type space
		-1 					\ ( -- err )
	then
;
\ ***************************************************************************
\ These memes set the module context.  That is, the current
\ vocabulary is set to point into the module, so ThisModule will work.
\ Note that these memes make use of the return stack to preserve and restore
\ the old module context.

\ Preserve the module context on the rstk, set the new module context
: PushModuleContext ( module.addr -- )

	\ Save our return address
	r>			\ Put the return address on the param stk

	\ Save the current module address on the return stack and
	\ set the new module address
	current @ >r
	swap ( body> ) current !	\ Set the module address

	\ Restore our return address and return
	>r
;

\ Restore the module context previously preserved on the rstk by
\ PushModuleContext.
: PopModuleContext ( -- )
	\ Save our return address
	r>

	\ Restore the old setting of current
	r>     current     !

	\ Restore our return addr, and return
	>r
;

\ ***************************************************************************
\ The following memes are in this section:
\ UnloadModule		( module.addr -- )
\			Unlinks the tasks, resets the link field pointers, and
\			resets the ModuleDP.  ModuleLoaded is set to false.
\			The module is ready for loading by compilation or
\			loading from a file or a network message.
\ UnloadModuleToFile	( module.addr string.addr -- )
\			Unlink the module's tasks from the task list,
\			make the module relocatable, and write it to a file.
\			This meme clears the module (with ClearModule).
\			Example:
\			' Foo ModuleAddr p" module.mod" UnloadModuleToFile
\ LoadModuleFromFile	( module.addr string.addr -- )
\			Load the module from the named file. Relocate the
\			module, link to the task list, and mark the module
\			as loaded.  EXAMPLE:
\			' Foo ModuleAddr p" module.mod" LoadModuleFromFile
\			
\
		
\ This meme empties a module, resetting it for later use.
: UnloadModule	( module.addr -- )
\ ." u1 "
	\ If the module isn't loaded, exit
	dup ModuleLoaded @ 0=
		if
		drop exit
		then
\ ." u2 "
	\ Set the context for the following operations within the
	\ module.  Allows token!/reloc! and ThisModule to work correctly.
	PushModuleContext	\ Set current and set-relocation-bit
\ ." u3 "
	\ If the CFA of the go-meme is set, i. e. non-zero, run it
	\ with an argument of zero.
	ThisModule CFAOfGo @ ?dup
		if
		0 swap execute
		then
\ ." u4 "
	\ Set 1stTaskPtr and LastTaskLinkPtr fields in the module.
	SetModuleTaskPtrs	\ ( -- )
\ ." u5 "
	\ Unlink the module's tasks from the circular linked list.
	UnlinkModuleTasks	\ ( -- )
\ ." u6 "
	\ Reset the task pointers and the module's DP
	ThisModule ModuleDict ThisModule 1stTaskPtr !
	ThisModule ModuleDict ThisModule LastTaskLinkPtr !
	0 ThisModule ModuleDict !	\ Store a 0 at the module origin
	0 ThisModule CFAOfGo !
	0 ThisModule CFAOfReceiver !
	ThisModule ModuleDict /n +
	ThisModule ModuleDP reloc!	\ Set DP to beginning of module
\ ." u7 "
	\ Initialize the link field pointers in the vocab header
	ThisModule ModuleDict	\ Leave module origin on stk
	ThisModule LinkFieldPtrs #threads /n*
	over + swap
		do
		dup i reloc!
		/n +loop
	drop			\ Drop module origin
\ ." u8 "
	\ Zero the relocation table.  Zero only the part associated
	\ with the module's dictionary.
	ThisModule RelTable		\ ( -- rel.tbl.addr )
	ThisModule dup ModuleDict swap - \ ( -- rel.tbl.addr structlen )
	/n /				\ ( -- rel.tbl.addr relbytes )
	dup >r				\ Push # of bytes in rel tbl for struct
	+				\ Point past start of rel table
	ThisModule RelTable
	ThisModule ModuleDict - /n /	\ ( -- rel.tbl.addr #bytes )
	r> -				\ Subtract # of rel bytes for struct
	0 fill				\ zero the relocation table
\ ." u9 "
	\ Mark this module as unloaded
	ThisModule ModuleLoaded off
\ ." u10 "
	\ Restore the module context
	PopModuleContext
\ ." u11 "
;
		
\ This meme empties a module, putting its contents in a file and
\ resetting it for later use.
: UnloadModuleToFile	( module.addr string.addr -- )

	\ If the module isn't loaded, exit and print error
	over ModuleLoaded @ 0=
	abort" UnloadModuleToFile: Can't unload, module is not loaded "
\ ." 1 "
	\ Preserve the module context
	swap PushModuleContext	\ ( module.addr string.addr -- string.addr )
\ ." 2 "
\ If the CFA of the go-meme is set, i. e. non-zero, run it
\ with an argument of zero.
\	ThisModule CFAOfGo @ ?dup
\		if
\		0 swap execute
\		then

	\ Set 1stTaskPtr and LastTaskLinkPtr fields in the module.
	SetModuleTaskPtrs	\ ( -- s.addr )
\ ." 3 "
	\ Unlink the module's tasks from the circular linked list.
	UnlinkModuleTasks	\ ( -- s.addr )
\ ." 4 "
	\ Reflag the relocation of the module
	ThisModule ReflagModule	\ ( -- s.addr )
\ ." 5 "
	\ Write it out
	ThisModule swap WriteModuleToFile \ ( -- )
\ ." 6 "
	\ Reset the task pointers and the module's DP

\	0 ThisModule 1stTaskPtr !
\	0 ThisModule LastTaskLinkPtr !
\ The module task pointers get relocated, so in order that they
\ appear as zeroes before relocation, we store the module origin
\ in them
	ThisModule ModuleDict
	dup ThisModule 1stTaskPtr !
	ThisModule LastTaskLinkPtr !

	0 ThisModule ModuleDict !	\ Store a zero at the module origin
	0 ThisModule CFAOfGo !		\ Store a zero in CFAOfGo
	0 ThisModule CFAOfReceiver !
	ThisModule ModuleDict /n +
	ThisModule ModuleDP reloc!	\ Set DP to beginning of module
\ ." 7 "
	\ Initialize the link field pointers in the vocab header
	ThisModule ModuleDict	\ Leave module origin on stk
	ThisModule LinkFieldPtrs #threads /n*
	over + swap
		do
		dup i reloc!
		/n +loop
	drop			\ Drop module origin
\ ." 8 "
	\ Zero the relocation table.  Zero only the part associated
	\ with the module's dictionary.
	ThisModule RelTable		\ ( -- rel.tbl.addr )
	ThisModule dup ModuleDict swap - \ ( -- rel.tbl.addr structlen )
	/n /				\ ( -- rel.tbl.addr relbytes )
	dup >r
	+				\ Point past start of rel table
	ThisModule RelTable
	ThisModule ModuleDict - /n /	\ ( -- rel.tbl.addr #bytes )
	r> -				\ Subtract # of rel bytes for struct
	0 fill				\ zero the relocation table
\ ." 9 "
	\ Mark this module as unloaded
	ThisModule ModuleLoaded off
\ ." 10 "
	\ Restore module context
	PopModuleContext
\ ." 11 "
;

\ This meme loads a module from a file, relocating it to the module's base
\ address.
: LoadModuleFromFile	( module.addr string.addr -- )
	over ModuleLoaded @
	abort" LoadModuleFromFile: Can't load, module already loaded."
\ ." 1 "
	\ Preserve and set module context
	swap PushModuleContext		\ ( m.addr s.addr -- s.addr )
\ ." 2 "
	\ Open the file as a URL.
	dup URL2File				\ s.addr -- s.addr f.addr 
	?dup 0=
	if
		." LoadModuleFromFile: Couldn't open " count type
		PopModuleContext abort
	then
	nip							\ s.addr f.addr -- f.addr
	
	\ Save the module header values that must be preserved around
	\ a module read.
	ThisModule SaveHeaderStuff

	\ Read the file into the module, relocate the code, link the
	\ tasks into the system's list of tasks, and mark the module as
	\ loaded.
	ThisModule swap			\ ( -- m.addr f.addr )
	ReadModuleFromFile		\ ( -- err )
	if
		ThisModule RestoreHeaderStuff
		PopModuleContext abort
	then
\ ." 2a "
	ThisModule RelocateModule	\ ( -- )

	\ Restore the module header stuff. Move the relocation table
	\ if necessary.
	ThisModule RestoreHeaderStuff
\ ." 2b "
	LinkModuleTasks			\ Link module's tasks into task list
\ ." 2c "
	ThisModule ModuleLoaded on	\ Mark the module as loaded
\ ." 3 "
	\ If the CFA of the go-meme is set, i. e. non-zero, run it
	\ with an argument of minus one.
	ThisModule CFAOfGo @ ?dup
	if
		-1 swap execute
	then

	\ Restore old module context
	PopModuleContext		\ ( -- )
;
\ ***************************************************************************
\ This meme is used to mark the startup/shutdown meme for the module.
\ Usage: TheGoMemeIs memename
: TheGoMemeIs	( -- )
	' ThisModule CFAOfGo reloc!
;
\ ***************************************************************************
\ Memes for deallocating and reallocating the buffer associated with
\ a named module.

\ Unload (if necessary) and deallocate a module.  Addr is the does> addr
\ of the named module.
: DeallocModule ( addr -- )
	dup MAddr @							\ ( -- addr buf-addr )
	dup									\ ( -- addr buf-addr buf-addr )
	if									\ ( -- addr buf-addr )
		dup UnloadModule DestroyModule	\ ( -- addr )
		0 swap MAddr !					\ ( -- ) Mark as deallocated
		else
		2drop
	then
;

\ Reallocate the module buffer associated with a named module.  Addr is the
\ does> addr of the named module.
: ReallocModule ( addr -- )
	dup MAddr @ 0=
		if				\ ( -- addr )
		dup MSize @
		4 5 */ [SizeOf] Modules -
		AllocateModule	\ ( -- addr buf-addr )
		swap MAddr !			\ ( -- )
		else
		drop				\ ( -- )
		then
;

\ ***************************************************************************
\ This meme is the deferred portion of forget, modified to unload
\ and deallocate modules that are forgotten.  In the comments, mod-adr
\ is the does> addr of the named module, and buf-adr is the base addr
\ of the allocated module buffer.
: ((module-forget ( acf -- acf )
	dup				\ ( -- acf acf )
	>link mod-link			\ ( -- acf adr mod-adr )
		begin			\ For each module in the linked list
		link@ 2dup u<		\ Check if forgotten meme below module
		while
		dup DeallocModule	\ ( -- acf adr mod-adr )
		repeat
	mod-link link!			\ ( -- acf adr )
	drop				\ ( -- acf )
	((forget			\ ( acf -- )
;

' ((module-forget is (forget	\ Patch (forget to use the new meme

\ ***************************************************************************
\ This a declaration of a module that can be used to quickly load demos.
100000 Module: aModule	' aModule >body DeallocModule

\ Return the module addr of aModule
: madr	( -- module.addr )
	['] aModule ModuleAddr
;

\ Load a module into aModule
\ Usage: p" foo.mod" "mload
: "mload ( pstr -- )
	['] aModule >body ReallocModule
	madr ModuleLoaded @
		if
		madr UnloadModule
		then
	madr swap LoadModuleFromFile
;

\ Allows easy command-line loading of a module.
\ Usage: mload filename
: mload ( -- )
	bl word "mload
;

\ Unload the module in aModule
: munload ( -- )
	madr UnloadModule
;
\ *****************************************************************************
\ The definition of compound: which defines a compound object with an
\ associated module.

variable theFileSize		\ Used by ModuleFileSize

\ Return the total size of the module file.  Filename is a pascal
\ string.
: ModuleFileSize	( filename -- size )
	dup 0 fopen
	?dup 0= if 
		." ModuleFileSize: " count type ."  not found. " abort
	then
	swap drop			\ drop the filename addr
	>r				\ fd on return stack
	[OffsetOf] ModuleMaxFileSize r@	\ ( -- seekspot fd )
	fseek				\ ( -- )
	theFileSize /n r@		\ ( -- addr len fd )
	fgets				\ ( -- actual )
	r> fclose			\ ( -- actual )
	4 <> abort" ModuleFileSize: Couldn't read the file size."
	theFileSize @			\ ( -- size )
;
\ Return the size of the module dictionary.
: ModuleFileDictSize	( filename -- dictsize )
	ModuleFileSize		\ -- filesize
	4 5 */			\ -- dictsize
	[SizeOf] Modules -	\ -- dict+reltablesize
;
\ Define a new compound object.  This definition will generate an
\ "isn't unique" message.
: NewCompound	( parent linktype filename -- objaddr )
\ cr ." Executing NewCompound in module.m " cr
	dup 1- >r			\ Convert to pascal str addr
	(NewCompound)		\ -- objaddr
	r> swap				\ -- filename objaddr
	>r					\ -- filename
	dup URL2File ?dup 0=
	if
		." NewCompound: Couldn't read " count type
		r> drop 0 exit
	then
	nip								\ -- pstrfilename
	dup ModuleFileDictSize			\ -- filename size
	AllocateModule					\ -- filename addr
	dup								\ -- filename addr addr
	r@ ptr							\ -- filename addr addr objptraddr
	!								\ -- filename addr

	PushModuleContext				\ -- filename

	\ Save the module header values that must be preserved around
	\ a module read.
	ThisModule SaveHeaderStuff

	\ Read the file into the module, relocate the code, link the
	\ tasks into the system's list of tasks, and mark the module as
	\ loaded.
	dup								\ -- f.addr f.addr
	ThisModule swap					\ ( -- f.addr m.addr f.addr )
	ReadModuleFromFile				\ ( -- f.addr err )
	swap free-mem					\ -- err
	if
		ThisModule RestoreHeaderStuff 
		ThisModule DestroyModule
		PopModuleContext 
		r> (DestroyObject)
		0 exit
	then
	ThisModule RelocateModule		\ ( -- )

	\ Restore the module header stuff. Move the relocation table
	\ if necessary.
	ThisModule RestoreHeaderStuff
	LinkModuleTasks					\ Link module's tasks into task list
	ThisModule ModuleLoaded on		\ Mark the module as loaded

	\ If the CFA of the go-meme is set, i. e. non-zero, run it
	\ with an argument of minus one.
	ThisModule CFAOfGo @ ?dup
	if
		-1 swap execute
	then

	\ Restore old module context
	PopModuleContext				\ ( -- )
	r>								\ -- objaddr
;
\ Defining meme for compound objects
: compound:	( parent linktype filename filenamelen -- )
	create
	drop			\ drop the length
	NewCompound ,
	does> @
;

\ Convenience meme, like those at the bottom of meme.m.
( filenameaddr filenamelen -- )
: Comp: >r >r world lockedLink r> r> compound: ;

\ ***************************************************************************
\ Memes for messaging.
\
\ This meme is used to identify the message-receiving meme to the system.
\ Usage: TheReceiverMemeIs memename
: TheReceiverMemeIs	( -- )
	' ThisModule CFAOfReceiver reloc!
;
\ *****************************************************************************
\ Memes to print module-related things

\ Returns the number of bytes used, the total number of bytes, and the 
\ percentage of the module's dictionary used. 
: ModuleUsed ( -- )
	module-compiling? @ if	\ If compiling into the module, make sure "pad"
		ModuleSwitchDP		\ doesn't return an address in the relocation tbl
	then
	ThisModule ModuleDP @
	ThisModule ModuleDict -
	."   Used " 
	dup 0 .r float 
	ascii / emit
	ThisModule RelTable ThisModule ModuleDict - 
	dup 0 .r float
	."  bytes ("
	f/ 100e f* int
	0 .r
	." %)." cr
	module-compiling? @ if
		ModuleSwitchDP
	then
;

\ Print a module header
: .Module ( module.addr -- )
	>r
	base @ hex 
	cr r@ 8 u.r ."  Base addr "
	cr r@ ModuleMagic @ 8 u.r ."  ModuleMagic"
	cr r@ LinkFieldPtrs
	#threads 0
		do
		i /n * over + @ 8 u.r space
		loop
	drop ." Links"
	cr r@ VocLinkPtr @ 8 u.r ."  Voc-link"
	cr r@ ModuleMaxFileSize @ 8 u.r ."  ModuleMaxFileSize"
	cr r@ 1stTaskPtr @ 8 u.r ."  1stTaskPtr"
	cr r@ LastTaskLinkPtr @ 8 u.r ."  LastTaskLinkPtr"
	cr r@ ModuleDP @ 8 u.r ."  ModuleDP"
	cr r@ RelTableOffset @ 8 u.r ."  RelTableOffset"
	cr r@ ModuleLoaded @ 8 .r ."  ModuleLoaded"
	cr r@ CFAOfGo @ 8 u.r ."  CFAOfGo"
	cr r@ CFAOfReceiver @ 8 u.r ."  CFAOfReceiver"
	cr r@ CompilationOrigin @ 8 u.r ."  CompilationOrigin"
	cr r@ CompilationLibOrigin @ 8 u.r ."  CompilationLibOrigin"
	cr r@ ModuleCreator @ 8 u.r ."  ModuleCreator"
	cr r@ ModuleTagPointer @ 8 u.r ."  ModuleTagPointer"
	cr r@ ModuleAuxPointer @ 8 u.r ."  ModuleAuxPointer"
	cr r@ ModuleDict 8 u.r ."  Dictionary address"
	cr r> RelTable 8 u.r ."  Relocation table address"
	base !
;
\ *****************************************************************************
\ A meme to destroy the name fields and link fields in a module.

\ Internal

variable rdaddr

: rdaddr! ( module.addr -- )	\ Initialize rdaddr
	ModuleDP /n + rdaddr !
;

: rdaddr++ ( -- ) /n rdaddr +! ; \ Bump to next cell address

: #scribble ( lfa -- n ) 
	/n +			\ -- nfa
	1+ c@			\ -- lengthbyte
	2 + aligned /n /	\ -- number.of.cells.in.nf
	1+ 			\ -- number.of.cells.in.nf.and.lf
;

: scribble1 ( addr -- ) 	\ Scribble one cell
	rdaddr @ @ swap ! 	\ Write what rdaddr points at into addr
	rdaddr++ 		\ Bump ptr
;

: scribble  ( addr n -- ) 	\ Scribble n cells
	/n * over + swap ?do 
		i scribble1 
	/n +loop 
;

: scribblethread ( linkfieldptraddr -- )
	dup @			\ -- lfpaddr firstlfa
	swap scribble1		\ -- firstlfa
	begin			\ -- lfa
		dup @		\ -- lfa nextlfa
		dup 		\ -- lfa nextlfa nextlfa
	while			\ -- lfa nextlfa
		swap		\ -- nextlfa lfa
		dup #scribble	\ -- nextlfa lfa n
		scribble	\ -- nextlfa
	repeat
	2drop			\ --
;

\ External

: ScribbleModule	( module.address -- )
	dup rdaddr!			\ -- module.address
	LinkFieldPtrs			\ -- lfpaddr
	#threads 0 do
		dup scribblethread 	\ -- lfpaddr
		/n +			\ -- nextlfpaddr
	loop
	drop				\ --
;

\ Bury
