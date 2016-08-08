# Microsoft Developer Studio Project File - Name="MEME" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MEME - Win32 Meme Float Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "meme.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "meme.mak" CFG="MEME - Win32 Meme Float Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MEME - Win32 Meme Float Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MEME - Win32 Meme Float Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MEME - Win32 Memeview Float Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MEME - Win32 Memeview Float Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinRel"
# PROP Intermediate_Dir ".\WinRel"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Ot /Og /Oi /Oy /Ob2 /Gy /I "\memedist\rwwin\include" /I "\memedist\mds\winrwvc" /I "\memedist\mds" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /FD /c
# SUBTRACT CPP /Ox /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 rwl21.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /version:2.80 /subsystem:windows /pdb:none /map /machine:I386 /libpath:"\memedist\rwwin\lib"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Extend, serialize, config
TargetPath=.\WinRel\meme.exe
TargetName=meme
InputPath=.\WinRel\meme.exe
SOURCE="$(InputPath)"

BuildCmds= \
	$(TargetPath) initdict -f loadutil.m \
	\memedist\util\sernum -f $(TargetPath) \
	\memedist\util\config -f $(TargetPath) -n winsock 0.0.0.0 \
	

"$(TargetPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\$(TargetName).dic" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build
# Begin Special Build Tool
TargetPath=.\WinRel\meme.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Copy interpreter to \meme
PostBuild_Cmds=cp -m $(TargetPath) \meme	touch meme.dic     	cp -m meme.dic \meme
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WinDebug"
# PROP Intermediate_Dir ".\WinDebug"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /G5 /ML /W3 /GR /GX /ZI /Od /I "\memedist\rwwin\include" /I "\memedist\mds\winrwvc" /I "\memedist\mds" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib rwl21.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /version:2.74 /subsystem:windows /incremental:no /debug /debugtype:both /machine:I386 /pdbtype:sept /libpath:"\memedist\rwwin\lib"
# SUBTRACT LINK32 /verbose /profile /pdb:none /map
# Begin Custom Build
TargetPath=.\WinDebug\meme.exe
TargetName=meme
InputPath=.\WinDebug\meme.exe
SOURCE="$(InputPath)"

BuildCmds= \
	$(TargetPath) initdict -f loadutil.m \
	\memedist\util\sernum -f $(TargetPath) \
	\memedist\util\config -f $(TargetPath) -n winsock 0.0.0.0 \
	

"$(TargetPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\$(TargetName).dic" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build
# Begin Special Build Tool
TargetPath=.\WinDebug\meme.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Copy interpreter to \meme
PostBuild_Cmds=cp -m $(TargetPath) \meme	touch meme.dic	cp -m meme.dic \meme
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Memeview"
# PROP BASE Intermediate_Dir ".\Memeview"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Memevrel"
# PROP Intermediate_Dir ".\Memevrel"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "c:\meme\winrwvc" /I "c:\meme" /I "c:\rwwin\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /FAs /FR /YX /c
# ADD CPP /nologo /G5 /W3 /GX /Ot /Og /Oi /Oy /Ob2 /I "\memedist\rwwin\include" /I "\memedist\mds\winrwvc" /I "\memedist\mds" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /D "MEMEVIEW" /FD /c
# SUBTRACT CPP /Ox /Os /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib c:\rwwin\lib\rwnlp.lib wsock32.lib /nologo /subsystem:windows /incremental:yes /machine:I386
# SUBTRACT BASE LINK32 /verbose /pdb:none
# ADD LINK32 ole32.lib oleaut32.lib odbc32.lib odbccp32.lib rwl21.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /version:2.74 /subsystem:windows /pdb:none /map /machine:I386 /out:".\Memevrel\MEMEVIEW.exe" /libpath:"\memedist\rwwin\lib"
# SUBTRACT LINK32 /verbose
# Begin Custom Build - Serialize, config
TargetPath=.\Memevrel\MEMEVIEW.exe
InputPath=.\Memevrel\MEMEVIEW.exe
SOURCE="$(InputPath)"

"$(TargetPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	\memedist\util\sernum -f $(TargetPath) 
	\memedist\util\config -f $(TargetPath) -n winsock 0.0.0.0 
	
