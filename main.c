#include <stdio.h>
#include "include/DexReader.h"
#include "include/DexDump.h"
#include "include/Helper.h"

int main() {

    char *path = "C:/Users/Hasee/Desktop/classes.dex";

    if(DexReader.init(path) !=0){
        puts("dex reader init error");
        return -1;
    }
    puts(DexReader.readTypeStrByIndex(6));
    printf("%p\n",DexReader.readTypeStrByIndex);

    DexDump.dumpClassDefTable();

    DexReader.close();

    return 0;
}