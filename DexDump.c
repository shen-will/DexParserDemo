//
// Created by Hasee on 2019/7/30.
//

#include "include/DexDump.h"
#include "include/Helper.h"
#include "include/DexReader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/DexFile.h"

#define SHOW_COUNT_TIP(name,size)  printf("%s table item count %u\n",name,size)
#define PRINT_ITEM_START(index) printf("#%u--------------------------\n",index)
#define PRINT_ITEM_END printf("----------------------------\n")
#define FLAG_TO_STR(flag,value,result,def)(flag & value) > 0 ? result : def



LOCAL void parseAccessFlags(unsigned int flags,char *str){

    str[0]=0;

    strcat(str,FLAG_TO_STR(flags,ACC_PUBLIC,"public,",""));
    strcat(str,FLAG_TO_STR(flags,ACC_FINAL,"final,",""));

    if(IS_CLASS(flags) || IS_METHOD(flags) || IS_INNER_CLASS(flags)){
        strcat(str,FLAG_TO_STR(flags,ACC_ABSTRACT,"abstract,",""));
    }

    if(IS_CLASS(flags) || IS_FIELD(flags) || IS_INNER_CLASS(flags)){
        strcat(str,FLAG_TO_STR(flags,ACC_ENUM,"enum,",""));
    }


    if(IS_METHOD(flags) || IS_FIELD(flags) || IS_INNER_CLASS(flags)){
        strcat(str,FLAG_TO_STR(flags,ACC_PRIVATE,"private,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_PROTECTED,"protected,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_STATIC,"static,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_SYNTHETIC,"synthetic,",""));
    }

    if(IS_CLASS(flags) || IS_INNER_CLASS(flags)){
        strcat(str,FLAG_TO_STR(flags,ACC_INTERFACE,"interface,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_ANNOTATION,"annotation,",""));

    }

    if(IS_METHOD(flags)){
        strcat(str,FLAG_TO_STR(flags,ACC_SYNCHRONIZED,"synchronized,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_BRIDGE,"bridge,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_VARARGS,"varargs,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_NATIVE,"native,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_STRICT,"strict,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_CONSTRUCTOR,"constructor,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_DECLARED_SYNCHRONIZED,"declared_synchronized,",""));
    }

    if(IS_FIELD(flags)){
        strcat(str,FLAG_TO_STR(flags,ACC_VOLATILE,"volatile,",""));
        strcat(str,FLAG_TO_STR(flags,ACC_TRANSIENT,"trabsient,",""));
    }
}


LOCAL void dumpClassData(unsigned int index){

    DexHeader *header = DexReader.readDexHeader();

    if(index >= header->classDefsSize)
        return;

    DexClassDef *classDef = DexReader.readClassDefTable()->at(index);
    /*
     * print format:
     * xxxx.java
     * static fields....
     * instance fields...
     * methods.....
     * */

    char *sourceFile;
    if(classDef->sourceFileIdx == kDexNoIndex){
        sourceFile ="unknow";
    } else{
        sourceFile = DexReader.readStringByIndex(classDef->sourceFileIdx);
    }
    printf("source file %s\n",sourceFile);
    int nameLen = strlen(sourceFile) -5;
    char name[nameLen+1];
    memcpy(name,sourceFile,nameLen);
    name[nameLen] =0;

    char classAccstr[125];
    parseAccessFlags(classDef->accessFlags,classAccstr);

    unsigned char isInterface = (classDef->accessFlags && ACC_INTERFACE ) > 0 ? 1 : 0;


    char *superClass;
    readTypeStringWithCopy(classDef->superclassIdx,superClass);

    char *interfaceStr = NULL;
    if(classDef->interfacesOff == 0){
        interfaceStr = "";
    } else{
        DexTypeList *typeList = (DexTypeList*)DexReader.offset(classDef->interfacesOff);
        readTypeListStrWithCopy(typeList,interfaceStr);
    }

    printf("%s %s %s extends %s implements %s {\n\n",
           classAccstr,isInterface ? "interface" : "class",name,superClass,interfaceStr);



     DexFieldIdTable *fieldIdTable = DexReader.readFieldIdTable();
     DexMethodIdTable *methodIdTable = DexReader.readMethodIdTable();
     DexClassDefTable *classDefTable = DexReader.readClassDefTable();
     DexProtoIdTable  *protoIdTable = DexReader.readProtoIdTable();

     DexClassDataHeader dataHeader;
     classDefTable->dataHeaderAt(index,&dataHeader);

    //classdata may be empty in some classes(no define field method ...)
     if(dataHeader.pdata != NULL){
         int i=0;
         DexField field;
         //parse static field / instance field
         for(;i<(dataHeader.staticFieldsSize + dataHeader.instanceFieldsSize);i++){

             dataHeader.readDexField(&dataHeader,&field);
             char accstr[125];
             parseAccessFlags(field.accessFlags,accstr);

             DexFieldId *fieldId = fieldIdTable->at(field.fieldIdx);

             char *name;
             readStringWithCopy(fieldId->nameIdx,name);
             char *type;
             readTypeStringWithCopy(fieldId->typeIdx,type);

             printf("%s %s %s;\n",accstr,type,name);

         }


         //parse direct method / virtual method
         DexMethod method;
         i =0;
         for(;i < (dataHeader.directMethodsSize + dataHeader.virtualMethodsSize);i++){
             dataHeader.readDexMethod(&dataHeader,&method);
             parseAccessFlags(method.accessFlags,classAccstr);

             DexMethodId *methodId =methodIdTable->at(method.methodIdx);

             char *name;
             readStringWithCopy(methodId->nameIdx,name);

             DexProtoId *protoId = protoIdTable->at(methodId->protoIdx);

             char *returnStr;
             readTypeStringWithCopy(protoId->returnTypeIdx,returnStr);

             char *params;
             if(protoId->parametersOff == 0){
                 params ="";
             } else{
                 DexTypeList *typeList = (DexTypeList*)DexReader.offset(protoId->parametersOff);
                 readTypeListStrWithCopy(typeList,params);
             }

             printf("%s %s %s(%s){\n//no code parse...\n}\n",classAccstr,returnStr,name,params);

         }

     }


     //end class
     puts("\n}");

     fieldIdTable->destroy((struct _table_struct*)fieldIdTable);
     methodIdTable->destroy((struct _table_struct*)methodIdTable);
     classDefTable->destroy((struct _table_struct*)classDefTable);
     protoIdTable->destroy((struct _table_struct*)protoIdTable);

}

LOCAL void dumpClassDefTable(){

    puts("hello fuck");

    DexClassDefTable *table =DexReader.readClassDefTable();

    SHOW_COUNT_TIP("class define",table->size);

    int i=0;
    for(;i<table->size;i++){

        DexClassDef *classDef = table->at(i);
        PRINT_ITEM_START(i);

        printf("类型: %s\n",DexReader.readTypeStrByIndex(classDef->classIdx));

        printf("类别 :%s\n",TO_TYPE_STR(classDef->accessFlags));

        char accessStr[128];
        parseAccessFlags(classDef->accessFlags,accessStr);
        printf("访问修饰: %s\n",accessStr);

        printf("父类: %s\n",classDef->superclassIdx == kDexNoIndex ?
        "java/lang/Object;" : DexReader.readTypeStrByIndex(classDef->superclassIdx));

        printf("源文: %s\n",DexReader.readStringByIndex(classDef->sourceFileIdx));
        DexClassDataHeader dataHeader;
        table->dataHeaderAt(i,&dataHeader);

        printf("static field count: %u\ninstance field count: %u\ndirect method count:"
               " %u\nvirtual method count: %u\n",
                dataHeader.staticFieldsSize,
                dataHeader.instanceFieldsSize,
                dataHeader.directMethodsSize,
                dataHeader.virtualMethodsSize);

    }
}


LOCAL void dumpProtoTable(){

    DexProtoIdTable *table = DexReader.readProtoIdTable();

    SHOW_COUNT_TIP("proto",table->size);

    int i=0;
    for(;i<table->size;i++){

        PRINT_ITEM_START(i);
        DexProtoId *id=table->at(i);


        char *params=NULL;
        if(id->parametersOff>0){
            DexTypeList *typeList =(DexTypeList*)DexReader.offset(id->parametersOff);
            int k =0;
            for(;k<typeList->size;k++){
                char *type = DexReader.readTypeStrByIndex(typeList->list[k].typeIdx);
                int lenType =strlen(type);

                int bakLen = params == NULL ? 0 : strlen(params);
                if(params == NULL){
                    params = malloc(lenType+2);
                    params[0]=0;
                } else{
                    params = realloc(params,bakLen+lenType+2);
                }

                sprintf(params+bakLen,"%s;",type);
            }
        }

        printf("描述符: %s\n",DexReader.readStringByIndex(id->shortyIdx));
        printf("参数列表: %s\n",params == NULL ? "" : params);
        printf("返回类型: %s\n",id->returnTypeIdx == 0 ? "void"
        :DexReader.readTypeStrByIndex(id->returnTypeIdx));

        if(params != NULL){
            free(params);
            params = NULL;
        }
    }

    PRINT_ITEM_END;
}


LOCAL void dumpTypeTable(){

    DexStringTable *table = DexReader.readStringTable();

    SHOW_COUNT_TIP("type",table->size);

    int i=0;
    for(;i<table->size;i++){

        puts(table->at(i));
    }

    PRINT_ITEM_END;

    table->destroy((struct _table_struct *) table);
}

LOCAL void dumpStringTable(){

    DexStringTable *table = DexReader.readStringTable();

    SHOW_COUNT_TIP("string",table->size);

    int i=0;
    for(;i<table->size;i++){

       puts(table->at(i));
    }

    PRINT_ITEM_END;

    table->destroy((struct _table_struct *) table);
}


LOCAL void dumpFieldTable(){

    DexFieldIdTable *table = DexReader.readFieldIdTable();

    SHOW_COUNT_TIP("field",table->size);

    int i=0;
    for(;i<table->size;i++){

        PRINT_ITEM_START(i);
        DexFieldId *id=table->at(i);

        printf("定义类: %s\n",DexReader.readTypeStrByIndex(id->classIdx));
        printf("类型: %s\n",DexReader.readTypeStrByIndex(id->typeIdx));
        printf("字段名: %s\n",DexReader.readStringByIndex(id->nameIdx));
    }

    PRINT_ITEM_END;

    table->destroy((struct _table_struct *) table);
}

LOCAL void dumpMethodTable(){

    DexMethodIdTable *table = DexReader.readMethodIdTable();

    DexProtoIdTable *proTable = DexReader.readProtoIdTable();

    SHOW_COUNT_TIP("method",table->size);

    int i=0;
    for(;i<table->size;i++){

        PRINT_ITEM_START(i);
        DexMethodId *id=table->at(i);

        printf("定义类: %s\n",DexReader.readTypeStrByIndex(id->classIdx));
        printf("字段名: %s\n",DexReader.readStringByIndex(id->nameIdx));

        DexProtoId *proid = proTable->at(id->protoIdx);

        char *params=NULL;
        if(proid->parametersOff>0){
            DexTypeList *typeList =(DexTypeList*)DexReader.offset(proid->parametersOff);
            int k =0;
            for(;k<typeList->size;k++){
                char *type = DexReader.readTypeStrByIndex(typeList->list[k].typeIdx);
                int lenType =strlen(type);

                int bakLen = params == NULL ? 0 : strlen(params);
                if(params == NULL){
                    params = malloc(lenType+2);
                    params[0]=0;
                } else{
                    params = realloc(params,bakLen+lenType+2);
                }

                sprintf(params+bakLen,"%s;",type);
            }
        }

        printf("描述符: %s\n",DexReader.readStringByIndex(proid->shortyIdx));
        printf("参数列表: %s\n",params == NULL ? "" : params);
        printf("返回类型: %s\n",proid->returnTypeIdx == 0 ? "void"
                                                   :DexReader.readTypeStrByIndex(proid->returnTypeIdx));

        if(params != NULL){
            free(params);
            params = NULL;
        }

    }

    PRINT_ITEM_END;

    table->destroy((struct _table_struct *) table);
}


LOCAL void dumpDexHeader(){

    DexHeader *header=DexReader.readDexHeader();

    char *magic;
    repCharWithCopy(header->magic,magic,'\n',' ',0);

    char *signature;
   // bitsToHexStr(header->signature,signature,kSHA1DigestLen);

    _bitsToHexStr(header->signature,signature,short,kSHA1DigestLen/sizeof(short));

    printf("magic %s\nchecksum %X\nsignature %s\nfileSize %u byte\n"
           "headerSize %u\nendianTag %#X\n"
           "linkSize %u\n"
           "linkOff %u\n"
           "mapOff %u\n"
           "stringIdsSize %u\n"
           "stringIdsOff %u\n"
           "typeIdsSize %u\n"
           "typeIdsOff %u\n"
           "protoIdsSize %u\n"
           "protoIdsOff %u\n"
            "fieldIdsSize %u\n"
            "fieldIdsOff %u\n"
            "methodIdsSize %u\n"
            "methodIdsOff %u\n"
            "classDefsSize %u\n"
            "classDefsOff %u\n"
            "dataSize %u\n"
            "dataOff %u\n",

            magic,
            header->checksum,
            signature,
            header->fileSize,
            header->headerSize,
            header->endianTag,
            header->linkSize,
            header->linkOff,
            header->mapOff,
            header->stringIdsSize,
            header->stringIdsOff,
            header->typeIdsSize,
            header->typeIdsOff,
            header->protoIdsSize,
            header->protoIdsOff,
            header->fieldIdsSize,
            header->fieldIdsOff,
            header->methodIdsSize,
            header->methodIdsOff,
            header->classDefsSize,
            header->classDefsOff,
            header->dataSize,
            header->dataOff);
}


_dex_dump DexDump ={dumpDexHeader,dumpProtoTable,dumpTypeTable,dumpStringTable,
        dumpFieldTable,dumpMethodTable,dumpClassDefTable,dumpClassData};