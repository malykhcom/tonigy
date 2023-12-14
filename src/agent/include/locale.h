#ifndef __LOCALE_H__
#define __LOCALE_H__

VOID localeInit(PSZ dir, PSZ lang, PSZ def);
PSZ localeGetMsg(PSZ key);
HBITMAP localeGetBitmap(PSZ key, HPS hps, PULONG x, PULONG y);
VOID localeFree();

#endif
