!include "../global.mak"

ALL : "$(OUTDIR)\MQ2Notepad.dll"

CLEAN :
	-@erase "$(INTDIR)\MQ2Notepad.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MQ2Notepad.dll"
	-@erase "$(OUTDIR)\MQ2Notepad.exp"
	-@erase "$(OUTDIR)\MQ2Notepad.lib"
	-@erase "$(OUTDIR)\MQ2Notepad.pdb"


LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(DETLIB) ..\Release\MQ2Main.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MQ2Notepad.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MQ2Notepad.dll" /implib:"$(OUTDIR)\MQ2Notepad.lib" /OPT:NOICF /OPT:NOREF 
LINK32_OBJS= \
	"$(INTDIR)\MQ2Notepad.obj" \
	"$(OUTDIR)\MQ2Main.lib"

"$(OUTDIR)\MQ2Notepad.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) $(LINK32_FLAGS) $(LINK32_OBJS)


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MQ2Notepad.dep")
!INCLUDE "MQ2Notepad.dep"
!ELSE 
!MESSAGE Warning: cannot find "MQ2Notepad.dep"
!ENDIF 
!ENDIF 


SOURCE=.\MQ2Notepad.cpp

"$(INTDIR)\MQ2Notepad.obj" : $(SOURCE) "$(INTDIR)"

