// MQ2Notepad.cpp : Defines the entry point for the DLL application.
//
//
// MQ2Notepad by Cosmic
// First release 10/20/2005
//
// Updated 5/7/2012 by Sym :: Set default dir to macro folder, cleaned up some code
// Updated 5/9/2012 by Maskoi :: Will no longer crash if file size over 64k. Will just tell you file is too big.
//
// known bugs:
// 1: Lines limited to 999 characters
// 2: Filesize limited to 64k
// 3: Path limited to 299 characters
// 4: exceeding any of these causes things to go boom
// 5: couldn't make horizontal scrollbars work, so words wrap. its annoying.
//
//
// Usage: (Don't forget to put the MQUI_NotepadWindow.xml in your UI directory
// /notepad dir - display current working directory
// /notepad dir <directoryname> - set current working directory
// /notepad <filename> - open file for editing
//


#include "../MQ2Plugin.h"
#include <iostream>

PreSetup("MQ2Notepad");

void SaveFile();

#define CWS_WANTRETURN      0x40000
char Directory[MAX_STRING];
char FileName[MAX_STRING];

class CNotepadWnd : public CCustomWnd {
public:
    CNotepadWnd():CCustomWnd("NotepadWindow") {
        SaveButton = (CButtonWnd*)GetChildItem("NPWSave");
        NoteBox = (CTextEntryWnd*)GetChildItem("NPWInput");
        NoteBox->WindowStyle|=CWS_WANTRETURN;
        SetWndNotification(CNotepadWnd);
    }
   
    ~CNotepadWnd()
    {
    }

    int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) {   
        //Save Button
        if (pWnd==(CXWnd*)SaveButton) {
            if (Message==XWM_LCLICK) {
                SaveFile();
            } else {
                DebugSpew("SaveButton message %Xh / %d",Message,Message);
            }
        }
        return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
    };
    CButtonWnd *SaveButton;
    CTextEntryWnd *NoteBox;
};
CNotepadWnd *MyWnd = 0;


