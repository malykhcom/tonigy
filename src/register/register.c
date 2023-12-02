#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include "rsaeuro.h"
#include "register.h"

static CHAR Font[]="8.Helv";
static CHAR Font2[]="6.System VIO";

static CHAR KeyFile[CCHMAXPATH];

static VOID Box(PSZ text, BOOL error)
{
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                  text, "Tonigy - Register", 0,
                  MB_OK | MB_MOVEABLE | (error?MB_ERROR:0));
}

static MRESULT EXPENTRY RegisterProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            LONG cx, cy;
            SWP swp;
            cx=WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
            cy=WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
            WinQueryWindowPos(hwnd, &swp);
            WinSetWindowPos(hwnd, NULL, (cx-swp.cx)/2, (cy-swp.cy)/2,
                            0, 0, SWP_MOVE);
            WinSetPresParam(hwnd, PP_FONTNAMESIZE,
                            sizeof(Font),
                            Font);
            WinSetPresParam(WinWindowFromID(hwnd, ID_KEY), PP_FONTNAMESIZE,
                            sizeof(Font2),
                            Font2);
        }
        return 0;
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case ID_OK:
            {
                static CHAR tmp[200];
                static CHAR tmp2[200];
                ULONG size;
                ULONG t;
                UINT ptr;
                size=WinQueryWindowText(WinWindowFromID(hwnd, ID_KEY),
                                        sizeof(tmp), tmp);
                ptr=0;
                for(t=0;t<size;t++)
                {
                    CHAR chr;
                    chr=tmp[t];
                    if (chr=='=' ||
                        chr=='+' ||
                        chr=='/' ||
                        (chr>='0' && chr<='9') ||
                        (chr>='A' && chr<='Z') ||
                        (chr>='a' && chr<='z'))
                    {
                        tmp2[ptr]=chr;
                        ptr++;
                    }
                }
                tmp2[ptr]='\0';
                if (ptr!=88 ||
                    R_DecodePEMBlock(tmp, &ptr, tmp2, 88) ||
                    ptr!=64)
                    Box("Incorrect key. Please reenter.", TRUE);
                else
                {
                    FILE* key;
                    key=fopen(KeyFile, "wb");
                    if (key)
                    {
                        fwrite(tmp, 1, 64, key);
                        fclose(key);
                        Box("The key was stored. You should restart Tonigy.", FALSE);
                        WinDismissDlg(hwnd, TRUE);
                    }
                    else
                        Box("Tonigy.key file cannot be opened.", TRUE);
                }
            }
            break;
        case ID_CANCEL:
            WinDismissDlg(hwnd, FALSE);
            break;
        }
        return 0;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

int main(int argc, char* argv[])
{
    HAB hab;
    HMQ hmq;
    PSZ tmp;
    memset(KeyFile, 0, sizeof(KeyFile));
    strcpy(KeyFile, argv[0]);
    tmp=strrchr(KeyFile, '\\');
    if (!tmp)
        tmp=KeyFile;
    else
        tmp++;
    strcpy(tmp, "tonigy.key");
    hab=WinInitialize(0);
    if (!hab)
    {
        DosBeep(1000, 10);
        return 1;
    }
    hmq=WinCreateMsgQueue(hab, 0);
    if (!hmq)
    {
        WinTerminate(hab);
        DosBeep(1000, 10);
        return 2;
    }
    WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, RegisterProc,
              NULL, IDD_REGISTER, NULL);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}
