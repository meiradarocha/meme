: cd  bl word "chdir  abort" Can't change directory"  ;
: pwd 256 alloc-mem ?dup 0= abort" Can't alloc pwd buffer" dup getcwd
	dup ctype free-mem ;

\ : fload bl word dup count type "load cr ;

: mod\ [compile] \ ; immediate
: main\ ; immediate

cd ..

fload bury.m
fload loadcom.fth
fload cmdcom.fth
fload locals.fth		\ Local variable support -MdG
fload dosmsc\doscmd.fth
fload dosmsc\ports.fth
fload struct.m
fload tasking.m
fload taskutil.m

main definitions

fload errors.mh			\ Error and progress codes
fload memeutil.m
fload meme.m
fload material.mh		\ Meme material data structure

fload mstructs.mh		\ Module structure definitions
fload stdml.m			\ The standard module library
fload module.m			\ The module code
fload modstuff.m		\ Module-related code

fload startmod.m		\ Execute the module named in the -m cmd line option

fload anomalo.m
fload net.m

fload dcmp.m

cd winrwvc

p" meme.dic" save-dictionary
