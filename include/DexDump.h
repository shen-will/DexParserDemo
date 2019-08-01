//
// Created by Hasee on 2019/7/30.
//

#ifndef DEXPARSEDEMO_DEXDUMP_H
#define DEXPARSEDEMO_DEXDUMP_H

typedef struct {
    void (*dumpDexHeader)();
    void (*dumpProtoTable)();
    void (*dumpTypeTable)();
    void (*dumpStringTable)();
    void (*dumpFieldTable)();
    void (*dumpMethodTable)();
    void (*dumpClassDefTable)();
}_dex_dump;

_dex_dump DexDump;

#endif //DEXPARSEDEMO_DEXDUMP_H
