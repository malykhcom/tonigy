#include "global.h"

#include "tonigy.h"

#define TMSG_OK     (WM_USER+10)
#define TMSG_REDRAW (WM_USER+11)

static ULONG ButtonW;
static ULONG ButtonWM;

typedef struct
{
    RECORDCORE core;
    ULONG drive;
    PSZ letter1;
    PSZ letter2;
    PSZ status;
    PSZ autoattach;
} DRIVEREC, * PDRIVEREC;

#define DRIVES_D    10
#define DRIVES_X    10
#define DRIVES_Y    54
#define BUTTON_X    7
#define BUTTON_Y    7
#define BUTTON_H    37
#define BUTTON_D    7
#define EXIT_D      7
#define BUTTON_W    (ButtonW)
#define ATTACH_X    BUTTON_X
#define DETACH_X    (BUTTON_X+(BUTTON_W+BUTTON_D)*1)
#define SETUP_X     (BUTTON_X+(BUTTON_W+BUTTON_D)*2)
#define EXIT_X      (BUTTON_X+(BUTTON_W+BUTTON_D)*3)
//#define BUTTON_W    60
//#define ATTACH_X    7
//#define DETACH_X    74
//#define SETUP_X     141
//#define EXIT_X      209

static HBITMAP Corner;
static HBITMAP Attach1, Attach2;
static HBITMAP Detach1, Detach2;
static HBITMAP Setup1, Setup2;
static HBITMAP Exit1, Exit2;
static HBITMAP Ok1, Ok2;
static HBITMAP Cancel1, Cancel2;

static CHAR ClientClass[]="TonigyMain";
static CHAR SetupClass[]="TonigySetup";
static CHAR Font[]="8.Helv";
static CHAR Font2[]="9.WarpSans";
static CHAR Empty[]="";

static PMAINDATA Maindata;
static HWND MainWindow;
static HWND Drives;
static HWND Help;
static HWND Menu;
static ULONG SelectedDrive;

static VOID SetText(HWND hwnd, ULONG id, PCSZ text)
{
    WinSetWindowText(WinWindowFromID(hwnd, id), text);
}

static MRESULT EXPENTRY ClientProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
static MRESULT EXPENTRY DriveSetupProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
static MRESULT EXPENTRY OptionsProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
static MRESULT EXPENTRY SetupProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

static VOID OpenSetup(HWND);

static VOID FillDrive(PDRIVEREC driverec)
{
    PCHAR tmp;
    PIOPTIONS options;
    options=&Maindata->Options[driverec->drive];
    tmp=driverec->letter1;
    tmp[0]=(CHAR)(driverec->drive+Maindata->StartLetter);
    tmp[1]=':';
    tmp[2]='\0';
    if (options->Letter)
    {
        tmp=driverec->letter2;
        tmp[0]=options->Letter;
        tmp[1]=':';
        tmp[2]='\0';
        driverec->status=options->Attached?
            localeGetMsg("ui.main.attached"):
            localeGetMsg("ui.main.detached");
        driverec->autoattach=options->AutoAttach?
            localeGetMsg("ui.main.yes"):
            localeGetMsg("ui.main.no");
    }
    else
    {
        strcpy(driverec->letter2, localeGetMsg("ui.main.notselected"));
        driverec->status=Empty;
        driverec->autoattach=Empty;
    }
}

static BOOL AddDrive(HWND devicelist, ULONG drive)
{
    PDRIVEREC driverec;
    driverec=(PDRIVEREC)WinSendMsg(devicelist, CM_ALLOCRECORD,
                                   MPFROMLONG(sizeof(DRIVEREC)-sizeof(RECORDCORE)),
                                   MPFROMSHORT(1));
    if (driverec)
    {
        RECORDINSERT insert;
        driverec->drive=drive;
        driverec->letter1=(PCHAR)M_ALLOC(3);
        driverec->letter2=(PCHAR)M_ALLOC(strlen(localeGetMsg("ui.main.notselected"))+1);
        FillDrive(driverec);
        insert.cb=sizeof(RECORDINSERT);
        insert.pRecordOrder=(PRECORDCORE)CMA_END;
        insert.pRecordParent=NULL;
        insert.fInvalidateRecord=FALSE;
        insert.zOrder=CMA_TOP;
        insert.cRecordsInsert=1;
        WinSendMsg(devicelist, CM_INSERTRECORD,
                   MPFROMP(driverec),
                   MPFROMP(&insert));
        return TRUE;
    }
    return FALSE;
}

static BOOL InitDeviceList(HWND client)
{
    CNRINFO cnrinfo;
    FIELDINFO* finfo, *fnextinfo;
    FIELDINFOINSERT fin;
    ULONG t;
    Drives=WinCreateWindow(client, WC_CONTAINER, NULL,
                           CCS_READONLY | CCS_SINGLESEL | WS_VISIBLE,
                           0, 0, 0, 0,
                           client,
                           HWND_TOP,
                           0, NULL, NULL);
    if (!Drives)
        return FALSE;
    finfo=(FIELDINFO*)WinSendMsg(Drives,
                                 CM_ALLOCDETAILFIELDINFO,
                                 MPFROMLONG(4),
                                 0);
    fnextinfo=finfo;
    fnextinfo->flData=CFA_CENTER | CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR;
    fnextinfo->flTitle=0;
    fnextinfo->pTitleData=localeGetMsg("ui.main.cdletter");
    fnextinfo->offStruct=offsetof(DRIVEREC, letter1);
    
    fnextinfo=fnextinfo->pNextFieldInfo;
    fnextinfo->flData=CFA_CENTER | CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR;
    fnextinfo->flTitle=0;
    fnextinfo->pTitleData=localeGetMsg("ui.main.tonigyletter");
    fnextinfo->offStruct=offsetof(DRIVEREC, letter2);

    fnextinfo=fnextinfo->pNextFieldInfo;
    fnextinfo->flData=CFA_CENTER | CFA_STRING | CFA_HORZSEPARATOR | CFA_SEPARATOR;
    fnextinfo->flTitle=CFA_CENTER;
    fnextinfo->pTitleData=localeGetMsg("ui.main.status");
    fnextinfo->offStruct=offsetof(DRIVEREC, status);

    fnextinfo=fnextinfo->pNextFieldInfo;
    fnextinfo->flData=CFA_CENTER | CFA_STRING | CFA_HORZSEPARATOR;
    fnextinfo->flTitle=CFA_CENTER;
    fnextinfo->pTitleData=localeGetMsg("ui.main.autoattach");
    fnextinfo->offStruct=offsetof(DRIVEREC, autoattach);

    fin.cb=sizeof(FIELDINFOINSERT);
    fin.pFieldInfoOrder=(FIELDINFO*)CMA_END;
    fin.fInvalidateFieldInfo=FALSE;
    fin.cFieldInfoInsert=4;
    WinSendMsg(Drives, CM_INSERTDETAILFIELDINFO,
               finfo,
               &fin);
    for(t=0;t<Maindata->Drives;t++)
    {
        AddDrive(Drives, t);
    }
    cnrinfo.flWindowAttr=CV_DETAIL | CA_DETAILSVIEWTITLES;
    WinSendMsg(Drives, CM_SETCNRINFO,
               &cnrinfo,
               MPFROMLONG(CMA_FLWINDOWATTR));
    WinSetFocus(HWND_DESKTOP, Drives);
    return TRUE;
}

