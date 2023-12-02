#include "global.h"

VOID buttonDraw(PUSERBUTTON userbutton, HBITMAP bitmap1, HBITMAP bitmap2)
{
    POINTL point;
    HBITMAP bitmap;
    ULONG fl;
    point.x=0;
    point.y=0;
    switch (userbutton->fsState)
    {
    case BDS_DISABLED:
        bitmap=bitmap1;
        fl=DBM_HALFTONE;
        break;
    case BDS_HILITED:
        bitmap=bitmap2;
        fl=DBM_NORMAL;
        break;
    case BDS_DEFAULT:
    default:
        bitmap=bitmap1;
        fl=DBM_NORMAL;
    }
    WinDrawBitmap(userbutton->hps, bitmap, NULL, &point,
                  CLR_NEUTRAL, CLR_BACKGROUND, fl);
}
