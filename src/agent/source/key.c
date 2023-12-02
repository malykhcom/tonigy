#include "global.h"
#include "rsaeuro.h"
#include "rsa.h"

static CHAR PublicKey[sizeof(R_RSA_PUBLIC_KEY)]=
{
#include "public.h"
};

VOID keyDecode(PMAINDATA maindata)
{
    PCHAR Out;
    //if (maindata->Key[0])
    {
         //printf("!\n");
        Out=M_ALLOC(MAX_RSA_MODULUS_LEN+1);
        //if (Out)
        {
            CHAR res;
            UINT t=1;
            memset(Out, 0, MAX_RSA_MODULUS_LEN+1);
            res=(CHAR)RSAPublicDecrypt(Out, &t, maindata->Key, maindata->KeyLen,
                                       (R_RSA_PUBLIC_KEY*)PublicKey);
            if (res)
            {
                Out[0]='\0';
                /*Out[0]='a';
                Out[1]='b';
                Out[2]='\0';
                t=3;*/
            }
            memcpy(maindata->Key, Out, t);
            M_FREE(Out);
            maindata->KeyLen=t;
            maindata->KeyRes=res;
            //maindata->KeyRes=0;
        }
    }
    /*printf("len=%u, res=%s\n", maindata->KeyLen, maindata->KeyRes?"TRUE":"FALSE");
    printf("Decoded: '%s'\n", maindata->Key);
    printf("value=%d\n", maindata->Key[maindata->KeyLen-5]);
    if (!maindata->Key[0] ||
        maindata->KeyLen%5==0)
        printf("ok\n");
    else
    printf("cracked!\n");*/
}
