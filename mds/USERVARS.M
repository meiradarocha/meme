\ This file is copyright by Immersive Systems, Inc.  All rights
\ reserved.  You are permitted to use this file subject to
\ the provisions and restrictions in the license agreement, which
\ is in the file LICENSE or LICENSE.txt or LICENSE.html.  If this
\ set of files does not contain a file by one of the three
\ preceding names, please contact the Metatopia project at
\ sourceforge.net or at immersive.com.

\ User variables, needed by some words in the modcode.
\ by Marc de Groot.
\ Copyright (c) Immersive Systems, Inc.  All rights reserved.

base @ decimal
0		user #user
4		user span
8		user >in
12 		user base
16		user blk
20		user #tib
24		user state
28		user current
32		user delimiter
36		user sp0
40		user rp0
44		user up0
48		user last
52		user #out
56		user #line
60		user voc-link
64		user dpl
68		user warning
72		user caps
76		user v_errno
80		user #places
84		user handler
88		user 'expect
92		user link
96		user task-active?
100		user saved-ip
104		user saved-rp
108		user saved-sp
112		user dp
116		user debugging?
120		user 'debug
124		user <ip
128		user ip>
132		user cnt
136		user module-compiling?
140		user 'tib
144		user mod-link
148		user saved-fp
152		user fp0
156		user turnkey
160		user named-task?
164		user pertask
168		user saved-frp
172		user region-var
176		user context
hld up@ -	user hld
base !
