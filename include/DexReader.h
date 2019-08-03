//
// Created by Hasee on 2019/7/30.
//

#ifndef DEXPARSEDEMO_DEXREADER_H
#define DEXPARSEDEMO_DEXREADER_H

#include "DexFile.h"

#define readStringWithCopy(index)({\
    char *str =DexReader.readStringByIndex(index);\
    int strLen = strlen(str);\
    char cpstr[strLen+1];\
    cpstr[strLen] =0;\
    memcpy(cpstr,str,strLen);\
    cpstr;})

#define readTypeStringWithCopy(index)({\
    char *str =DexReader.readTypeStrByIndex(index);\
    int strLen = strlen(str);\
    char cpstr[strLen+1];\
    cpstr[strLen] =0;\
    memcpy(cpstr,str,strLen);\
    cpstr;})


#define readTypeListStrWithCopy(pTypeList)({\
    char *typeStr =(char*)calloc(1,1);\
    int i=0;\
    for(;i<pTypeList->size;i++){\
        char *str =DexReader.readTypeStrByIndex(pTypeList->list[i].typeIdx);\
        typeStr = realloc(typeStr,strlen(typeStr)+strlen(str)+2);\
        sprintf(typeStr,"%s,%s",typeStr,str);\
    }\
    unsigned int typeStrLen = strlen(typeStr);\
    char *result=alloca(typeStrLen+1);\
    result[typeStrLen] =0;\
    memcpy(result,typeStr,typeStrLen);\
    result;})

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