static PDRIVEREC NextDrive(PDRIVEREC driverec)
{
    USHORT cmd;
    USHORT search;
    if (driverec)
        cmd=CMA_NEXT;
    else
        cmd=CMA_FIRST;
    search=CMA_ITEMORDER;
    return (PDRIVEREC)WinSendMsg(Drives, CM_QUERYRECORD,
                                  driverec,
                                  MPFROM2SHORT(cmd, search));
}

static VOID AddButton(HWND client, ULONG id, PSZ text)
{
    WinCreateWindow(client, WC_BUTTON, text,
                    BS_PUSHBUTTON | BS_NOPOINTERFOCUS | WS_VISIBLE |
                    BS_USERBUTTON,
                    0, 0, 0, 0,
                    client, HWND_TOP,
                    id, NULL, NULL);
}

BOOL uiInit(PSZ exename, PHWND hwnd, PMAINDATA maindata)
{
    HWND client;
    ULONG flags=FCF_TITLEBAR | FCF_SYSMENU |
        FCF_SIZEBORDER | FCF_ACCELTABLE |
        FCF_NOBYTEALIGN | FCF_TASKLIST | FCF_MINBUTTON;
    HELPINIT helpinit;
    CHAR path[CCHMAXPATH];
    PCHAR name;
    HPS hps;
    WinRegisterClass(maindata->Hab, ClientClass, ClientProc,
                     CS_MOVENOTIFY | CS_SIZEREDRAW |
                     CS_SAVEBITS | CS_CLIPCHILDREN, 0);
    WinRegisterClass(maindata->Hab, SetupClass, SetupProc,
                     CS_MOVENOTIFY | CS_SIZEREDRAW |
                     CS_SAVEBITS | CS_CLIPCHILDREN, 0);
    *hwnd=WinCreateStdWindow(HWND_DESKTOP, 0,
                             &flags, ClientClass,
                             "Tonigy",
                             0, NULL,
                             ID_MAIN,
                             &client);
    if (!*hwnd)
        return FALSE;
    WinSetPresParam(*hwnd, PP_FONTNAMESIZE,
                    sizeof(Font),
                    Font);
    WinSetPresParam(*hwnd, PP_FONTNAMESIZE,
                    sizeof(Font2),
                    Font2);
    WinSetPresParam(client, PP_FONTNAMESIZE,
                    sizeof(Font),
                    Font);
    WinSetPresParam(client, PP_FONTNAMESIZE,
                    sizeof(Font2),
                    Font2);
    Maindata=maindata;
    hps=WinGetPS(client);
    Corner=GpiLoadBitmap(hps, NULL, IDB_CORNER, 0, 0);
    {
        ULONG x, y;
        ButtonW=ButtonWM=10;
        Attach1=localeGetBitmap("ui.attach.1", hps, &x, &y);
        ButtonW=max(ButtonW, x);
        Attach2=localeGetBitmap("ui.attach.2", hps, &x, &y);
        ButtonW=max(ButtonW, x);
        Detach1=localeGetBitmap("ui.detach.1", hps, &x, &y);
        Detach2=localeGetBitmap("ui.detach.2", hps, &x, &y);
        Setup1=localeGetBitmap("ui.setup.1", hps, &x, &y);
        Setup2=localeGetBitmap("ui.setup.2", hps, &x, &y);
        Exit1=localeGetBitmap("ui.exit.1", hps, &x, &y);
        Exit2=localeGetBitmap("ui.exit.2", hps, &x, &y);
        Ok1=localeGetBitmap("ui.ok.1", hps, &x, &y);
        ButtonWM=max(ButtonWM, x);
        Ok2=localeGetBitmap("ui.ok.2", hps, &x, &y);
        ButtonWM=max(ButtonWM, x);
        Cancel1=localeGetBitmap("ui.cancel.1", hps, &x, &y);
        Cancel2=localeGetBitmap("ui.cancel.2", hps, &x, &y);
    }
    WinReleasePS(hps);
    AddButton(client, ID_ATTACH, "attach");
    AddButton(client, ID_DETACH, "detach");
    AddButton(client, ID_SETUP, "setup");
    AddButton(client, ID_EXIT, "exit");
    Menu=WinLoadMenu(client, NULL, IDM_MAIN);
    //WinLoadMenu(*hwnd, NULL, IDM_MAIN);
    if (!InitDeviceList(client))
    {
        WinDestroyWindow(*hwnd);
        return FALSE;
    }
    WinSendMsg(*hwnd, WM_SETICON,
               (MPARAM)WinLoadPointer(HWND_DESKTOP, NULL, 1),
               NULL);
/*    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_ATTACH),
               MPFROMP(localeGetMsg("ui.menu.attach")));
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_DETACH),
               MPFROMP(localeGetMsg("ui.menu.detach")));
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_SETUP),
               MPFROMP(localeGetMsg("ui.menu.setup")));*/
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_EXIT),
               MPFROMP(localeGetMsg("ui.menu.exit")));
    memset(&helpinit, 0, sizeof(helpinit));
    strcpy(path, exename);
    name=strrchr(path, '\\');
    if (name)
        name++;
    else
        name=path;
    helpinit.cb=sizeof(helpinit);
    helpinit.phtHelpTable=(PHELPTABLE)MAKEULONG(ID_MAIN, 0xffff);
    helpinit.pszHelpWindowTitle=localeGetMsg("ui.help.title");
    helpinit.fShowPanelId=CMIC_HIDE_PANEL_ID;
    strcpy(name, "tonigy.inf");
    helpinit.pszHelpLibraryName=name;
    Help=WinCreateHelpInstance(maindata->Hab, &helpinit);
    if (Help && helpinit.ulReturnCode)
    {
        WinDestroyHelpInstance(Help);
        Help=NULL;
    }
    if (Help)
    {
        WinAssociateHelpInstance(Help, *hwnd);
    }
    WinSetWindowPos(*hwnd, NULL,
                    maindata->X, maindata->Y, maindata->W, maindata->H,
                    SWP_SIZE | SWP_MOVE |
                    (maindata->Min ? SWP_MINIMIZE : (SWP_SHOW | SWP_ACTIVATE)));
    MainWindow=*hwnd;
    return TRUE;
}

