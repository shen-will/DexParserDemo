#include <stdio.h>
#include "include/DexReader.h"
#include "include/DexDump.h"
#include "include/Helper.h"


int main() {

    //dex path
    char *path = "";

    if(DexReader.init(path) !=0){
        puts("dex reader init error");
        return -1;
    }

    //DexDump.dumpDexHeader();
   // DexDump.dumpClassDefTable();
   //DexDump.dumpClassData(2403);

    DexReader.close();

    return 0;
}