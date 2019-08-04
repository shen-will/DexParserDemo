//
// Created by Hasee on 2019/7/30.
//

#ifndef DEXPARSEDEMO_DEXFILE_H
#define DEXPARSEDEMO_DEXFILE_H


typedef unsigned char u1;
typedef unsigned short u2;
typedef unsigned int u4;

#define kSHA1DigestLen 20
#define MAGIC_LENGTH 8

#define kDexEndianConstant 0x12345678   /* the endianness indicator */
#define kDexNoIndex 0xffffffff          /* not a valid index value */

typedef struct {
    u1  magic[MAGIC_LENGTH];           /* includes version number */
    u4  checksum;           /* adler32 checksum */
    u1  signature[kSHA1DigestLen]; /* SHA-1 hash */
    u4  fileSize;           /* length of entire file */
    u4  headerSize;         /* offset to start of next section */
    u4  endianTag;
    u4  linkSize;
    u4  linkOff;
    u4  mapOff;
    u4  stringIdsSize;
    u4  stringIdsOff;
    u4  typeIdsSize;
    u4  typeIdsOff;
    u4  protoIdsSize;
    u4  protoIdsOff;
    u4  fieldIdsSize;
    u4  fieldIdsOff;
    u4  methodIdsSize;
    u4  methodIdsOff;
    u4  classDefsSize;
    u4  classDefsOff;
    u4  dataSize;
    u4  dataOff;
}DexHeader;



/*
 * table base struct , extern by sub struct
 * */
#define TableStruct(type) \
unsigned int size;\
type* (*at)(unsigned int);\
void (*destroy)(struct _table_struct*); \

struct _table_struct {
    TableStruct(void)
};


typedef struct {
    u4 stringDataOff;      /* file offset to string_data_item */
}DexStringId;


typedef struct {
    TableStruct(char)
}DexStringTable;


//tyep id
typedef struct {
    u4  descriptorIdx;      /* index into stringIds list for type descriptor */
}DexTypeId;

//type string table
typedef struct {
    TableStruct(char)
}DexTypeTable;


typedef struct {
    u2  typeIdx;            /* index into typeIds */
}DexTypeItem;


typedef struct {
    u4  size;               /* #of entries in list */
    DexTypeItem list[1];    /* entries */
}DexTypeList;

typedef struct {
    u4  shortyIdx;          /* index into stringIds for shorty descriptor */
    u4  returnTypeIdx;      /* index into typeIds list for return type */
    u4  parametersOff;      /* file offset to type_list for parameter types */
}DexProtoId;

typedef struct {
    TableStruct(DexProtoId)
}DexProtoIdTable;



typedef struct {
    u2  classIdx;           /* index into typeIds list for defining class */
    u2  typeIdx;            /* index into typeIds for field type */
    u4  nameIdx;            /* index into stringIds for field name */
}DexFieldId;

typedef struct {
    TableStruct(DexFieldId)
}DexFieldIdTable;

typedef struct {
    u2  classIdx;           /* index into typeIds list for defining class */
    u2  protoIdx;           /* index into protoIds for method prototype */
    u4  nameIdx;            /* index into stringIds for method name */
}DexMethodId;

typedef struct {
    TableStruct(DexMethodId)
}DexMethodIdTable;


typedef struct{
    u4  classIdx;           /* index into typeIds for this class */
    u4  accessFlags;
    u4  superclassIdx;      /* index into typeIds for superclass */
    u4  interfacesOff;      /* file offset to DexTypeList */
    u4  sourceFileIdx;      /* index into stringIds for source file name */
    u4  annotationsOff;     /* file offset to annotations_directory_item */
    u4  classDataOff;       /* file offset to class_data_item */
    u4  staticValuesOff;    /* file offset to DexEncodedArray */
}DexClassDef;

//encoded field
typedef struct {
    //origin type is uleb128
    u4 fieldIdx;    /* index to a field_id_item */
    u4 accessFlags;
}DexField;


//encoded method
typedef struct{
    //origin type is uleb128
    u4 methodIdx;    /* index to a method_id_item */
    u4 accessFlags;
    u4 codeOff;      /* file offset to a code_item */
}DexMethod;