VOID uiTerm(HWND hwnd)
{
    PDRIVEREC driverec;
    driverec=NextDrive(NULL);
    while (driverec)
    {
        M_FREE(driverec->letter1);
        M_FREE(driverec->letter2);
        driverec=NextDrive(driverec);
    }
    if (Help)
        WinDestroyHelpInstance(Help);
    WinDestroyWindow(hwnd);
}

static VOID Box(HPS hps, LONG x1, LONG y1, LONG x2, LONG y2, LONG color)
{
    POINTL pointl;
    GpiSetColor(hps, color);
    pointl.x=x1;
    pointl.y=y1;
    GpiMove(hps, &pointl);
    pointl.x=x2;
    pointl.y=y2;
    GpiBox(hps, DRO_OUTLINE, &pointl, 0, 0);
}

static VOID Line(HPS hps, LONG x1, LONG y1, LONG x2, LONG y2, LONG color)
{
    POINTL pointl;
    GpiSetColor(hps, color);
    pointl.x=x1;
    pointl.y=y1;
    GpiMove(hps, &pointl);
    pointl.x=x2;
    pointl.y=y2;
    GpiLine(hps, &pointl);
}

static VOID EnableMenuItem(USHORT id, BOOL status)
{
    WinSendMsg(Menu, MM_SETITEMATTR,
               MPFROM2SHORT(id, TRUE),
               MPFROM2SHORT(MIA_DISABLED, status?FALSE:MIA_DISABLED));
}

static VOID EnableButtons(HWND hwnd, PIOPTIONS options)
{
    if (options->Letter)
    {
        if (options->Attached)
        {
            WinEnableWindow(WinWindowFromID(hwnd, ID_ATTACH), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, ID_DETACH), TRUE);
            EnableMenuItem(ID_ATTACH, FALSE);
            EnableMenuItem(ID_DETACH, TRUE);
        }
        else
        {
            WinEnableWindow(WinWindowFromID(hwnd, ID_ATTACH), TRUE);
            WinEnableWindow(WinWindowFromID(hwnd, ID_DETACH), FALSE);
            EnableMenuItem(ID_ATTACH, TRUE);
            EnableMenuItem(ID_DETACH, FALSE);
        }
    }
    else
    {
        WinEnableWindow(WinWindowFromID(hwnd, ID_ATTACH), FALSE);
        WinEnableWindow(WinWindowFromID(hwnd, ID_DETACH), FALSE);
        EnableMenuItem(ID_ATTACH, FALSE);
        EnableMenuItem(ID_DETACH, FALSE);
    }
}

static VOID PrepareMenu(VOID)
{
    static CHAR buffer[256];
    CHAR drive[3];
    PIOPTIONS op = &Maindata->Options[SelectedDrive];
    if (op->Letter)
    {
        drive[0] = op->Letter;
        drive[1] = ':';
        drive[2] = '\0';
    }
    else
    {
        drive[0] = '\0';
    }
    _snprintf(buffer, sizeof(buffer),
              "%s %s", localeGetMsg("ui.menu.attach"), drive);
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_ATTACH),
               MPFROMP(buffer));
    _snprintf(buffer, sizeof(buffer),
              "%s %s", localeGetMsg("ui.menu.detach"), drive);
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_DETACH),
               MPFROMP(buffer));
    _snprintf(buffer, sizeof(buffer),
              "%s %c:", localeGetMsg("ui.menu.eject"),
              Maindata->StartLetter+SelectedDrive);
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_EJECT),
               MPFROMP(buffer));
    WinSendMsg(Menu, MM_SETITEMTEXT,
               MPFROMSHORT(ID_SETUP),
               MPFROMP(localeGetMsg("ui.menu.setup")));
}

static VOID ShowMenu(HWND parent, HWND owner, LONG x, LONG y)
{
    PrepareMenu();
    WinPopupMenu(parent, owner, Menu,
                 x,
                 y,
                 0,
                 PU_HCONSTRAIN | PU_VCONSTRAIN |
                 PU_NONE | PU_KEYBOARD | PU_MOUSEBUTTON1 |
                 PU_MOUSEBUTTON2);
}

