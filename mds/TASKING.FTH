\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ Multitasking

only main also hidden also   hidden definitions

80  20 /n*  +	constant task-rs-size
h# 100 /n*	constant task-ps-size
h# 400 /n*	constant task-dictionary-size

up@ link !			\ link the current task to itself (token!)

: local ( task-base user-var-addr -- user-var-addr-in-tasks-space )
   up@ - +
;

\ put the task at task-addr to sleep (make it inactive)
: sleep ( task-addr -- )  task-active? local off  ;

\ awaken the task at task-addr (make it active)
: wake  ( task-addr -- )  task-active? local  on  ;

\ put current task to sleep
: stop  ( -- )  up@ sleep pause  ;

\ Layout of private storage for a new task:
\     Space             Size
\     -----             ----
\     User Area         user-size
\     Dictionary	task-dictionary-size
\     Parameter Stack   task-ps-size
\     TIB		task-rs-size
\     Return Stack      task-rs-size
\
\ The terminal input buffer and the Return Stack share an area of
\ size task-rs-size. TIB grows up, Return Stack grows down.

\ Before the new task has been forked, invoking the task name will
\ return its pfa.  After it has been forked, it will return the
\ address of its user area
\ Pfa of task contains the address and size

: "task: ( size name -- ) \ name and allocate a new task
   "create  ( size )
   here token, , does> @
;

: default-task-size ( -- size )
   user-size h# 200 +
;

: task:  \ name  ( -- name ) \ name and allocate a new task using default size
   default-task-size bl word  "task:
;

nuser task-size

\ Allocate and initialize the user area for the new task, schedule it
: tfork ( task-pfa -- task-user-area )
  \ Allocate run-time space
   dup na1+ @        			( task-pfa task-size)
   task-ps-size task-rs-size + +	( task-pfa task-size-with-stacks )
   task-dictionary-size +		( task-pfa size-with-stacks-and-dict )
   dup alloc-mem     			( task-pfa task-size task-address)
   
  \ Check to make sure that we were able to allocate the memory
  
   2dup swap erase	\ debug code, zerofill the user's memory

  \ Initialize the user area with a copy of the current task's user area
  \ up@  over  #user @  cmove		( task-pfa  task-size  task-address)
  up@		( task-pfa  task-size  task-addr  our-up)
  over		( task-pfa  task-size  task-addr  out-up  task-addr)
  #user		( task-pfa  task-size  task-addr  our-up task-addr #user-addr)
  @		( task-pfa  task-size  task-addr  our-up task-addr bytes-used)
  cmove		( task-pfa  task-size  task-addr)
  
  \ Since we copied the user area, his link already points to my successor.
  \ Now make him my new successor in the task queue.
   dup link !	\  * was token!		( task-pfa  task-size task-addr)

  \ Save my UP and switch to the new task's user area
   up@ >r  up!				( task-pfa  task-size)

  \ Fix the parameter field of the task word so it points to the new
  \ user area instead of to itself
   up@ rot !	\ was token!	( task-size )

  \ Set the task address and task size user variables
   dup task-size !
   up@ +				( top-of-new-task-data-area)
   dup rp0 token!	\ was token!	( top-of-new-return-stack)
   
  \ Save the address of the top of the new task's data stack
   task-rs-size -			( top-of-new-data-stack & start of TIB )
   dup tib !
   dup sp0 token!

  \ Set up the new task's dictionary area
   task-dictionary-size -		( start-of-dictionary-area )
   dp !
   
  \ Set the new task's user pointer
   up@  up0 !

  \ Put the new task to sleep
   up@ sleep

  \ Restore my up
   up@   r> up!				( top-of-new-data-stack task-addr)

  \ set the new task's stack top
\   dup -rot sp0 local token!		( task-addr)			
;

\ Restart the task using its word to execute
: set-task ( ip task -- )
   up@ -rot   up!    ( my-up ip )

  \ Initialize ip,rp,sp for the task
   saved-ip !    ( my-up )
   sp0 @  saved-sp token!	\ was token! ( my-up )
   rp0 @  saved-rp token! 	\ was token! ( my-up )

   up!
;

\ Establish a word for the task to execute
: start ( cfa task -- )
   swap >body swap  ( ip task )
   set-task
;

\ The test for already-forked relies on the fact that the task pfa
\ initially contains its own address.  After forking, the task pfa
\ contains the address of the tasks-user area.  Obviously, this
\ test wouldn't work if the first user variable pointed to itself,
\ but it doesn't, so its okay

: fork ( cfa task-pfa -- )  \ give the task a word to execute
 \ Make sure the world is set up for multitaking
   multi
 \ Don't fork if we've already done so
   dup dup @ =  if  tfork  then  ( cfa task )
   start
;

\ Defining word for a task and its action.  Example:
\  variable counts
\  background counter   begin pause 1 counts +! again  ;

: background ( -- )
   ( default-task-size ) task:
   multi
   lastacf >body
   dup dup @ =  if  tfork  then  ( task )
   here swap set-task
   !csp ]
;

\ Compiled inside a definition to change the behavior of an
\ existing task, for example (assuming the previous definition of counter):
\  : countdown   counter activate  begin pause -1 counts +! again ;

: activate ( -- )  r> over set-task wake  ;
