//
// Created by Hasee on 2019/7/30.
//

#ifndef DEXPARSEDEMO_HELPER_H
#define DEXPARSEDEMO_HELPER_H

#include <stdlib.h>
#include <string.h>
#include "DexFile.h"

#define LOCAL static

#define getFileSize(file,psize) {\
    unsigned int t_csize = ftell(file); \
    if(fseek(file,0,SEEK_END) != 0){\
        *psize =-1;\
     }else{\
        *psize = ftell(file);\
        fseek(file,t_csize,SEEK_SET);\
     }}


#define repCharWithCopy(str,dst,c_old,c_new,r){\
    unsigned int len=strlen(str)+1;\
    dst=(char*)alloca(len);\
    strcpy(dst,str);\
    char *p=dst;\
    while ((p=strchr(p,c_old))!=NULL){\
    *p=c_new;\
    if(r == 0)\
        break;\
    }}

#define _bitsToHexStr(bs,bak,type,count) {\
    int len =sizeof(type) * count;\
    int step = sizeof(type);\
    unsigned char *bs_bak;\
    if(step > 1){\
        bs_bak = malloc(len);\
        memcpy(bs_bak,bs,len);\
    }else{\
        bs_bak = bs;\
    }\
    bak =(char*)alloca(len*2+1);\
    bak[len*2] =0;\
    unsigned char buf[17]={0};\
    int index=0;\
    while (index < (len/step)){\
        unsigned type *p = ((unsigned type*)bs_bak)+index;\
        reverBits((unsigned char*)p,step);\
        sprintf(buf,"%016llX",(long long)*p);\
        memcpy(bak+index*2*step,buf+16-(step*2),step*2);\
        index++;\
    }\
    if(step >1) free(bs_bak);\
}



#define newObject(type) (type*) malloc(sizeof(type))


void reverBits(unsigned char *cp, unsigned int len);


int decodeUnsignedLeb128(u1 *bs,unsigned int *len);

int decodeSignedLeb128(u1 *bs,unsigned int *len);


#endif //DEXPARSEDEMO_HELPER_H