static MRESULT EXPENTRY ClientProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_SIZE:
        {
            SWP swp;
            LONG x, y;
            x=SHORT1FROMMP(mp2);
            y=SHORT2FROMMP(mp2);
            WinSetWindowPos(Drives, NULL,
                            DRIVES_D, DRIVES_Y,
                            x-DRIVES_D*2, y-DRIVES_Y-DRIVES_D-1,
                            SWP_SIZE | SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, ID_ATTACH), NULL,
                            ATTACH_X, BUTTON_Y,
                            BUTTON_W, BUTTON_H,
                            SWP_SIZE | SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, ID_DETACH), NULL,
                            DETACH_X, BUTTON_Y,
                            BUTTON_W, BUTTON_H,
                            SWP_SIZE | SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, ID_SETUP), NULL,
                            SETUP_X, BUTTON_Y,
                            BUTTON_W, BUTTON_H,
                            SWP_SIZE | SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, ID_EXIT), NULL,
                            max(x-BUTTON_W-EXIT_D, EXIT_X), BUTTON_Y,
                            BUTTON_W, BUTTON_H,
                            SWP_SIZE | SWP_MOVE);
            if (WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp))
            {
                if (!(swp.fl & SWP_MINIMIZE))
                {
                    Maindata->W=swp.cx;
                    Maindata->H=swp.cy;
                }
            }
        }
        return 0;
    case WM_PAINT:
        {
            HPS hps;
            RECTL rectl;
            SWP swp;
            hps=WinBeginPaint(hwnd, NULL, &rectl);
            WinFillRect(hps, &rectl, CLR_PALEGRAY);
            //GpiErase(hps);
            if (WinQueryWindowPos(Drives, &swp))
            {
                /*Box(hps, swp.x-2, swp.y-2, swp.x+swp.cx+1, swp.y+swp.cy+1,
                    CLR_BLACK);
                Line(hps, swp.x-1, swp.y-1, swp.x-1, swp.y+swp.cy,
                     CLR_DARKGRAY);
                Line(hps, swp.x-1, swp.y+swp.cy, swp.x+swp.cx, swp.y+swp.cy,
                     CLR_DARKGRAY);
                Line(hps, swp.x-1, swp.y-1, swp.x+swp.cx, swp.y-1,
                     CLR_WHITE);
                Line(hps, swp.x+swp.cx, swp.y-1, swp.x+swp.cx, swp.y+swp.cy,
                CLR_WHITE);*/
                Box(hps, swp.x-1, swp.y-2, swp.x+swp.cx+1, swp.y+swp.cy,
                    CLR_WHITE);
                Box(hps, swp.x-2, swp.y-1, swp.x+swp.cx, swp.y+swp.cy+1,
                    CLR_BLACK);
            }
            if (WinQueryWindowRect(hwnd, &rectl))
            {
                POINTL pointl;
                pointl.x=rectl.xRight-8;
                pointl.y=0;
                WinDrawBitmap(hps, Corner, NULL, &pointl,
                              CLR_NEUTRAL, CLR_BACKGROUND, DBM_NORMAL);
            }
            WinEndPaint(hps);
        }
        return 0;
    case TMSG_REDRAW:
        {
            PDRIVEREC driverec;
            driverec=NextDrive(NULL);
            while (driverec)
            {
                FillDrive(driverec);
                driverec=NextDrive(driverec);
            }
            WinSendMsg(Drives, CM_INVALIDATERECORD,
                       NULL,
                       MPFROM2SHORT(0, 0));
            EnableButtons(hwnd, &Maindata->Options[SelectedDrive]);
        }
        return 0;
    case TMSG_ATTACH:
        {
            ULONG drive;
            PIOPTIONS options;
            drive=LONGFROMMP(mp1);
            options=&Maindata->Options[drive];
            if (options->Letter &&
                !options->Attached)
            {
                if (utilAttach(Maindata, drive))
                {
                    WinSendMsg(hwnd, TMSG_REDRAW, 0, 0);
                    return 0;
                }
            }
        }
        return MRFROMLONG(1);
    case TMSG_DETACH:
        {
            ULONG drive;
            PIOPTIONS options;
            drive=LONGFROMMP(mp1);
            options=&Maindata->Options[drive];
            if (options->Letter &&
                options->Attached)
            {
                if (utilDetach(Maindata, drive))
                {
                    WinSendMsg(hwnd, TMSG_REDRAW, 0, 0);
                    return 0;
                }
            }
        }
        return MRFROMLONG(1);
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case ID_ATTACH:
            {
                PIOPTIONS options;
                options=&Maindata->Options[SelectedDrive];
                if (options->Letter &&
                    !options->Attached)
                {
                    if (utilAttach(Maindata, SelectedDrive))
                    {
                        WinSendMsg(hwnd, TMSG_REDRAW, 0, 0);
                    }
                    else
                        errAttach(hwnd, options->Letter);
                }
            }
            return 0;
        case ID_DETACH:
            {
                PIOPTIONS options;
                options=&Maindata->Options[SelectedDrive];
                if (options->Letter &&
                    options->Attached)
                {
                    if (utilDetach(Maindata, SelectedDrive))
                    {
                        WinSendMsg(hwnd, TMSG_REDRAW, 0, 0);
                    }
                    else
                        errDetach(hwnd, options->Letter);
                }
            }
            return 0;
        case ID_EJECT:
            utilEject(Maindata, SelectedDrive);
            return 0;
        case ID_SETUP:
            OpenSetup(hwnd);
            return 0;
        case ID_EXIT:
            WinSendMsg(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
        break;
    case WM_CONTROL:
        switch (SHORT2FROMMP(mp1))
        {
        case CN_EMPHASIS:
            {
                PNOTIFYRECORDEMPHASIS tmp;
                tmp=(PNOTIFYRECORDEMPHASIS)mp2;
                if (tmp->fEmphasisMask & CRA_CURSORED)
                {
                    PDRIVEREC driverec;
                    driverec=(PDRIVEREC)tmp->pRecord;
                    if (driverec->core.flRecordAttr & CRA_CURSORED)
                    {
                        SelectedDrive=driverec->drive;
                        EnableButtons(hwnd,
                                      &Maindata->Options[driverec->drive]);
                    }
                }
            }
            return 0;
        case CN_ENTER:
            WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_SETUP), NULL);
            return 0;
        case CN_HELP:
            WinSendMsg(Help, HM_EXT_HELP, 0, 0);
            return 0;
        case CN_CONTEXTMENU:
            {
                POINTL ptl;
                WinQueryPointerPos(HWND_DESKTOP, &ptl);
                ShowMenu(HWND_DESKTOP, hwnd, ptl.x, ptl.y);
            }
            return 0;
        case BN_PAINT:
            {
                switch (SHORT1FROMMP(mp1))
                {
                case ID_ATTACH:
                    buttonDraw((PUSERBUTTON)PVOIDFROMMP(mp2),
                               Attach1, Attach2);
                    break;
                case ID_DETACH:
                    buttonDraw((PUSERBUTTON)PVOIDFROMMP(mp2),
                               Detach1, Detach2);
                    break;
                case ID_SETUP:
                    buttonDraw((PUSERBUTTON)PVOIDFROMMP(mp2),
                               Setup1, Setup2);
                    break;
                case ID_EXIT:
                    buttonDraw((PUSERBUTTON)PVOIDFROMMP(mp2),
                               Exit1, Exit2);
                    break;
                }
            }
            return 0;
        }
        break;
    case WM_MOVE:
        {
            SWP swp;
            if (WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp))
            {
                if (!(swp.fl & SWP_MINIMIZE))
                {
                    Maindata->X=swp.x;
                    Maindata->Y=swp.y;
                    Maindata->Min=FALSE;
                }
                else
                    Maindata->Min=TRUE;
            }
        }
        return 0;
    case WM_BUTTON1DOWN:
        WinSendMsg(WinQueryWindow(hwnd, QW_PARENT), WM_TRACKFRAME,
                   (MPARAM)TF_MOVE, 0);
        return 0;
    case WM_BUTTON2UP:
        {
            SWP swp;
            if (WinQueryWindowPos(Drives, &swp))
            {
                POINTL ptl;
                RECTL rtl;
                ptl.x=SHORT1FROMMP(mp1);
                ptl.y=SHORT2FROMMP(mp1);
                rtl.xLeft=swp.x;
                rtl.yBottom=swp.y;
                rtl.xRight=swp.x+swp.cx;
                rtl.yTop=swp.y+swp.cy;
                if (!WinPtInRect(Maindata->Hab, &rtl, &ptl))
                    ShowMenu(hwnd, hwnd, ptl.x, ptl.y);
            }
        }
        return 0;
    }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
}

