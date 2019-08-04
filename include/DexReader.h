//
// Created by Hasee on 2019/7/30.
//

#ifndef DEXPARSEDEMO_DEXREADER_H
#define DEXPARSEDEMO_DEXREADER_H

#include "DexFile.h"

#define readStringWithCopy(index,str){\
    char *ori =DexReader.readStringByIndex(index);\
    int strLen = strlen(ori);\
    str =(char*)alloca(strLen+1);\
    str[strLen] =0;\
    memcpy(str,ori,strLen);}\

#define readTypeStringWithCopy(index,str){\
    char *ori =DexReader.readTypeStrByIndex(index);\
    int strLen = strlen(ori);\
    str =(char*)alloca(strLen+1);\
    str[strLen] =0;\
    memcpy(str,ori,strLen);}\



#define readTypeListStrWithCopy(pTypeList,str){\
    char *typeStr =(char*)malloc(512);\
    typeStr[0] =0;\
    int i=0;\
    for(;i<pTypeList->size;i++){\
        char *str_tmp =DexReader.readTypeStrByIndex(pTypeList->list[i].typeIdx);\
        sprintf(typeStr,"%s,%s",typeStr,str_tmp);\
    }\
    unsigned int typeStrLen = strlen(typeStr);\
    str =(char*)alloca(typeStrLen+1);\
    str[typeStrLen] =0;\
    memcpy(str,typeStr,typeStrLen);}\


typedef struct {
    int (*init)(const char*);
    void (*close)();
    void* (*offset)(unsigned int);
    char* (*readStringByIndex)(unsigned int);
    char* (*readTypeStrByIndex)(unsigned int);
    DexHeader* (*readDexHeader)();
    DexStringTable* (*readStringTable)();
    DexTypeTable* (*readTypeTable)();
    DexProtoIdTable* (*readProtoIdTable)();
    DexFieldIdTable* (*readFieldIdTable)();
    DexMethodIdTable* (*readMethodIdTable)();
    DexClassDefTable* (*readClassDefTable)();
}_dex_reader;

_dex_reader DexReader;


#endif //DEXPARSEDEMO_DEXREADER_H