void ReadWindowINI(PCSIDLWND pWindow) {
    pWindow->Location.top      = GetPrivateProfileInt("Settings","ChatTop",100,INIFileName);
    pWindow->Location.bottom   = GetPrivateProfileInt("Settings","ChatBottom",800,INIFileName);
    pWindow->Location.left     = GetPrivateProfileInt("Settings","ChatLeft",100,INIFileName);
    pWindow->Location.right    = GetPrivateProfileInt("Settings","ChatRight",800,INIFileName);
	pWindow->Locked			   = (GetPrivateProfileInt("Settings", "Locked", 0, INIFileName) ? true:false);
    pWindow->Fades             = (GetPrivateProfileInt("Settings","Fades",1,INIFileName) ? true:false);
    pWindow->FadeDelay		   = GetPrivateProfileInt("Settings","Delay",2000,INIFileName);
    pWindow->FadeDuration      = GetPrivateProfileInt("Settings","Duration",500,INIFileName);
    pWindow->Alpha             = GetPrivateProfileInt("Settings","Alpha",255,INIFileName);
    pWindow->FadeToAlpha       = GetPrivateProfileInt("Settings","FadeToAlpha",255,INIFileName);
    pWindow->BGType            = GetPrivateProfileInt("Settings","BGType",1,INIFileName);
	ARGBCOLOR col = { 0 };
	col.ARGB = pWindow->BGColor;
	col.A = GetPrivateProfileInt("Settings", "BGTint.alpha", 255, INIFileName);
	col.R = GetPrivateProfileInt("Settings", "BGTint.red", 0, INIFileName);
	col.G =  GetPrivateProfileInt("Settings", "BGTint.green", 0, INIFileName);
	col.B = GetPrivateProfileInt("Settings", "BGTint.blue", 0, INIFileName);
    GetPrivateProfileString("Settings","Directory",gszMacroPath,Directory,999,INIFileName);
}
template <unsigned int _Size>LPSTR SafeItoa(int _Value, char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
void WriteWindowINI(PCSIDLWND pWindow) {
    CHAR szTemp[MAX_STRING] = {0};

    if (pWindow->Minimized) {
        WritePrivateProfileString("Settings","ChatTop", SafeItoa(pWindow->OldLocation.top,szTemp,10),INIFileName);
        WritePrivateProfileString("Settings","ChatBottom", SafeItoa(pWindow->OldLocation.bottom,szTemp,10),INIFileName);
        WritePrivateProfileString("Settings","ChatLeft",SafeItoa(pWindow->OldLocation.left,szTemp,10),INIFileName);
        WritePrivateProfileString("Settings","ChatRight",SafeItoa(pWindow->OldLocation.right,szTemp,10),INIFileName);
    } else {
        WritePrivateProfileString("Settings","ChatTop",SafeItoa(pWindow->Location.top,szTemp,10),INIFileName);
        WritePrivateProfileString("Settings","ChatBottom",SafeItoa(pWindow->Location.bottom,szTemp,10),INIFileName);
        WritePrivateProfileString("Settings","ChatLeft",SafeItoa(pWindow->Location.left,szTemp,10),INIFileName);
        WritePrivateProfileString("Settings","ChatRight",SafeItoa(pWindow->Location.right,szTemp,10),INIFileName);
    }

    WritePrivateProfileString("Settings","Locked",SafeItoa(pWindow->Locked,szTemp,10),INIFileName);
    WritePrivateProfileString("Settings","Fades",SafeItoa(pWindow->Fades,szTemp,10),INIFileName);
    WritePrivateProfileString("Settings","Delay",SafeItoa(pWindow->FadeDelay,szTemp,10),INIFileName);
    WritePrivateProfileString("Settings","Duration",SafeItoa(pWindow->FadeDuration,szTemp,10),INIFileName);
    WritePrivateProfileString("Settings","Alpha",SafeItoa(pWindow->Alpha,szTemp,10),INIFileName);
    WritePrivateProfileString("Settings","FadeToAlpha",SafeItoa(pWindow->FadeToAlpha,szTemp,10),INIFileName);
    WritePrivateProfileString("Settings","BGType",SafeItoa(pWindow->BGType,szTemp,10),INIFileName);
	ARGBCOLOR col = { 0 };
	col.ARGB = pWindow->BGColor;
	WritePrivateProfileString("Settings", "BGTint.alpha", SafeItoa(col.A, szTemp, 10), INIFileName);
	WritePrivateProfileString("Settings", "BGTint.red", SafeItoa(col.R, szTemp, 10), INIFileName);
	WritePrivateProfileString("Settings", "BGTint.green", SafeItoa(col.G, szTemp, 10), INIFileName);
	WritePrivateProfileString("Settings", "BGTint.blue", SafeItoa(col.B, szTemp, 10), INIFileName);
    WritePrivateProfileString("Settings","Directory",Directory,INIFileName);
}

void StartEditor() {
    // Destroy old window
    if (MyWnd) {
        delete MyWnd;
        MyWnd=0;
    }

    char s[MAX_STRING]={0};
    char filename[MAX_STRING];
    sprintf_s(filename,"%s\\%s",Directory,FileName);

    FILE *f;
	errno_t err = fopen_s(&f, filename, "r");
    if (f==NULL) {
        sprintf_s(s,"Notepad: File not found (%s)",filename);
        WriteChatColor(s,CONCOLOR_RED);
        return;
    }

    //Build Window
    DebugTry(CScreenPieceTemplate *templ=pSidlMgr->FindScreenPieceTemplate("NotepadWindow"));
    if (templ) {
        MyWnd = new CNotepadWnd;
    } else {
        return; //God forbid
    }
    ReadWindowINI((PCSIDLWND) MyWnd);
    sprintf_s(s,"Notepad - '%s'",FileName);
    SetCXStr(&(((PCSIDLWND)MyWnd)->WindowText),s);

    //Load File
    char buff[64000]={0};
    char line[2000]={0};
    while (!feof(f)) {
      if (fgets(line,1999,f)!=NULL)
         strcat_s(buff,line);
    // Check char size is less than 64k or plugin will crash
    if (strlen(buff) > 63750) {
        sprintf_s(s,"Notepad: => %s <= is too large to edit.",FileName);
        WriteChatColor(s,CONCOLOR_RED);
        delete MyWnd;
        MyWnd=0;
        return;
    }
    }
    fclose(f);
    // strip tabs
    for (unsigned int i=0;i<strlen(buff);i++) if (buff[i]=='\t') buff[i]=' ';
    SetCXStr(&(PCXSTR)MyWnd->NoteBox->InputText,buff);

}

void SaveFile() {
    char filename[MAX_STRING];
    sprintf_s(filename,"%s\\%s",Directory,FileName);
    char buff[64000]={0};
    GetCXStr((PCXSTR)MyWnd->NoteBox->InputText,buff,63999);
    FILE *f;
	errno_t err = fopen_s(&f, filename, "w");
    if (f==NULL) {
        char s[MAX_STRING]={0};
        sprintf_s(s,"Notepad: Write Error (%s)",filename);
        WriteChatColor(s,CONCOLOR_RED);
        return;
    }
    fputs(buff,f);
    fclose(f);
}

void Usage(void) {
    WriteChatColor("Sytax: /notepad <filename> - edit a file",USERCOLOR_DEFAULT);
    WriteChatColor("Sytax: /notepad dir <directory name> - set the directory",USERCOLOR_DEFAULT);
}

void Notepad(PSPAWNINFO pChar, PCHAR szLine) {
    char Arg1[MAX_STRING] = {0};
    char Arg2[MAX_STRING] = {0};
   
    GetArg(Arg1,szLine,1);
    GetArg(Arg2,szLine,2);

    if (_stricmp(Arg1,"dir")==0) {
        if (strlen(Arg2)>0) {
            strcpy_s(Directory,Arg2);
            WritePrivateProfileString("Settings","Directory",Directory,INIFileName);
        } else {
            char s[MAX_STRING]={0};
            GetPrivateProfileString("Settings","Directory",gszMacroPath,Directory,999,INIFileName);
            sprintf_s(s,"Notepad: Current Directory '%s'",Directory);
            WriteChatColor(s,USERCOLOR_DEFAULT);
        }
        return;
    }
    if (strlen(Arg1)>0) {
        GetPrivateProfileString("Settings","Directory",gszMacroPath,Directory,999,INIFileName);
        sprintf_s(FileName,"%s",Arg1);
        StartEditor();
    } else {
        Usage();
    }
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID) {
    DebugSpewAlways("Initializing MQ2Notepad");
    AddCommand("/notepad",Notepad);
    AddXMLFile("MQUI_NotepadWindow.xml");
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID) {
    DebugSpewAlways("Shutting down MQ2Notepad");
    if (MyWnd) {
        WriteWindowINI((PCSIDLWND)MyWnd);
        delete MyWnd;
        MyWnd=0;
    }
    RemoveCommand("/notepad");
    RemoveXMLFile("MQUI_NotepadWindow.xml");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID) {
    DebugSpewAlways("MQ2Notepad::OnCleanUI()");
    // destroy custom windows, etc
    if (MyWnd) {
        WriteWindowINI((PCSIDLWND)MyWnd);
        delete MyWnd;
        MyWnd=0;
    }
}