/////////

static HWND Notebook;
static ULONG Current;

static VOID AJEnable(HWND hwnd, BOOL enable)
{
    WinEnableWindow(WinWindowFromID(hwnd, ID_PREREAD), enable);
    WinEnableWindow(WinWindowFromID(hwnd, ID_PREREAD_), enable);
    WinEnableWindow(WinWindowFromID(hwnd, ID_TRIES), enable);
    WinEnableWindow(WinWindowFromID(hwnd, ID_TRIES_), enable);
}

static ULONG Tries[]=
{
    1, 2, 3, 4, 5, 10, 20, 40
};

MRESULT EXPENTRY DriveSetupProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            static CHAR tmp[40];
            ULONG drivemap;
            ULONG t;
            PIOPTIONS options;
            WinSetWindowULong(hwnd, QWL_USER, Current);
            WinSetPresParam(hwnd, PP_FONTNAMESIZE,
                            sizeof(Font),
                            Font);
            WinSetPresParam(hwnd, PP_FONTNAMESIZE,
                            sizeof(Font2),
                            Font2);
            SetText(hwnd, ID_TONIGY_, localeGetMsg("ui.setup.drive.tonigy"));
            SetText(hwnd, ID_LETTER_, localeGetMsg("ui.setup.drive.letter"));
            SetText(hwnd, ID_AUTOATTACH, localeGetMsg("ui.setup.drive.autoattach"));
            SetText(hwnd, ID_LOG, localeGetMsg("ui.setup.drive.log"));
            SetText(hwnd, ID_READING_, localeGetMsg("ui.setup.drive.audio"));
            SetText(hwnd, ID_AJ, localeGetMsg("ui.setup.drive.aj"));
            SetText(hwnd, ID_PREREAD_, localeGetMsg("ui.setup.drive.preread"));
            SetText(hwnd, ID_TRIES_, localeGetMsg("ui.setup.drive.tries"));
            //SetText(hwnd, ID_CANCEL, localeGetMsg("ui.setup.drive."));
            if (DosQueryCurrentDisk(&t, &drivemap))
                drivemap=0;
            options=&Maindata->Options[Current];
            WinSendDlgItemMsg(hwnd, ID_LETTER, LM_INSERTITEM,
                              MPFROM2SHORT(LIT_END, 0),
                              localeGetMsg("ui.setup.drive.notselected"));
            for(t='A';t<='Z';t++)
            {
                if (drivemap & 1 &&
                    (t!=options->Letter ||
                     !options->Attached))
                    _snprintf(tmp, sizeof(tmp), "%c: (%s)", t,
                              localeGetMsg("ui.setup.drive.busy"));
                else
                    _snprintf(tmp, sizeof(tmp), "%c:", t);
                WinSendDlgItemMsg(hwnd, ID_LETTER, LM_INSERTITEM,
                                  MPFROM2SHORT(LIT_END, 0), tmp);
                drivemap>>=1;
            }
            t=options->Letter;
            if (t>='A' && t<='Z')
                WinSendDlgItemMsg(hwnd, ID_LETTER, LM_SELECTITEM,
                                  MPFROM2SHORT(t-'A'+1, 0),
                                  MPFROM2SHORT(TRUE, 0));
            else
                WinSendDlgItemMsg(hwnd, ID_LETTER, LM_SELECTITEM,
                                  MPFROM2SHORT(0, 0),
                                  MPFROM2SHORT(TRUE, 0));
            WinSendDlgItemMsg(hwnd, ID_AUTOATTACH, BM_SETCHECK,
                              MPFROM2SHORT(options->AutoAttach?1:0, 0),
                              NULL);
            WinSendDlgItemMsg(hwnd, ID_LOG, BM_SETCHECK,
                              MPFROM2SHORT(options->Log?1:0, 0),
                              NULL);
            WinSendDlgItemMsg(hwnd, ID_PREREAD, LM_INSERTITEM,
                              MPFROM2SHORT(LIT_END, 0), localeGetMsg("ui.setup.autodetect"));
            WinSendDlgItemMsg(hwnd, ID_PREREAD, LM_INSERTITEM,
                              MPFROM2SHORT(LIT_END, 0), localeGetMsg("ui.setup.drive.sector"));
            for(t=2;t<=10;t++)
            {
                _snprintf(tmp, sizeof(tmp), "%u %s", t, localeGetMsg("ui.setup.drive.sectors"));
                WinSendDlgItemMsg(hwnd, ID_PREREAD, LM_INSERTITEM,
                                  MPFROM2SHORT(LIT_END, 0), tmp);
            }
            t=min(options->Options.Preread, 10);
            WinSendDlgItemMsg(hwnd, ID_PREREAD, LM_SELECTITEM,
                              MPFROM2SHORT(t, 0),
                              MPFROM2SHORT(TRUE, 0));
            for(t=0;t<sizeof(Tries)/sizeof(Tries[0]);t++)
            {
                _snprintf(tmp, sizeof(tmp), "%u", Tries[t]);
                WinSendDlgItemMsg(hwnd, ID_TRIES, LM_INSERTITEM,
                                  MPFROM2SHORT(LIT_END, 0), tmp);
            }
            for(t=0;t<sizeof(Tries)/sizeof(Tries[0]);t++)
            {
                if (options->Options.Tries<=Tries[t])
                    break;
            }
            t=min(t, sizeof(Tries)/sizeof(Tries[0])-1);
            WinSendDlgItemMsg(hwnd, ID_TRIES, LM_SELECTITEM,
                              MPFROM2SHORT(t, 0),
                              MPFROM2SHORT(TRUE, 0));
            WinSendDlgItemMsg(hwnd, ID_AJ, BM_SETCHECK,
                              MPFROM2SHORT(options->Options.Antijitter?1:0, 0),
                              NULL);
            AJEnable(hwnd, options->Options.Antijitter);
        }
        return 0;
    case WM_CONTROL:
        switch(SHORT1FROMMP(mp1))
        {
        case ID_AJ:
            if (WinSendDlgItemMsg(hwnd, ID_AJ, BM_QUERYCHECK, NULL, NULL))
                AJEnable(hwnd, TRUE);
            else
                AJEnable(hwnd, FALSE);
            break;
        }
        return 0;
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case DID_CANCEL:
            return 0;
        }
        break;
    case TMSG_OK:
        {
            ULONG t;
            PIOPTIONS options;
            ULONG drive=WinQueryWindowULong(hwnd, QWL_USER);
            options=&Maindata->Options[drive];
            options->AutoAttach=WinSendDlgItemMsg(hwnd, ID_AUTOATTACH,
                                                  BM_QUERYCHECK,
                                                  NULL, NULL)?TRUE:FALSE;
            DosRequestMutexSem(Maindata->OptionsMutex, SEM_INDEFINITE_WAIT);
            options->Log=WinSendDlgItemMsg(hwnd, ID_LOG,
                                           BM_QUERYCHECK,
                                           NULL, NULL)?TRUE:FALSE;
            options->Options.Antijitter=WinSendDlgItemMsg(hwnd, ID_AJ,
                                                          BM_QUERYCHECK,
                                                          NULL, NULL)?TRUE:FALSE;
            t=(ULONG)WinSendDlgItemMsg(hwnd, ID_PREREAD,
                                       LM_QUERYSELECTION,
                                       MPFROM2SHORT(LIT_CURSOR, 0), NULL);
            options->Options.Preread=t;
            t=(ULONG)WinSendDlgItemMsg(hwnd, ID_TRIES,
                                       LM_QUERYSELECTION,
                                       MPFROM2SHORT(LIT_CURSOR, 0), NULL);
            options->Options.Tries=Tries[t];
            DosReleaseMutexSem(Maindata->OptionsMutex);
            t=(ULONG)WinSendDlgItemMsg(hwnd, ID_LETTER, LM_QUERYSELECTION,
                                       MPFROM2SHORT(LIT_CURSOR, 0), NULL);
            if (t)
                t=(CHAR)(t-1+'A');
            if (t!=options->Letter)
            {
                if (options->Letter && options->Attached)
                {
                    if (!utilDetach(Maindata, drive))
                    {
                        errDetach(hwnd, options->Letter);
                    }
                }
                options->Letter=(CHAR)t;
            }
        }
        return 0;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY OptionsProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        {
            ULONG t;
            WinSetPresParam(hwnd, PP_FONTNAMESIZE,
                            sizeof(Font),
                            Font);
            WinSetPresParam(hwnd, PP_FONTNAMESIZE,
                            sizeof(Font2),
                            Font2);
            SetText(hwnd, ID_CDDRIVES_, localeGetMsg("ui.setup.general.drives"));
            SetText(hwnd, ID_CDLETTER_, localeGetMsg("ui.setup.general.firstletter"));
            SetText(hwnd, ID_READING_, localeGetMsg("ui.setup.general.reading"));
            SetText(hwnd, ID_HIPRIO, localeGetMsg("ui.setup.general.hiprio"));
            SetText(hwnd, ID_LANG_, localeGetMsg("ui.setup.general.language"));
            SetText(hwnd, ID_ENGLISH, localeGetMsg("ui.english"));
            SetText(hwnd, ID_GERMAN, localeGetMsg("ui.german"));
            SetText(hwnd, ID_RUSSIAN, localeGetMsg("ui.russian"));
            SetText(hwnd, ID_APPLY_, localeGetMsg("ui.setup.general.apply"));
            SetText(hwnd, ID_APPLY2_, localeGetMsg("ui.setup.general.apply2"));
            //SetText(hwnd, ID_CANCEL, );
            WinSendDlgItemMsg(hwnd, ID_CDLETTER, LM_INSERTITEM,
                              MPFROM2SHORT(LIT_END, 0), localeGetMsg("ui.setup.autodetect"));
            for(t='A';t<='Z'+1-Maindata->Drives;t++)
            {
                CHAR tmp[40];
                _snprintf(tmp, sizeof(tmp), "%c:", t);
                WinSendDlgItemMsg(hwnd, ID_CDLETTER, LM_INSERTITEM,
                                  MPFROM2SHORT(LIT_END, 0), tmp);
            }
            WinSendDlgItemMsg(hwnd, ID_HIPRIO, BM_SETCHECK,
                              MPFROM2SHORT(Maindata->HighPrio?1:0, 0),
                              NULL);
            t=Maindata->FirstLetter;
            if (t>='A' && t<='Z')
                WinSendDlgItemMsg(hwnd, ID_CDLETTER, LM_SELECTITEM,
                                  MPFROM2SHORT(t-'A'+1, 0),
                                  MPFROM2SHORT(TRUE, 0));
            else
                WinSendDlgItemMsg(hwnd, ID_CDLETTER, LM_SELECTITEM,
                                  MPFROM2SHORT(0, 0),
                                  MPFROM2SHORT(TRUE, 0));
            switch (Maindata->Lang)
            {
            case 0:
                t=ID_ENGLISH;
                break;
            case 1:
                t=ID_GERMAN;
                break;
            case 2:
                t=ID_RUSSIAN;
                break;
            }
            WinSendDlgItemMsg(hwnd,
                              t,
                              BM_SETCHECK,
                              MPFROM2SHORT(1, 0),
                              NULL);
        }
        return 0;
    case TMSG_OK:
        {
            ULONG t;
            BOOL highprio;
            highprio=WinSendDlgItemMsg(hwnd, ID_HIPRIO,
                                       BM_QUERYCHECK,
                                       NULL, NULL)?TRUE:FALSE;
            if (highprio!=Maindata->HighPrio)
            {
                utilPrio(Maindata->Agent, highprio);
                Maindata->HighPrio=highprio;
            }
            if (WinSendDlgItemMsg(hwnd, ID_ENGLISH,
                                  BM_QUERYCHECK,
                                  NULL, NULL))
                Maindata->Lang=0;
            if (WinSendDlgItemMsg(hwnd, ID_GERMAN,
                                  BM_QUERYCHECK,
                                  NULL, NULL))
                Maindata->Lang=1;
            if (WinSendDlgItemMsg(hwnd, ID_RUSSIAN,
                                   BM_QUERYCHECK,
                                   NULL, NULL))
                Maindata->Lang=2;
            t=(ULONG)WinSendDlgItemMsg(hwnd, ID_CDLETTER, LM_QUERYSELECTION,
                                       MPFROM2SHORT(LIT_CURSOR, 0), NULL);
            if (t)
                t=(CHAR)(t-1+'A');
            if (t!=Maindata->FirstLetter)
            {
                Maindata->FirstLetter=t;
                if (t)
                    Maindata->StartLetter=t;
                else
                    Maindata->StartLetter=Maindata->AutoCDLetter;
                for(t=0;t<Maindata->Drives;t++)
                {
                    PIOPTIONS options;
                    options=&Maindata->Options[t];
                    if (options->Letter && options->Attached)
                    {
                        if (!utilDetach(Maindata, t))
                        {
                            errDetach(hwnd, options->Letter);
                        }
                    }
                }
            }
        }
        return 0;
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case DID_CANCEL:
            return 0;
        }
        break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

