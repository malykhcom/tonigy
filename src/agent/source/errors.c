#include "global.h"
#include "tonigy.h"

static CHAR Error[128];

VOID errMsg(HWND hwnd, PSZ error)
{
    WinMessageBox(HWND_DESKTOP, hwnd, error,
                  localeGetMsg("error.title"),
                  0, MB_ERROR | MB_OK | MB_MOVEABLE);
}

VOID errMemory(HWND hwnd)
{
    errMsg(hwnd, localeGetMsg("error.memory"));
}

VOID errAttach(HWND hwnd, CHAR letter)
{
    _snprintf(Error, sizeof(Error),
              localeGetMsg("error.attach"),
              letter);
    WinMessageBox(HWND_DESKTOP, hwnd, Error,
                  localeGetMsg("error.title"),
                  IDD_ATTACH, MB_ERROR | MB_OK | MB_MOVEABLE | MB_HELP);
    //errMsg(hwnd, Error);
}

VOID errDetach(HWND hwnd, CHAR letter)
{
    _snprintf(Error, sizeof(Error),
              localeGetMsg("error.detach"),
              letter);
    errMsg(hwnd, Error);
}
