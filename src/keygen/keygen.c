#include <os2def.h>
#include <stdio.h>
#include "rsaeuro.h"
#include "rsa.h"

static CHAR PrivateKey[sizeof(R_RSA_PRIVATE_KEY)]=
{
#include "..\global\private.h"
};

static CHAR Sign[10]=
{
    0x81, 0x2, 0x83, 0x4, 0x1, 0x1, 0x55, 0x55, 0xaa, 0xaa
};
static CHAR Data[40];
static CHAR Out[MAX_RSA_MODULUS_LEN+1];
static CHAR Key[MAX_RSA_MODULUS_LEN+1];

int main(int argc, char* argv[])
{
    UINT t;
    if (argc!=2)
    {
        fprintf(stderr,"Usage: keygen \"User Name\"\n");
        return 1;
    }
    fprintf(stderr, "Generate key for \"%s\"...\n", argv[1]);
    memcpy(&Data[0], Sign, 10);
    memcpy(&Data[10], Sign, 10);
    memcpy(&Data[20], Sign, 10);
    memcpy(&Data[30], Sign, 10);
    if (strlen(argv[1])+1>30)
    {
        fprintf(stderr, "Error: Name too long.\n");
        return 2;
    }
    strcpy(Data, argv[1]);
    if (RSAPrivateEncrypt(Out, &t, Data, sizeof(Data),
                          (R_RSA_PRIVATE_KEY*)PrivateKey) ||
       t!=64)
    {
        fprintf(stderr, "Error: Encoding step 1 failed.\n");
        return 3;
    }
    if (R_EncodePEMBlock(Key, &t, Out, t) ||
        t!=88)
    {
        fprintf(stderr, "Error: Encoding step 2 failed.\n");
        return 4;
    }
    fwrite(Key, 1, 44, stdout);
    fprintf(stdout, "\n");
    fwrite(&Key[44], 1, 44, stdout);
    fprintf(stdout, "\n");
    return 0;
}