static HWND SetupHwnd=NULL;

static BOOL SizeTabs(HWND hwndNb)
{
    HPS hpsTemp;
    SIZEL szlMax;
    ULONG ulMajor;
    ULONG ulPage;
    CHAR achText[256];
    BOOKTEXT btText;
    POINTL aptlBox[TXTBOX_COUNT];
    ULONG ulStyle;
    hpsTemp=WinGetPS(hwndNb);
    szlMax.cx=0;
    szlMax.cy=0;
    ulMajor=LONGFROMMR(WinSendMsg(hwndNb,
                                  BKM_QUERYPAGEID,
                                  MPFROMLONG(0),
                                  MPFROM2SHORT(BKA_FIRST,BKA_MAJOR)));
    while (ulMajor!=0)
    {
        ulPage=ulMajor;

        while (ulPage!=0)
        {
            btText.pString=achText;
            btText.textLen=sizeof(achText);

            WinSendMsg(hwndNb,
                       BKM_QUERYTABTEXT,
                       MPFROMLONG(ulPage),
                       MPFROMP(&btText));

            GpiQueryTextBox(hpsTemp,
                            strlen(btText.pString),
                            btText.pString,
                            TXTBOX_COUNT,
                            aptlBox);
            aptlBox[TXTBOX_TOPRIGHT].x-=aptlBox[TXTBOX_BOTTOMLEFT].x;
            aptlBox[TXTBOX_TOPRIGHT].y-=aptlBox[TXTBOX_BOTTOMLEFT].y;
            szlMax.cx=max(szlMax.cx,aptlBox[TXTBOX_TOPRIGHT].x);
            szlMax.cy=max(szlMax.cy,aptlBox[TXTBOX_TOPRIGHT].y);
            ulPage=LONGFROMMR(WinSendMsg(hwndNb,
                                         BKM_QUERYPAGEID,
                                         MPFROMLONG(ulPage),
                                         MPFROM2SHORT(BKA_NEXT,0)));
            if (ulPage!=0)
            {
                ulStyle=LONGFROMMR(WinSendMsg(hwndNb,
                                              BKM_QUERYPAGESTYLE,
                                              MPFROMLONG(ulPage),
                                              0));
                if ((ulStyle & BKA_MAJOR)!=0)
                {
                    ulPage=0;
                } /* endif */
            } /* endif */
        } /* endwhile */
        ulMajor=LONGFROMMR(WinSendMsg(hwndNb,
                                      BKM_QUERYPAGEID,
                                      MPFROMLONG(ulMajor),
                                      MPFROM2SHORT(BKA_NEXT,BKA_MAJOR)));
    } /* endwhile */
    WinReleasePS(hpsTemp);
    WinSendMsg(hwndNb,
               BKM_SETDIMENSIONS,
               MPFROM2SHORT(szlMax.cx*4/3,szlMax.cy*3/2),
               MPFROMSHORT(BKA_MAJORTAB));
    return TRUE;
}