# End Custom Build
# Begin Special Build Tool
TargetPath=.\Memevrel\MEMEVIEW.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Move memeview to \meme
PostBuild_Cmds=cp -m $(TargetPath) \meme
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Memevie0"
# PROP BASE Intermediate_Dir ".\Memevie0"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Memevdbg"
# PROP Intermediate_Dir ".\Memevdbg"
# PROP Ignore_Export_Lib 0
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /I "c:\meme\winrwvc" /I "c:\meme" /I "c:\rwwin\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /FAs /FR /YX /c
# ADD CPP /nologo /G5 /ML /W3 /Gm /Gi /GX /ZI /Od /I "\memedist\rwwin\include" /I "\memedist\mds\winrwvc" /I "\memedist\mds" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "RWFLOAT" /D "MEMEVIEW" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib c:\rwwin\lib\rwnlp.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /verbose /profile /pdb:none /incremental:no
# ADD LINK32 ole32.lib oleaut32.lib odbc32.lib odbccp32.lib rwl21.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib /nologo /version:2.73 /subsystem:windows /debug /machine:I386 /out:".\Memevdbg\MEMEVIEW.exe" /libpath:"\memedist\rwwin\lib"
# SUBTRACT LINK32 /verbose /profile /pdb:none /incremental:no /map
# Begin Custom Build
TargetPath=.\Memevdbg\MEMEVIEW.exe
InputPath=.\Memevdbg\MEMEVIEW.exe
SOURCE="$(InputPath)"

"$(TargetPath)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	\memedist\util\sernum -f $(TargetPath) 
	\memedist\util\config -f $(TargetPath) -n winsock 0.0.0.0 
	
# End Custom Build
# Begin Special Build Tool
TargetPath=.\Memevdbg\MEMEVIEW.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Move memeview to \meme
PostBuild_Cmds=cp -m $(TargetPath) \meme
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "MEME - Win32 Meme Float Win32 Release"
# Name "MEME - Win32 Meme Float Win32 Debug"
# Name "MEME - Win32 Memeview Float Win32 Release"
# Name "MEME - Win32 Memeview Float Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Allocdic.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\bitcon.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Chario.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Cyberspc.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\debuglog.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Decser.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dkmodule.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\expiry.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Extend.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Forth.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

# ADD BASE CPP /FAcs
# ADD CPP /FAcs

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Globals.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Init.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Io.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Main.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Matherr.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Meme.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MEME.rc
# End Source File
# Begin Source File

SOURCE=.\Memejoy.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Mememat.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Mememaus.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\memenet.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Memerend.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Memeser.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Mstimer.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Often.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\rfcache.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Rfile.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Syscall.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Util.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Winglobl.c

!IF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Release"

# ADD CPP /nologo /Ze /Gi-

!ELSEIF  "$(CFG)" == "MEME - Win32 Meme Float Win32 Debug"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Release"

!ELSEIF  "$(CFG)" == "MEME - Win32 Memeview Float Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\bitcon.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\Cyberspc.h
# End Source File
# Begin Source File

SOURCE=..\errors.h
# End Source File
# Begin Source File

SOURCE=..\externs.h
# End Source File
# Begin Source File

SOURCE=..\forth.h
# End Source File
# Begin Source File

SOURCE=..\meme.h
# End Source File
# Begin Source File

SOURCE=..\mememat.h
# End Source File
# Begin Source File

SOURCE=.\memenet.h
# End Source File
# Begin Source File

SOURCE=..\MEMEOPS.H
# End Source File
# Begin Source File

SOURCE=.\memerend.h
# End Source File
# Begin Source File

SOURCE=..\MEMEVER.H
# End Source File
# Begin Source File

SOURCE=..\messages.h
# End Source File
# Begin Source File

SOURCE=..\mstimer.h
# End Source File
# Begin Source File

SOURCE=..\prims.h
# End Source File
# Begin Source File

SOURCE=..\protos.h
# End Source File
# Begin Source File

SOURCE=.\psprotos.h
# End Source File
# Begin Source File

SOURCE=.\Renderer.h
# End Source File
# Begin Source File

SOURCE=..\rfile.h
# End Source File
# Begin Source File

SOURCE=..\SEROPS.H
# End Source File
# Begin Source File

SOURCE="..\..\Program Files\DevStudio\Vc\include\sys\Stat.h"
# End Source File
# Begin Source File

SOURCE="..\..\Program Files\DevStudio\Vc\include\sys\Timeb.h"
# End Source File
# Begin Source File

SOURCE="..\..\Program Files\DevStudio\Vc\include\sys\Types.h"
# End Source File
# Begin Source File

SOURCE=.\Winextrn.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\icon6.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\material.mh
# End Source File
# End Target
# End Project
