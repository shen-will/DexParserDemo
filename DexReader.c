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

LOCAL DexProtoId* readProtoIdByIndex(unsigned int index){

    DexHeader *header = readDexHeader();

    if(index >= header->protoIdsSize)
        return NULL;

    return ((DexProtoId*)(fileMap+header->protoIdsOff))+index;

}

LOCAL DexProtoIdTable* readProtoIdTable(){

    DexHeader *header = readDexHeader();

    DexProtoIdTable *table = newObject(DexProtoIdTable);

    table->size = header->protoIdsSize;

    table->at = readProtoIdByIndex;

    table->destroy = destroyTable;

    return table;

}

LOCAL DexFieldId* readFieldIdByIndex(unsigned int index){


    DexHeader *header = readDexHeader();

    if(index >= header->fieldIdsSize)
        return NULL;

    return ((DexFieldId*)(fileMap + header->fieldIdsOff)) + index;
}

LOCAL DexFieldIdTable* readFieldIdTable(){

    DexHeader *header = readDexHeader();

    DexFieldIdTable *table = newObject(DexFieldIdTable);
    table->size = header->fieldIdsSize;
    table->at = readFieldIdByIndex;
    table->destroy = destroyTable;
}

LOCAL DexMethodId* readMethodIdByIndex(unsigned int index){


    DexHeader *header = readDexHeader();

    if(index >= header->methodIdsSize)
        return NULL;

    return ((DexMethodId*)(fileMap + header->methodIdsOff)) + index;
}


LOCAL DexMethodIdTable* readMethodIdTable(){
    DexHeader *header = readDexHeader();

    DexMethodIdTable *table = newObject(DexMethodIdTable);
    table->size = header->methodIdsSize;
    table->at = readMethodIdByIndex;
    table->destroy = destroyTable;
}


LOCAL DexClassDef* readClassDefByIndex(unsigned int index){

    DexHeader *header = readDexHeader();

    if(index >= header->classDefsSize)
        return  NULL;

    return ((DexClassDef*)(fileMap+header->classDefsOff))+index;
}



LOCAL void readDexField(struct _class_data_header *dataHeader,DexField *field){

    if(dataHeader == NULL || dataHeader->pdata == NULL || field ==NULL)
        return;

    //static field  / instance field / direct method / virtual method
    unsigned int len=0;
    field->fieldIdx = decodeUnsignedLeb128(dataHeader->pdata,&len);
    dataHeader->pdata +=len;
    field->accessFlags = decodeUnsignedLeb128(dataHeader->pdata,&len);
    dataHeader->pdata +=len;
}


LOCAL void readDexMethod(struct _class_data_header *dataHeader,DexMethod *method){

    if(dataHeader == NULL || dataHeader->pdata == NULL || method ==NULL)
        return;

    unsigned int len =0;
    method->methodIdx = decodeUnsignedLeb128(dataHeader->pdata,&len);
    dataHeader->pdata +=len;
    method->accessFlags = decodeUnsignedLeb128(dataHeader->pdata,&len);
    dataHeader->pdata +=len;
    method->codeOff = decodeUnsignedLeb128(dataHeader->pdata,&len);
    dataHeader->pdata +=len;

}



LOCAL void readClassDataHeader(unsigned int index,DexClassDataHeader *dataHeader){

    DexClassDef *classDef = readClassDefByIndex(index);

    if(classDef == NULL)
        return;

    if(classDef->classDataOff == 0){

        dataHeader->staticFieldsSize =0;
        dataHeader->pdata =NULL;
        dataHeader->virtualMethodsSize =0;
        dataHeader->directMethodsSize =0;
        dataHeader->instanceFieldsSize =0;
        dataHeader->readDexField = NULL;
        dataHeader->readDexMethod = NULL;
        return;
    }


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

    dataHeader->pdata = pdata;

    dataHeader->readDexMethod =readDexMethod;
    dataHeader->readDexField = readDexField;

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
                        readStringTable,readTypeTable,readProtoIdTable,readFieldIdTable,readMethodIdTable,
                        readClassDefTable};