static LONG PageX, PageY;

static ULONG AddPage(PCSZ tabtext, HWND hwnd)
{
    ULONG pageid;
    SWP swp;
    pageid=(ULONG)WinSendMsg(Notebook, BKM_INSERTPAGE,
                             NULL,
                             MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_MAJOR,
                                          BKA_LAST));
    WinSendMsg(Notebook, BKM_SETTABTEXT, (MPARAM)pageid,
               MPFROMP(tabtext));
    WinQueryWindowPos(hwnd, &swp);
    //printf("%ux%u\n", swp.cx, swp.cy);
    PageX=max(PageX, swp.cx);
    PageY=max(PageY, swp.cy);
    WinSendMsg(Notebook, BKM_SETPAGEWINDOWHWND, (MPARAM)pageid,
               MPFROMHWND(hwnd));
    return pageid;
}

static VOID OkPages(VOID)
{
    ULONG pageid;
    pageid=(ULONG)WinSendMsg(Notebook, BKM_QUERYPAGEID,
                             0,
                             MPFROM2SHORT(BKA_FIRST, BKA_MAJOR));
    while (pageid)
    {
        HWND hwnd;
        hwnd=(HWND)WinSendMsg(Notebook, BKM_QUERYPAGEWINDOWHWND,
                              (MPARAM)pageid,
                              0);
        WinSendMsg(hwnd, TMSG_OK, 0, 0);
        pageid=(ULONG)WinSendMsg(Notebook, BKM_QUERYPAGEID,
                                 (MPARAM)pageid,
                                 MPFROM2SHORT(BKA_NEXT, BKA_MAJOR));
    }
    WinSendMsg(MainWindow, TMSG_REDRAW, 0, 0);
    WinDestroyWindow(SetupHwnd);
    SetupHwnd=NULL;
}

static VOID CancelPages(VOID)
{
    WinDestroyWindow(SetupHwnd);
    SetupHwnd=NULL;
}

