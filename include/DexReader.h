//
// Created by Hasee on 2019/7/30.
//

#ifndef DEXPARSEDEMO_DEXREADER_H
#define DEXPARSEDEMO_DEXREADER_H

#include "DexFile.h"

typedef struct {
    int (*init)(const char*);
    void (*close)();
    void* (*offset)(unsigned int);
    char* (*readStringByIndex)(unsigned int);
    char* (*readTypeStrByIndex)(unsigned int);
    DexHeader* (*readDexHeader)();
    DexStringTable* (*readStringTable)();
    DexTypeTable* (*readTypeTable)();
    DexProtoTable* (*readProtoTable)();
    DexFieldTable* (*readFieldTable)();
    DexMethodTable* (*readMethodTable)();
    DexClassDefTable* (*readClassDefTable)();
}_dex_reader;

_dex_reader DexReader;


#endif //DEXPARSEDEMO_DEXREADER_H