typedef struct _class_data_header {
    // origin type is uleb128
    u4 staticFieldsSize;
    u4 instanceFieldsSize;
    u4 directMethodsSize;
    u4 virtualMethodsSize;

    //data offset start of dex file  from classdef
    void *pdata;

    void(*readDexField)(struct _class_data_header *dataHeader,DexField *field);
    void(*readDexMethod)(struct _class_data_header *dataHeader,DexMethod *method);

}DexClassDataHeader;


typedef struct{
    TableStruct(DexClassDef)

    void (*dataHeaderAt)(unsigned int,DexClassDataHeader*);

}DexClassDefTable;


typedef struct {
    u2  registersSize;
    u2  insSize;
    u2  outsSize;
    u2  triesSize;
    u4  debugInfoOff;       /* file offset to debug info stream */
    u4  insnsSize;          /* size of the insns array, in u2 units */
    u2  insns[1];
    /* followed by optional u2 padding */
    /* followed by try_item[triesSize] */
    /* followed by uleb128 handlersSize */
    /* followed by catch_handler_item[handlersSize] */
}DexCode;



enum {
    ACC_PUBLIC       = 0x00000001,       // class, field, method, ic
    ACC_PRIVATE      = 0x00000002,       // field, method, ic
    ACC_PROTECTED    = 0x00000004,       // field, method, ic
    ACC_STATIC       = 0x00000008,       // field, method, ic
    ACC_FINAL        = 0x00000010,       // class, field, method, ic
    ACC_SYNCHRONIZED = 0x00000020,       // method (only allowed on natives)
    ACC_SUPER        = 0x00000020,       // class (not used in Dalvik)
    ACC_VOLATILE     = 0x00000040,       // field
    ACC_BRIDGE       = 0x00000040,       // method (1.5)
    ACC_TRANSIENT    = 0x00000080,       // field
    ACC_VARARGS      = 0x00000080,       // method (1.5)
    ACC_NATIVE       = 0x00000100,       // method
    ACC_INTERFACE    = 0x00000200,       // class, ic
    ACC_ABSTRACT     = 0x00000400,       // class, method, ic
    ACC_STRICT       = 0x00000800,       // method
    ACC_SYNTHETIC    = 0x00001000,       // field, method, ic
    ACC_ANNOTATION   = 0x00002000,       // class, ic (1.5)
    ACC_ENUM         = 0x00004000,       // class, field, ic (1.5)
    ACC_CONSTRUCTOR  = 0x00010000,       // method (Dalvik only)
    ACC_DECLARED_SYNCHRONIZED =
    0x00020000,       // method (Dalvik only)
    ACC_CLASS_MASK =
    (ACC_PUBLIC | ACC_FINAL | ACC_INTERFACE | ACC_ABSTRACT
     | ACC_SYNTHETIC | ACC_ANNOTATION | ACC_ENUM),
    ACC_INNER_CLASS_MASK =
    (ACC_CLASS_MASK | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC),
    ACC_FIELD_MASK =
    (ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
     | ACC_VOLATILE | ACC_TRANSIENT | ACC_SYNTHETIC | ACC_ENUM),
    ACC_METHOD_MASK =
    (ACC_PUBLIC | ACC_PRIVATE | ACC_PROTECTED | ACC_STATIC | ACC_FINAL
     | ACC_SYNCHRONIZED | ACC_BRIDGE | ACC_VARARGS | ACC_NATIVE
     | ACC_ABSTRACT | ACC_STRICT | ACC_SYNTHETIC | ACC_CONSTRUCTOR
     | ACC_DECLARED_SYNCHRONIZED),
};

#define IS_CLASS(flag) ((flag & ACC_CLASS_MASK) > 0)
#define IS_INNER_CLASS(flag) ((flag & ACC_INNER_CLASS_MASK) > 0)
#define IS_FIELD(flag) ((flag & ACC_FIELD_MASK) > 0)
#define IS_METHOD(flag) ((flag & ACC_METHOD_MASK) > 0)

#define TO_TYPE_STR(flag) (IS_CLASS(flag) ? "class" : IS_INNER_CLASS(flag) \
? "inner class" : IS_FIELD(flag) ? "field" : IS_METHOD(flag) ? "method" : "unknow")

#endif //DEXPARSEDEMO_DEXFILE_H