MRESULT EXPENTRY SetupProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_CREATE:
        {
            PDLGTEMPLATE pdlg;
            HWND tmp;
            CHAR text[40];
            ULONG t;
            ULONG selectedpage;
            WinCreateWindow(hwnd, WC_BUTTON, "",
                            BS_PUSHBUTTON | WS_VISIBLE | BS_USERBUTTON |
                            BS_NOPOINTERFOCUS,
                            0, 0, 0, 0,
                            hwnd, HWND_TOP, ID_OK, NULL, NULL);
            WinCreateWindow(hwnd, WC_BUTTON, "",
                            BS_PUSHBUTTON | WS_VISIBLE | BS_USERBUTTON |
                            BS_NOPOINTERFOCUS,
                            0, 0, 0, 0,
                            hwnd, HWND_TOP, ID_CANCEL, NULL, NULL);
            Notebook=WinCreateWindow(hwnd, WC_NOTEBOOK,
                                     NULL, BKS_SPIRALBIND |
                                     0x00000800 |
                                     /*BKS_TABBEDDIALOG |*/ WS_VISIBLE,
                                     0, 0, 0, 0,
                                     hwnd, HWND_TOP,
                                     0, NULL, NULL);
            WinSetPresParam(Notebook, PP_FONTNAMESIZE,
                            sizeof(Font),
                            Font);
            WinSetPresParam(Notebook, PP_FONTNAMESIZE,
                            sizeof(Font2),
                            Font2);
            pdlg=(PDLGTEMPLATE)M_ALLOC(sizeof(DLGTEMPLATE));
            DosGetResource(NULL, RT_DIALOG, IDD_SETUP, (PPVOID)&pdlg);
            PageX=PageY=0;
            for(t=0;t<Maindata->Drives;t++)
            {
                Current=t;
                tmp=WinCreateDlg(hwnd, NULL, DriveSetupProc, pdlg, NULL);
                _snprintf(text, sizeof(text), localeGetMsg("ui.setup.drive.title"),
                          (CHAR)(t+Maindata->StartLetter));
                if (t==SelectedDrive)
                    selectedpage=AddPage(text, tmp);
                else
                    AddPage(text, tmp);
            }
            DosGetResource(NULL, RT_DIALOG, IDD_OPTIONS, (PPVOID)&pdlg);
            tmp=WinCreateDlg(hwnd, NULL, OptionsProc, pdlg, NULL);
            AddPage(localeGetMsg("ui.setup.general.title"), tmp);
            M_FREE(pdlg);
            WinSendMsg(Notebook, BKM_TURNTOPAGE, (MPARAM)selectedpage, 0);
            SizeTabs(Notebook);
            WinSendMsg(Notebook, BKM_SETNOTEBOOKCOLORS,
                       MPFROMLONG(SYSCLR_DIALOGBACKGROUND),
                       MPFROMLONG(BKA_BACKGROUNDPAGECOLORINDEX));
            {
                RECTL rect;
                rect.xLeft=0;
                rect.xRight=PageX;
                rect.yBottom=0;
                rect.yTop=PageY;
                WinSendMsg(Notebook, BKM_CALCPAGERECT,
                           MPFROMP(&rect),
                           (MPARAM)FALSE);
                PageX=rect.xRight-rect.xLeft;
                PageY=rect.yTop-rect.yBottom+DRIVES_Y;
            }
        }
        return 0;
    case WM_SIZE:
        {
            LONG x, y;
            x=SHORT1FROMMP(mp2);
            y=SHORT2FROMMP(mp2);
            WinSetWindowPos(Notebook, NULL,
                            0, DRIVES_Y,
                            x, y-DRIVES_Y, SWP_SIZE | SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, ID_OK), NULL,
                            x/2-ButtonWM-16, BUTTON_Y,
                            ButtonWM, BUTTON_H, SWP_SIZE | SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, ID_CANCEL), NULL,
                            x/2+16, BUTTON_Y,
                            ButtonWM, BUTTON_H, SWP_SIZE | SWP_MOVE);
        }
        return 0;
    case WM_MOVE:
        {
            SWP swp;
            if (WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp))
            {
                if (!(swp.fl & SWP_MINIMIZE))
                {
                    Maindata->SetupX=swp.x;
                    Maindata->SetupY=swp.y;
                }
            }
        }
        return 0;
    case WM_PAINT:
        {
            HPS hps;
            RECTL rectl;
            hps=WinBeginPaint(hwnd, NULL, &rectl);
            WinFillRect(hps, &rectl, CLR_PALEGRAY);
            WinEndPaint(hps);
        }
        return 0;
    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case ID_OK:
            OkPages();
            break;
        case ID_CANCEL:
            CancelPages();
            break;
        }
        return 0;
    case WM_CONTROL:
        switch (SHORT2FROMMP(mp1))
        {
        case BN_PAINT:
            switch (SHORT1FROMMP(mp1))
            {
            case ID_OK:
                buttonDraw((PUSERBUTTON)PVOIDFROMMP(mp2),
                           Ok1, Ok2);
                break;
            case ID_CANCEL:
                buttonDraw((PUSERBUTTON)PVOIDFROMMP(mp2),
                           Cancel1, Cancel2);
                break;
            }
            return 0;
        }
        break;
    case WM_CLOSE:
        CancelPages();
        return 0;
    }
    return WinDefWindowProc(hwnd, msg, mp1, mp2);
}

static VOID OpenSetup(HWND hwnd)
{
    if (!SetupHwnd)
    {
        ULONG flags=FCF_TITLEBAR | FCF_SYSMENU | FCF_DLGBORDER |
            FCF_TASKLIST | FCF_MINBUTTON;
        HWND setuphwndclient;
        RECTL rect;
        SetupHwnd=WinCreateStdWindow(HWND_DESKTOP, 0,
                                     &flags, SetupClass,
                                     localeGetMsg("ui.setup.title"),
                                     0, NULL,
                                     ID_SETUP, &setuphwndclient);
        if (Help)
        {
            WinAssociateHelpInstance(Help, SetupHwnd);
        }
        rect.xLeft=0;
        rect.xRight=PageX;
        rect.yBottom=0;
        rect.yTop=PageY;
        WinCalcFrameRect(SetupHwnd, &rect, FALSE);
        WinSetWindowPos(SetupHwnd, NULL,
                        Maindata->SetupX, Maindata->SetupY,
                        rect.xRight-rect.xLeft, rect.yTop-rect.yBottom,
                        SWP_SHOW | SWP_SIZE | SWP_MOVE);
    }
    else
    {
        WinSetWindowPos(SetupHwnd, NULL, 0, 0, 0, 0,
                        SWP_RESTORE | SWP_SHOW);
    }
    WinSetFocus(HWND_DESKTOP, Notebook);
}