#include "global.h"

#define MESSAGES_NUMBER 2048

typedef struct _MESSAGE
{
    PSZ key;
    PSZ message;
} MESSAGE;

static MESSAGE CurrentMessages[MESSAGES_NUMBER];
static ULONG CurrentLen;
static MESSAGE DefaultMessages[MESSAGES_NUMBER];
static ULONG DefaultLen;

static CHAR LocaleDir[2048];

#define STATE_BEFORE_KEY 0
#define STATE_KEY        1
#define STATE_AFTER_KEY  2
#define STATE_VALUE      3
#define STATE_VALUE_SLASH 4
#define STATE_COMMENT    5

static BOOL isSpace(CHAR chr)
{
    if (chr==' ' || chr=='\t')
        return TRUE;
    else
        return FALSE;
}

static BOOL isNewLine(CHAR chr)
{
    if (chr=='\r' || chr=='\n')
        return TRUE;
    else
        return FALSE;
}

static PSZ copyString(PSZ string)
{
    PSZ ret=M_ALLOC(strlen(string)+1);
    strcpy(ret, string);
    return ret;
}

#define DONE_VALUE \
{ \
    buffer[ptr]='\0'; \
    tmp.message=copyString(buffer); \
    messages[counter]=tmp; \
    counter++; \
} \

static ULONG parse(PSZ data, MESSAGE messages[])
{
    ULONG state=STATE_BEFORE_KEY;
    static CHAR buffer[10000];
    ULONG ptr;
    MESSAGE tmp;
    ULONG counter=0;
    for (;;)
    {
        CHAR chr=*data;
        data++;
        if (chr=='\0')
            break;
        switch (state)
        {
        case STATE_BEFORE_KEY:
            if (!isSpace(chr) && !isNewLine(chr))
            {
                buffer[0]=chr;
                ptr=1;
                state=STATE_KEY;
            }
            if (chr=='#')
                state=STATE_COMMENT;
            break;
        case STATE_KEY:
            if (isSpace(chr) || chr=='=')
            {
                buffer[ptr]='\0';
                ptr=0;
                tmp.key=copyString(buffer);
                if (chr=='=')
                    state=STATE_VALUE;
                else
                    state=STATE_AFTER_KEY;
            }
            else
            {
                buffer[ptr]=chr;
                ptr++;
            }
            break;
        case STATE_AFTER_KEY:
            if (chr=='=')
                state=STATE_VALUE;
            break;
        case STATE_VALUE:
            if (isNewLine(chr))
            {
                DONE_VALUE
                if (counter==MESSAGES_NUMBER)
                    return counter;
                state=STATE_BEFORE_KEY;
            }
            else
            {
                if (chr=='\\')
                    state=STATE_VALUE_SLASH;
                else
                {
                    buffer[ptr]=chr;
                    ptr++;
                }
            }
            break;
        case STATE_VALUE_SLASH:
            switch (chr)
            {
            case 'r':
                buffer[ptr++]='\r';
                break;
            case 'n':
                buffer[ptr++]='\n';
                break;
            case '\\':
                buffer[ptr++]='\\';
                break;
            }
            state=STATE_VALUE;
            break;
        case STATE_COMMENT:
            if (isNewLine(chr))
            {
                state=STATE_BEFORE_KEY;
            }
            break;
        }
    }
    if (state==STATE_VALUE)
    {
        DONE_VALUE
    }
    return counter;
}

static ULONG load(PSZ lang, MESSAGE messages[])
{
    static CHAR filename[2048];
    FILE* file;
    ULONG ret=0;
    strcpy(filename, LocaleDir);
    strcat(filename, "\\messages.");
    strcat(filename, lang);
    file=fopen(filename, "rt");
    if (file!=NULL)
    {
        ULONG len=filelength(fileno(file));
        PSZ m=M_ALLOC(len+1);
        if (m!=NULL)
        {
            fread(m, 1, len, file);
            m[len]='\0';
            ret=parse(m, messages);
            M_FREE(m);
        }
        fclose(file);
    }
    return ret;
}

static VOID print(MESSAGE messages[], ULONG len)
{
    ULONG t;
    for (t=0; t<len; t++)
        printf("[%s]=[%s]\n", messages[t].key, messages[t].message);
}

static VOID unload(MESSAGE messages[], ULONG len)
{
    ULONG t;
    for (t=0; t<len; t++)
    {
        M_FREE(messages[t].key);
        M_FREE(messages[t].message);
    }
}

VOID localeInit(PSZ dir, PSZ lang, PSZ def)
{
    strcpy(LocaleDir, dir);
    CurrentLen=load(lang, CurrentMessages);
    //print(CurrentMessages, CurrentLen);
    //printf("====\n");
    DefaultLen=load(def, DefaultMessages);
    //print(DefaultMessages, DefaultLen);
}

VOID localeFree()
{
    unload(CurrentMessages, CurrentLen);
    unload(DefaultMessages, DefaultLen);
}

static PSZ getMsg(MESSAGE messages[], ULONG len, PSZ key)
{
    ULONG t;
    for (t=0; t<len; t++)
    {
        MESSAGE* tmp=&messages[t];
        if (!strcmp(tmp->key, key))
        {
            return tmp->message;
        }
    }
    return NULL;
}

static PSZ findMsg(PSZ key)
{
    PSZ ret;
    ret=getMsg(CurrentMessages, CurrentLen, key);
    if (ret!=NULL)
        return ret;
    return getMsg(DefaultMessages, DefaultLen, key);
}

PSZ localeGetMsg(PSZ key)
{
    PSZ ret=findMsg(key);
    if (ret!=NULL)
        return ret;
    return key;
}

static HBITMAP loadBMP(HPS hps, PCHAR data, ULONG size, PULONG x, PULONG y)
{
    HBITMAP hbmp;
    PBITMAPFILEHEADER2 pbfh2;
    PBITMAPINFOHEADER2 pbih2;
    PBYTE pb;

    pbfh2=(PBITMAPFILEHEADER2)data;
    if (pbfh2->usType!=BFT_BMAP)
        return NULL;
    pbih2=&pbfh2->bmp2;
    if (pbfh2->offBits>=size)
        return NULL;
    pb=data+pbfh2->offBits;

    hbmp=GpiCreateBitmap(hps, pbih2, CBM_INIT, pb, (PBITMAPINFO2)pbih2);

    if (hbmp)
    {
        if (pbih2->cbFix==sizeof(BITMAPINFOHEADER))
        {
            PBITMAPINFO ptr;
            ptr=(PBITMAPINFO)pbih2;
            *x=ptr->cx;
            *y=ptr->cy;
        }
        else
        {
            *x=pbih2->cx;
            *y=pbih2->cy;
        }
    }
    return hbmp;
}

HBITMAP localeGetBitmap(PSZ key, HPS hps, PULONG x, PULONG y)
{
    static CHAR buffer[2048];
    PSZ filename;
    FILE* file;
    ULONG len;
    PCHAR data;
    HBITMAP ret=NULL;
    filename=findMsg(key);
    if (filename==NULL)
        return NULL;
    strcpy(buffer, LocaleDir);
    strcat(buffer, "\\");
    strcat(buffer, filename);
    file=fopen(buffer, "rb");
    if (file!=NULL)
    {
        len=filelength(fileno(file));
        data=M_ALLOC(len);
        if (data)
        {
            fread(data, 1, len, file);
            ret=loadBMP(hps, data, len, x, y);
            M_FREE(data);
        }
        fclose(file);
    }
    return ret;
}
