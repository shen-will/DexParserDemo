//
// Created by Hasee on 2019/7/30.
//

#include <stdio.h>
#include "include/DexReader.h"
#include <stdlib.h>

#include "include/Helper.h"
#include "include/DexFile.h"

#ifdef _WIN32
#include <rpc.h>
#endif

#define checkInit(ret_value) if(fileMap == NULL) return ret_value

#define checkLocalStr(name)\
    LOCAL char *name;\
    if(name != NULL) {\
        free(name);\
        name = NULL;\
    }


LOCAL void *fileMap;

LOCAL int init(const char *path) {

    if(path == NULL) return -1;

    FILE *file=fopen(path,"rb");

    if(file == NULL) return -1;

    unsigned int fsize =-1;
    getFileSize(file,&fsize);

    if(fsize <= 0) return -1;

    if((fileMap = malloc(fsize)) == NULL)
        return -1;

    if(fread(fileMap,1,fsize,file) != fsize)
        return -1;

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    return 0;
}

LOCAL void close() {
    if(fileMap != NULL){
        free(fileMap);
        fileMap = NULL;
    }
}

LOCAL void destroyTable(struct _table_struct *table){
    if(table == NULL) return;

    free(table);
}



LOCAL DexHeader* readDexHeader(){

    checkInit(NULL);

    return (DexHeader*)fileMap;
}

//don't reentry if you want to keep result string,copy to other area
LOCAL char* readStringByIndex(unsigned int index){

    checkLocalStr(str)

    DexHeader *header = readDexHeader();

    if(index >= header->stringIdsSize)
        return  str;

    DexStringId id =  ((DexStringId*)(fileMap+header->stringIdsOff))[index];

    unsigned int len=0;
    int str_len=decodeUnsignedLeb128((u1*)(fileMap+id.stringDataOff),&len);

    str = malloc(str_len+1);
    str[str_len]=0;

    memcpy(str,(u1*)(fileMap+id.stringDataOff+len),str_len);

    return str;
}

LOCAL DexStringTable* readStringTable(){

    DexHeader *header = readDexHeader();

    DexStringTable *table = malloc(sizeof(DexStringTable));

    table->size = header->stringIdsSize;
    table->at = readStringByIndex;
    table->destroy = destroyTable;

    return table;
}




LOCAL char* readTypeStrByIndex(unsigned int index){

    DexHeader *header = readDexHeader();

    if(index >= header->typeIdsSize) return NULL;

    DexTypeId id=((DexTypeId*)(fileMap+header->typeIdsOff))[index];

    return readStringByIndex(id.descriptorIdx);
}


LOCAL DexTypeTable* readTypeTable(){

    DexHeader *header = readDexHeader();

    DexTypeTable *table =malloc(sizeof(DexTypeTable));
    table->at = readTypeStrByIndex;

    table->size = header->typeIdsSize;

    table->destroy = destroyTable;
}

LOCAL DexProtoId* readProtoByIndex(unsigned int index){

    DexHeader *header = readDexHeader();

    if(index >= header->protoIdsSize)
        return NULL;

    return ((DexProtoId*)(fileMap+header->protoIdsOff))+index;

}

LOCAL DexProtoTable* readProtoTable(){

    DexHeader *header = readDexHeader();

    DexProtoTable *table = malloc(sizeof(DexProtoTable));

    table->size = header->protoIdsSize;

    table->at = readProtoByIndex;

    table->destroy = destroyTable;

    return table;

}

LOCAL DexFieldId* readFieldByIndex(unsigned int index){


    DexHeader *header = readDexHeader();

    if(index >= header->fieldIdsSize)
        return NULL;

    return ((DexFieldId*)(fileMap + header->fieldIdsOff)) + index;
}

LOCAL DexFieldTable* readFieldTable(){

    DexHeader *header = readDexHeader();

    DexFieldTable *table = malloc(sizeof(DexFieldTable));
    table->size = header->fieldIdsSize;
    table->at = readFieldByIndex;
    table->destroy = destroyTable;
}

LOCAL DexMethodId* readMethodByIndex(unsigned int index){


    DexHeader *header = readDexHeader();

    if(index >= header->methodIdsSize)
        return NULL;

    return ((DexMethodId*)(fileMap + header->methodIdsOff)) + index;
}


LOCAL DexMethodTable* readMethodTable(){
    DexHeader *header = readDexHeader();

    DexMethodTable *table = malloc(sizeof(DexMethodTable));
    table->size = header->methodIdsSize;
    table->at = readMethodByIndex;
    table->destroy = destroyTable;
}


LOCAL DexClassDef* readClassDefByIndex(unsigned int index){

    DexHeader *header = readDexHeader();

    if(index >= header->classDefsSize)
        return  NULL;

    return ((DexClassDef*)(fileMap+header->classDefsOff))+index;
}

LOCAL void readClassDataHeader(unsigned int index,DexClassDataHeader *dataHeader){

    DexClassDef *classDef = readClassDefByIndex(index);

    if(classDef == NULL)
        return;

    void* pdata = fileMap + classDef->classDataOff;

    /*
    uleb128 staticFieldsSize;
    uleb128 instanceFieldsSize;
    uleb128 directMethodsSize;
    uleb128 virtualMethodsSize;*/

    unsigned int len;
    dataHeader->staticFieldsSize = decodeUnsignedLeb128(pdata,&len);
    pdata+=len;
    dataHeader->instanceFieldsSize = decodeUnsignedLeb128(pdata,&len);
    pdata+=len;
    dataHeader->directMethodsSize = decodeUnsignedLeb128(pdata,&len);
    pdata+=len;
    dataHeader->virtualMethodsSize = decodeUnsignedLeb128(pdata,&len);
    pdata+=len;

    dataHeader->dataOff = (unsigned int)pdata;
}

LOCAL DexClassDefTable*  readClassDefTable(){

    DexHeader *header = readDexHeader();

    DexClassDefTable *table = (DexClassDefTable*)malloc(sizeof(table));

    table->size = header->classDefsSize;
    table->at = readClassDefByIndex;
    table->destroy = destroyTable;
    table->dataHeaderAt = readClassDataHeader;

    return table;
}

LOCAL void* offset(unsigned int len){
    return fileMap +len;
}


_dex_reader DexReader ={init,close,offset,readStringByIndex,readTypeStrByIndex,readDexHeader,
                        readStringTable,readTypeTable,readProtoTable,readFieldTable,readMethodTable,
                        readClassDefTable};



