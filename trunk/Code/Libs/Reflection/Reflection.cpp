/*
   GameRiff - Framework for creating various video game services
   System for reflecting c++ classes
   Copyright (C) 2011, Shaun Leach.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
  
       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.
  
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Pch.h"

//////////////////////////////////////////////////////
//
// Constants
//

enum EReflIndex {
    REFL_INDEX_BOOL,
    REFL_INDEX_INT8,
    REFL_INDEX_UINT8,
    REFL_INDEX_INT16,
    REFL_INDEX_UINT16,
    REFL_INDEX_INT32,
    REFL_INDEX_UINT32,
    REFL_INDEX_INT64,
    REFL_INDEX_UINT64,
    REFL_INDEX_INT128,
    REFL_INDEX_UINT128,
    REFL_INDEX_FLOAT16,
    REFL_INDEX_FLOAT32,
    REFL_INDEX_STRING,
    REFL_INDEX_CLASS,
    REFL_INDEX_ENUM,
    REFL_INDEX_FIXED_ARRAY,
    REFL_INDEX_VAR_ARRAY,
    REFL_INDEX_POINTER,
    REFL_INDEX_COLOR,
    REFL_INDEX_ANGLE,
    REFL_INDEX_PERCENTAGE,
    REFL_INDEX_EULER_ANGLES,
    REFL_INDEX_VEC3,
    REFL_INDEX_VEC4,
    REFL_INDEX_QUATERNION,
    REFL_INDEX_ENDTYPE,
};

static ReflTypeDesc  * s_descHead  = NULL;
static ReflAlias      * s_classAliasHead = NULL;

ReflHash ReflClass::s_classType(TOWSTR(ReflClass));

//////////////////////////////////////////////////////
//
// Internal Types
//

typedef void (*ToStringFunc)(const ReflMember * desc, const byte * data, const chargr * format, chargr * str, unsigned len);
typedef void (*FromStringFunc)(const ReflMember * desc, byte * data, const chargr * format, const chargr * str, unsigned len);

struct TypeDesc {
    TypeDesc() {
        // Empty constructor seen as the table will have already 
        //   been initialized in the function below by the time
        //   the global constructor is called.
        typeHash = ReflHash(m_typeName);
    }

    TypeDesc(
        EReflIndex        index,
        const chargr    * name,
        unsigned          size,
        const chargr    * inFormat,
        ToStringFunc      toStr,
        FromStringFunc    fromStr
    ) :
        m_index(index),
        m_typeName(name),
        typeSize(size),
        typeHash(name),
        format(inFormat),
        toString(toStr),
        fromString(fromStr)
    {
    }

    const chargr  * GetTypeName(const ReflTypeDesc * desc) const {
        const chargr * name = m_typeName;
        if (m_index == REFL_INDEX_ENUM) {
            ASSERTMSGGR(desc != NULL, "Unregistered enum type");
            name = desc->GetTypeName();
        }
        return name;
    }

    bool TypeMatches(ReflHash givenType, ReflHash memberType) const {
        bool result = false;
        if (m_index == REFL_INDEX_ENUM) 
            result = givenType == memberType;
        else
            result = givenType == typeHash;
        return result;
    }

    ReflHash        typeHash;
    unsigned        typeSize;
    const chargr  * format;
    ToStringFunc    toString;
    FromStringFunc  fromString;

private:
    EReflIndex      m_index;
    const chargr  * m_typeName;
};

static TypeDesc s_typeDesc[REFL_INDEX_ENDTYPE];

//////////////////////////////////////////////////////
//
// Internal Functions
//

//====================================================
template<EReflIndex t_reflType, typename t_dataType>
void ConvertToString(const ReflMember * , const byte * data, const chargr * format, chargr * string, unsigned len) {
    ASSERTMSGGR(format != NULL, "Need to handle this type via specialization");
    StrPrintf(string, len, format, *((t_dataType *) data));
}

//====================================================
template<>
void ConvertToString<REFL_INDEX_BOOL, bool>(const ReflMember * , const byte * data, const chargr * format, chargr * string, unsigned len) {
    StrPrintf(string, len, L"%s", *((bool *) data) ? L"true" : L"false");
}

//====================================================
template<>
void ConvertToString<REFL_INDEX_INT8, int8>(const ReflMember * , const byte * data, const chargr * format, chargr * string, unsigned len) {
    int value = *(reinterpret_cast<const int8 *>(data));
    StrPrintf(string, len, L"%d", value);
}

//====================================================
template<>
void ConvertToString<REFL_INDEX_UINT8, uint8>(const ReflMember * , const byte * data, const chargr * format, chargr * string, unsigned len) {
    unsigned value = *(reinterpret_cast<const uint8 *>(data));
    StrPrintf(string, len, L"%u", value);
}

//====================================================
template<>
void ConvertToString<REFL_INDEX_ENUM, char>(const ReflMember * memberDesc, const byte * data, const chargr * format, chargr * string, unsigned len) {
    int * idata = (int *) data;
    const ReflTypeDesc * enumDesc = ReflLibrary::GetClassDesc(memberDesc->TypeHash());
    ASSERTMSGGR(enumDesc != NULL, "Unregistered enum type");
    const ReflTypeDesc::EnumValue * enumValue = enumDesc->GetEnumValue(*data);
    if (enumValue != NULL) 
        StrPrintf(string, len, L"%s", enumValue->name);
    else {
        ASSERTMSGGR(false, "Unhandled enum value");
        //Need to log a message here?
    }
}

//====================================================
template<>
void ConvertToString<REFL_INDEX_PERCENTAGE, float>(const ReflMember * , const byte * data, const chargr * format, chargr * string, unsigned len) {
    StrPrintf(string, len, L"%3.3f", 100.0f * (*((float *) data)));
}

//====================================================
template<>
void ConvertToString<REFL_INDEX_ANGLE, float>(const ReflMember * , const byte * data, const chargr * format, chargr * string, unsigned len) {
    float * fdata = (float *) data;
    StrPrintf(string, len, L"%3.3f", MathRadiansToDegrees(*fdata));
}

//====================================================
template<EReflIndex t_reflType, typename t_dataType>
void ConvertFromString(const ReflMember * , byte * data, const chargr * format, const chargr * string, unsigned len) {
    ASSERTMSGGR(format != NULL, "Need to handle this type via specialization");
    StrReadValue(string, len, format, (t_dataType *) data);
}

//====================================================
template<>
void ConvertFromString<REFL_INDEX_BOOL, bool>(const ReflMember * , byte * data, const chargr * , const chargr * string, unsigned len) {
    if (StrICmp(string, "true", 4) == 0) 
        *((bool *) data) = true;
    else
        *((bool *) data) = false;
}

//====================================================
template<>
void ConvertFromString<REFL_INDEX_INT8, int8>(const ReflMember * , byte * data, const chargr * , const chargr * string, unsigned len) {
    int value;
    StrReadValue(string, len, L"%d", &value);
    *(reinterpret_cast<int8 *>(data)) = value;
}

//====================================================
template<>
void ConvertFromString<REFL_INDEX_UINT8, uint8>(const ReflMember * , byte * data, const chargr * , const chargr * string, unsigned len) {
    unsigned value;
    StrReadValue(string, len, L"%u", &value);
    *(reinterpret_cast<uint8 *>(data)) = value;
}

//====================================================
template<>
void ConvertFromString<REFL_INDEX_ENUM, char>(const ReflMember * memberDesc, byte * data, const chargr * , const chargr * string, unsigned len) {
    const ReflTypeDesc * enumDesc = ReflLibrary::GetClassDesc(memberDesc->TypeHash());
    ASSERTMSGGR(enumDesc != NULL, "Unregistered enum type");
    const ReflTypeDesc::EnumValue * enumValue = enumDesc->GetEnumValue(string, len);
    if (enumValue != NULL) {
        *((int *) data) = enumValue->value;
    }
    else {
        ASSERTMSGGR(false, "Unregistered enum value");
        //Need to log a message here?
    }
}

//====================================================
template<>
void ConvertFromString<REFL_INDEX_PERCENTAGE, float>(const ReflMember * , byte * data, const chargr * , const chargr * string, unsigned len) {
    float * fdata = (float *) data;
    StrReadValue(string, len, L"%f", fdata);
    *fdata /= 100.0f;
}

//====================================================
template<>
void ConvertFromString<REFL_INDEX_ANGLE, float>(const ReflMember * , byte * data, const chargr * , const chargr * string, unsigned len) {
    float * fdata = (float *) data;
    StrReadValue(string, len, L"%f", fdata);
    *fdata = MathDegreesToRadians(*fdata);
}

//====================================================
/*template<>
void ConvertFromString<REFL_INDEX_CHAR>(byte * data, const chargr * string, unsigned len) {
    swscanf(string, L"%c", (char *) data);
}*/

//====================================================
void InitializeTypeTable() {
    s_typeDesc[REFL_INDEX_BOOL        ] = TypeDesc(REFL_INDEX_BOOL,          L"bool",            sizeof(bool),         NULL,      &ConvertToString<REFL_INDEX_BOOL,           bool>, &ConvertFromString<REFL_INDEX_BOOL,           bool>);
    s_typeDesc[REFL_INDEX_INT8        ] = TypeDesc(REFL_INDEX_INT8,          L"int8",            sizeof(int8),         NULL,      &ConvertToString<REFL_INDEX_INT8,           int8>, &ConvertFromString<REFL_INDEX_INT8,           int8>);
    s_typeDesc[REFL_INDEX_UINT8       ] = TypeDesc(REFL_INDEX_UINT8,         L"uint8",           sizeof(uint8),        NULL,      &ConvertToString<REFL_INDEX_UINT8,         uint8>, &ConvertFromString<REFL_INDEX_UINT8,         uint8>);
    s_typeDesc[REFL_INDEX_INT16       ] = TypeDesc(REFL_INDEX_INT16,         L"int16",           sizeof(int16),        L"%hd",    &ConvertToString<REFL_INDEX_INT16,         int16>, &ConvertFromString<REFL_INDEX_INT16,         int16>);
    s_typeDesc[REFL_INDEX_UINT16      ] = TypeDesc(REFL_INDEX_UINT16,        L"uint16",          sizeof(uint16),       L"%hu",    &ConvertToString<REFL_INDEX_UINT16,       uint16>, &ConvertFromString<REFL_INDEX_UINT16,       uint16>);
    s_typeDesc[REFL_INDEX_INT32       ] = TypeDesc(REFL_INDEX_INT32,         L"int32",           sizeof(int32),        L"%d",     &ConvertToString<REFL_INDEX_INT32,         int32>, &ConvertFromString<REFL_INDEX_INT32,         int32>);
    s_typeDesc[REFL_INDEX_UINT32      ] = TypeDesc(REFL_INDEX_UINT32,        L"uint32",          sizeof(uint32),       L"%u",     &ConvertToString<REFL_INDEX_UINT32,       uint32>, &ConvertFromString<REFL_INDEX_UINT32,       uint32>);
    s_typeDesc[REFL_INDEX_INT64       ] = TypeDesc(REFL_INDEX_INT64,         L"int64",           sizeof(int64),        L"%lld",   &ConvertToString<REFL_INDEX_INT64,         int64>, &ConvertFromString<REFL_INDEX_INT64,         int64>);
    s_typeDesc[REFL_INDEX_UINT64      ] = TypeDesc(REFL_INDEX_UINT64,        L"uint64",          sizeof(uint64),       L"%llu",   &ConvertToString<REFL_INDEX_UINT64,       uint64>, &ConvertFromString<REFL_INDEX_UINT64,       uint64>);
    s_typeDesc[REFL_INDEX_INT128      ] = TypeDesc(REFL_INDEX_INT128,        L"int128",          sizeof(int128),       NULL,      &ConvertToString<REFL_INDEX_INT128,       int128>, &ConvertFromString<REFL_INDEX_INT128,       int128>);
    s_typeDesc[REFL_INDEX_UINT128     ] = TypeDesc(REFL_INDEX_UINT128,       L"uint128",         sizeof(uint128),      NULL,      &ConvertToString<REFL_INDEX_UINT128,     uint128>, &ConvertFromString<REFL_INDEX_UINT128,     uint128>);
    s_typeDesc[REFL_INDEX_FLOAT16     ] = TypeDesc(REFL_INDEX_FLOAT16,       L"float16",         sizeof(float16),      L"%f",     &ConvertToString<REFL_INDEX_FLOAT16,       float>, &ConvertFromString<REFL_INDEX_FLOAT16,       float>);
    s_typeDesc[REFL_INDEX_FLOAT32     ] = TypeDesc(REFL_INDEX_FLOAT32,       L"float32",         sizeof(float32),      L"%f",     &ConvertToString<REFL_INDEX_FLOAT32,       float>, &ConvertFromString<REFL_INDEX_FLOAT32,       float>);
    s_typeDesc[REFL_INDEX_STRING      ] = TypeDesc(REFL_INDEX_STRING,        L"chargr *",        sizeof(chargr *),     L"%s",     &ConvertToString<REFL_INDEX_STRING, const chargr>, &ConvertFromString<REFL_INDEX_STRING, const chargr>);
    s_typeDesc[REFL_INDEX_CLASS       ] = TypeDesc(REFL_INDEX_CLASS,         L"class",           0,                    NULL,      &ConvertToString<REFL_INDEX_CLASS,          char>, &ConvertFromString<REFL_INDEX_CLASS,          char>);
    s_typeDesc[REFL_INDEX_ENUM        ] = TypeDesc(REFL_INDEX_ENUM,          L"enum",            sizeof(unsigned),     NULL,      &ConvertToString<REFL_INDEX_ENUM,           char>, &ConvertFromString<REFL_INDEX_ENUM,           char>);
    s_typeDesc[REFL_INDEX_FIXED_ARRAY ] = TypeDesc(REFL_INDEX_FIXED_ARRAY,   L"fixedarray",      0,                    NULL,      &ConvertToString<REFL_INDEX_FIXED_ARRAY,    char>, &ConvertFromString<REFL_INDEX_FIXED_ARRAY,    char>);
    s_typeDesc[REFL_INDEX_VAR_ARRAY   ] = TypeDesc(REFL_INDEX_VAR_ARRAY,     L"vararray",        0,                    NULL,      &ConvertToString<REFL_INDEX_VAR_ARRAY,      char>, &ConvertFromString<REFL_INDEX_VAR_ARRAY,      char>);
    s_typeDesc[REFL_INDEX_POINTER     ] = TypeDesc(REFL_INDEX_POINTER,       L"pointer",         sizeof(void *),       NULL,      &ConvertToString<REFL_INDEX_POINTER,        char>, &ConvertFromString<REFL_INDEX_POINTER,        char>);
    s_typeDesc[REFL_INDEX_COLOR       ] = TypeDesc(REFL_INDEX_COLOR,         L"color",           sizeof(color),        NULL,      &ConvertToString<REFL_INDEX_COLOR,          char>, &ConvertFromString<REFL_INDEX_COLOR,          char>);
    s_typeDesc[REFL_INDEX_ANGLE       ] = TypeDesc(REFL_INDEX_ANGLE,         L"angle",           sizeof(angle),        NULL,      &ConvertToString<REFL_INDEX_ANGLE,         float>, &ConvertFromString<REFL_INDEX_ANGLE,         float>);
    s_typeDesc[REFL_INDEX_PERCENTAGE  ] = TypeDesc(REFL_INDEX_PERCENTAGE,    L"percentage",      sizeof(percentage),   NULL,      &ConvertToString<REFL_INDEX_PERCENTAGE,    float>, &ConvertFromString<REFL_INDEX_PERCENTAGE,    float>);
    s_typeDesc[REFL_INDEX_EULER_ANGLES] = TypeDesc(REFL_INDEX_EULER_ANGLES,  L"eulers",          sizeof(eulers),       NULL,      &ConvertToString<REFL_INDEX_EULER_ANGLES,  float>, &ConvertFromString<REFL_INDEX_EULER_ANGLES,  float>);
    s_typeDesc[REFL_INDEX_VEC3        ] = TypeDesc(REFL_INDEX_VEC3,          L"vec3",            sizeof(vec3),         NULL,      &ConvertToString<REFL_INDEX_VEC3,          float>, &ConvertFromString<REFL_INDEX_VEC3,          float>);
    s_typeDesc[REFL_INDEX_VEC4        ] = TypeDesc(REFL_INDEX_VEC4,          L"vec4",            sizeof(vec4),         NULL,      &ConvertToString<REFL_INDEX_VEC4,          float>, &ConvertFromString<REFL_INDEX_VEC4,          float>);
    s_typeDesc[REFL_INDEX_QUATERNION  ] = TypeDesc(REFL_INDEX_QUATERNION,    L"quaternion",      sizeof(quaternion),   NULL,      &ConvertToString<REFL_INDEX_QUATERNION,    float>, &ConvertFromString<REFL_INDEX_QUATERNION,    float>);

};

//////////////////////////////////////////////////////
//
// Member Functions
//

//====================================================
ReflMember::ReflMember(
    ReflTypeDesc * container,
    ReflHash        typeHash, 
    const chargr  * name, 
    unsigned        size,
    unsigned        offset
) :
    m_nameHash(name),
    m_name(name),
    m_index(REFL_INDEX_ENDTYPE),
    m_typeHash(typeHash),
    m_offset(offset),
    m_next(NULL),
    m_convFunc(NULL),
    m_deprecated(false),
    m_tempBinding(NULL)
{
    m_index = DetermineTypeIndex(typeHash);

    if (m_index == REFL_INDEX_ENDTYPE) {
        // If it's not registered, assume it's a user type
        m_index = REFL_INDEX_CLASS;
    }
    else{
        ASSERTMSGGR(size == s_typeDesc[m_index].typeSize, "Type sizze doesn't match");
    }

    container->RegisterMember(this);
}

//====================================================
void ReflMember::AdjustOffset(unsigned offset) {
    m_offset -= offset;
}

//====================================================
bool ReflMember::ConvertClassMember(
    IStructuredTextStream * stream, 
    ReflHash                nameHash,
    ReflClass             * inst, 
    ReflIndex               oldType
) const {
    if (stream->ReadChildNode() == STREAM_ERROR_NODEDOESNTEXIST) 
        return true;

    do {
        chargr nodeName[64];
        stream->ReadNodeName(nodeName, 64);

        if (StrICmp(nodeName, L"DataMember", 10) == 0) {
            chargr name[256];
            EStreamError result = stream->ReadNodeAttribute(L"Name", name, 256);
            ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");
            ReflHash nameHash(name);

            chargr type[256];
            result = stream->ReadNodeAttribute(L"Type", type, 256);
            ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");
            ReflHash typeHash(type);

            ReflIndex oldType = DetermineTypeIndex(typeHash);
            ASSERTMSGGR(oldType != REFL_INDEX_ENDTYPE, "Log: Unsupported type?");

            ConvertDataMember(stream, nameHash, inst, oldType);
        }
        else if (StrICmp(nodeName, L"BaseClass", 9) == 0) {
            ConvertClassMember(stream, nameHash, inst, oldType);
        }
    } while (stream->ReadNextNode() != STREAM_ERROR_NODEDOESNTEXIST);

    stream->ReadParentNode();

    return true;
}

//====================================================
bool ReflMember::ConvertDataMember(
    IStructuredTextStream * stream, 
    ReflHash                nameHash,
    ReflClass             * inst, 
    ReflIndex               oldType
) const {
    chargr value[256];
    stream->ReadNodeValue(value, 256);
    byte container[16];
    ASSERTMSGGR(sizeof(container) >= s_typeDesc[oldType].typeSize, "Array is too small");
    s_typeDesc[oldType].fromString(this, container, s_typeDesc[oldType].format, value, 255);
    m_convFunc(inst, nameHash, s_typeDesc[oldType].typeHash, container);

    return true;
}

//====================================================
bool ReflMember::Deserialize(
    IStructuredTextStream * stream, 
    ReflHash                nameHash, 
    ReflClass             * inst,
    void                  * base, 
    unsigned                offset
) const {
    chargr type[256];
    EStreamError result = stream->ReadNodeAttribute(L"Type", type, 256);
    ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");
    ReflHash typeHash(type);

    bool retResult = true;
    if (s_typeDesc[TypeIndex()].TypeMatches(typeHash, m_typeHash)) {
        if (m_index == REFL_INDEX_CLASS) {
            retResult = DeserializeClassMember(stream, base, offset);
        }
        else {
            chargr value[256];
            stream->ReadNodeValue(value, 256);
            byte * member = reinterpret_cast<byte *>(base);
            if (!m_deprecated) 
                member += m_offset + offset;
            else {
                ASSERTMSGGR(m_tempBinding != NULL, "Need to log this");
                member = reinterpret_cast<byte *>(m_tempBinding);
            }
            s_typeDesc[TypeIndex()].fromString(this, member, s_typeDesc[TypeIndex()].format, value, 256);
        }
    }
    else if (m_convFunc != NULL) {
        ReflIndex oldType = DetermineTypeIndex(typeHash);
        ASSERTMSGGR(oldType != REFL_INDEX_ENDTYPE, "Log: Unsupported type?");

        if (oldType == REFL_INDEX_CLASS) {
            if (stream->ReadChildNode() == STREAM_ERROR_NODEDOESNTEXIST) 
                return true;
            retResult = ConvertClassMember(stream, nameHash, inst, oldType);
            stream->ReadParentNode();
        }
        else {
            retResult = ConvertDataMember(stream, nameHash, inst, oldType);
        }
    }
    
    return retResult;
}

//====================================================
bool ReflMember::DeserializeClassMember(IStructuredTextStream * stream, void * base, unsigned offset) const {
    const ReflTypeDesc * subClass = ReflLibrary::GetClassDesc(m_typeHash);
    if (subClass != NULL) {
        if (stream->ReadChildNode() == STREAM_ERROR_NODEDOESNTEXIST) {
            ASSERTMSGGR(false, "Need to log an error here");
            return true;
        }
        chargr nodeName[64];
        stream->ReadNodeName(nodeName, 64);
        if (StrICmp(nodeName, L"Class", 5) != 0) {
            ASSERTMSGGR(false, "Need to log an error here");
            return true;
        }

        chargr typeName[256];
        if (stream->ReadNodeAttribute(L"Type", typeName, 256) == STREAM_ERROR_OK) {
            if (ReflHash(typeName) == m_typeHash) 
                subClass->Deserialize(stream, base, offset + m_offset);
        }
        else 
            ASSERTMSGGR(false, "Need to log this error message");

        stream->ReadParentNode();
    }
    else {
        // Need to log error
        ASSERTMSGGR(false, "Unregistered type, log error");
    }

    return true;
}

//====================================================
ReflIndex ReflMember::DetermineTypeIndex(ReflHash typeHash) const {
    static bool s_typeTableInitialized = false;
    if (!s_typeTableInitialized) {
        InitializeTypeTable();
        s_typeTableInitialized = true;
    }

    ReflIndex type = REFL_INDEX_ENDTYPE;
    for (unsigned i = 0; i < REFL_INDEX_ENDTYPE; i++) {
        if (s_typeDesc[i].typeHash == typeHash) {
            type = static_cast<ReflIndex>(i);
            break;
        }
    }

    return type;
}

//====================================================
void ReflMember::Finalize() {
    if (m_index == REFL_INDEX_CLASS) {
        const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(m_typeHash);
        ASSERTMSGGR(desc != NULL, "Unregistered class type");
        if (desc->IsEnumType()) 
            m_index = REFL_INDEX_ENUM;
    }
}

//====================================================
bool ReflMember::Matches(ReflHash hash) const {
    return m_nameHash == hash;
}

//====================================================
void ReflMember::RegisterConversionFunc(ReflConversionFunc func) {
    ASSERTMSGGR(m_convFunc == NULL, "Conversion fucntion already registered");
    m_convFunc = func;
}

//====================================================
void ReflMember::SetNext(ReflMember * next) {
    m_next = next;
}

//====================================================
bool ReflMember::Serialize(
    IStructuredTextStream * stream, 
    const ReflClass       * inst, 
    const void            * base,
    unsigned                offset
) const {
    if (m_deprecated) 
        return true;

    stream->WriteNode(L"DataMember");
    stream->WriteNodeAttribute(L"Name", Name());
    const ReflTypeDesc * typeDesc = ReflLibrary::GetClassDesc(m_typeHash);
    stream->WriteNodeAttribute(L"Type", s_typeDesc[TypeIndex()].GetTypeName(typeDesc));
    if (m_index == REFL_INDEX_CLASS) {
        if (typeDesc != NULL) 
            typeDesc->Serialize(stream, inst, offset + m_offset);
        else {
            // Need to log error
            ASSERTMSGGR(false, "Unregistered type, log error");
        }
    }
    else {
        chargr value[1024];
        const byte * member = reinterpret_cast<const byte *>(base);
        member += m_offset + offset;
        s_typeDesc[TypeIndex()].toString(this, member, s_typeDesc[TypeIndex()].format, value, 1024);

        stream->WriteNodeValue(value);
    }
    stream->EndNode();
    
    return true;
}

//====================================================
ReflTypeDesc::ReflTypeDesc(
    const chargr  * name, 
    unsigned        size,
    unsigned        baseOffset,
    unsigned        reflOffset,
    ReflCreateFunc  creationFunc
) :
    m_typeName(name),
    m_typeHash(name),
    m_version(1),
    m_size(size),
    m_baseOffset(baseOffset),
    m_reflOffset(reflOffset),
    m_creationFunc(creationFunc),
    m_finalizeFunc(NULL),
    m_versioningFunc(NULL),
    m_members(NULL),
    m_next(NULL),
    m_parents(NULL),
    m_enumValues(NULL)
{
}

//====================================================
void ReflTypeDesc::AddParent(Parent * parent) {
    //parent->offset -= m_baseOffset;
    parent->next = m_parents;
    m_parents = parent;
}

//====================================================
void * ReflTypeDesc::CastToBase(ReflClass * inst) const {
    byte * base = reinterpret_cast<byte *>(inst);
    return reinterpret_cast<void *>(base - m_reflOffset - m_baseOffset);
}

//====================================================
const void * ReflTypeDesc::CastToBase(const ReflClass * inst) const {
    const byte * base = reinterpret_cast<const byte *>(inst);
    return reinterpret_cast<const void *>(base - m_reflOffset - m_baseOffset);
}

//====================================================
ReflClass * ReflTypeDesc::CastToReflClass(void * inst) const {
    byte * base = static_cast<byte *>(inst);
    return reinterpret_cast<ReflClass *>(base + m_reflOffset + m_baseOffset);
}

//====================================================
bool ReflTypeDesc::CalculateCastOffset(ReflHash givenType, ReflHash targetType, int * offset) const {
    bool targetTypeFound    = false;
    if (targetType == m_typeHash) {
        // Casting to the actual type from any type
        *offset = 0;
        targetTypeFound = true;
    }
    else if (givenType == m_typeHash) {
        // Downcasting from our actual type to ReflClass
        if (targetType == ReflClass::GetReflType()) {
            *offset = m_reflOffset + m_baseOffset;
            targetTypeFound = true;
        }
        else {
            // Downcasting from our actual type to a parent type
            unsigned targetOffset       = 0;
            unsigned targetReflOffset   = 0;
            bool targetParentFound  = FindParentOffset(targetType, &targetOffset, &targetReflOffset);
            if (targetParentFound) {
                *offset = targetOffset;
                targetTypeFound = true;
            }
        }
    }
    else {
        // Any to any
        if (givenType == ReflClass::GetReflType()) {
            unsigned targetOffset       = 0;
            unsigned targetReflOffset   = 0;
            bool targetParentFound  = FindParentOffset(targetType, &targetOffset, &targetReflOffset);
            if (targetParentFound) {
                *offset = targetOffset;
                targetTypeFound = true;
            }
        }
        else if (targetType == ReflClass::GetReflType()) {
            *offset = m_reflOffset + m_baseOffset;
            targetTypeFound = true;
        }
        else {
            unsigned targetOffset       = 0;
            unsigned targetReflOffset   = 0;
            bool targetParentFound  = FindParentOffset(targetType, &targetOffset, &targetReflOffset);

            if (targetParentFound) {
                *offset = targetOffset;
                targetTypeFound = true;
            }
        }
    }

    return targetTypeFound;
}

//====================================================
void * ReflTypeDesc::CastTo(ReflClass * inst, ReflHash givenType, ReflHash targetType) const {
    void * base = NULL;
    if (givenType != m_typeHash && givenType != ReflClass::GetReflType()) {
        unsigned offset     = 0;
        unsigned reflOffset = 0;
        bool parentFound = FindParentOffset(givenType, &offset, &reflOffset);
        if (parentFound) 
            base = reinterpret_cast<byte *>(inst) - offset - reflOffset;
    }
    else
        base = CastToBase(inst);
    void * ret = NULL;
    if (base != NULL) 
        ret = CastTo(base, givenType, targetType);
    return ret;
}

//====================================================
const void * ReflTypeDesc::CastTo(const ReflClass * inst, ReflHash givenType, ReflHash targetType) const {
    const void * base = NULL;
    if (givenType != m_typeHash && givenType != ReflClass::GetReflType()) {
        unsigned offset     = 0;
        unsigned reflOffset = 0;
        bool parentFound = FindParentOffset(givenType, &offset, &reflOffset);
        if (parentFound) {
            if (offset != 0) 
                base = reinterpret_cast<const byte *>(inst) - offset - reflOffset;
        }
    }
    else
        base = CastToBase(inst);

    const void * ret = NULL;
    if (base != NULL) 
        ret = CastTo(base, givenType, targetType);
    return ret;
}

//====================================================
void * ReflTypeDesc::CastTo(void * inst, ReflHash givenType, ReflHash targetType) const {

    int offset = 0;
    bool targetTypeFound = CalculateCastOffset(givenType, targetType, &offset);

    void * ret = NULL;
    if (targetTypeFound) {
        byte * ptr = reinterpret_cast<byte *>(inst);
        if (ptr != NULL) 
            ret = reinterpret_cast<void *>(ptr + offset);
    }

    return ret;
}

//====================================================
const void * ReflTypeDesc::CastTo(const void * inst, ReflHash givenType, ReflHash targetType) const {
    TESTME("Make sure const version works");

    int offset = 0;
    bool targetTypeFound = CalculateCastOffset(givenType, targetType, &offset);

    const void * ret = NULL;
    if (targetTypeFound) {
        const byte * ptr = reinterpret_cast<const byte *>(inst);
        if (ptr != NULL) 
            ret = reinterpret_cast<const void *>(ptr + offset);
    }

    return ret;
}

//====================================================
void ReflTypeDesc::ClearAllTempBindings() {
    ReflMember * member = m_members;
    while(member != NULL) {
        member->ClearTempBinding();

        member = member->GetNext();
    }
}

//====================================================
void ReflTypeDesc::ClearTempBinding(ReflHash memberHash, ReflHash typeHash) {
    ReflMember * member = FindMember(memberHash);
    if (member != NULL && member->TypeHash() == typeHash) {
        member->ClearTempBinding();
    }
}

//====================================================
bool ReflTypeDesc::Deserialize(
    IStructuredTextStream * stream, 
    ReflClass             * inst
) const {
    return Deserialize(stream, CastToBase(inst), 0);
}

//====================================================
bool ReflTypeDesc::Deserialize(
    IStructuredTextStream * stream, 
    void                  * inst, 
    unsigned                offset
) const {
    chargr versionStr[32];
    unsigned version = 0;
    if (stream->ReadNodeAttribute(L"Version", versionStr, 32) == STREAM_ERROR_OK) {
        StrReadValue(versionStr, 32, L"%x", &version);
    }
    else 
        ASSERTMSGGR(false, "Need to log this error");

    if (m_versioningFunc != NULL) {
        m_versioningFunc(stream, const_cast<ReflTypeDesc *>(this), version, CastToReflClass(inst));
    }
    else {
        DeserializeMembers(stream, inst, offset);
    }

    FinalizeInst(inst);

    return true;
}

//====================================================
bool ReflTypeDesc::DeserializeMembers(
    IStructuredTextStream * stream, 
    void                  * inst
) const {
    return DeserializeMembers(stream, inst, 0);
}

//====================================================
bool ReflTypeDesc::DeserializeMembers(
    IStructuredTextStream * stream, 
    void                  * inst, 
    unsigned                offset
) const {
    if (stream->ReadChildNode() == STREAM_ERROR_NODEDOESNTEXIST) 
        return true;

    do {
        chargr nodeName[64];
        stream->ReadNodeName(nodeName, 64);

        if (StrICmp(nodeName, L"DataMember", 10) == 0) {

            chargr name[256];
            EStreamError result = stream->ReadNodeAttribute(L"Name", name, 256);
            ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");
            ReflHash nameHash(name);
            unsigned memberOffset = 0;
            const ReflMember * member = FindMember(nameHash, &memberOffset);
            if (member != NULL) {
                member->Deserialize(
                    stream, 
                    nameHash,
                    CastToReflClass(inst),
                    inst, 
                    offset + memberOffset
                );
            }
        }
        else if (StrICmp(nodeName, L"BaseClass", 9) == 0) {
            chargr baseClassName[256];
            EStreamError result = stream->ReadNodeAttribute(L"Type", baseClassName, 256);
            if (result == STREAM_ERROR_OK) {
                const ReflTypeDesc * parentDesc = ReflLibrary::GetClassDesc(ReflHash(baseClassName));
                if (parentDesc != NULL) {
                    Parent * parent = FindParent(parentDesc->GetHash());
                    if (parent != NULL) 
                        parentDesc->Deserialize(stream, inst, offset + parent->baseOffset);
                }
            }
            else {
                ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");
            }
        }
    } while (stream->ReadNextNode() != STREAM_ERROR_NODEDOESNTEXIST);

    stream->ReadParentNode();

    return true;
}

//====================================================
void ReflTypeDesc::Finalize() {
    Parent * parent = NULL;
    while (m_parents != NULL) {
        Parent * next   = m_parents->next;
        m_parents->next = parent;
        parent          = m_parents;

        const ReflTypeDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
        ASSERTMSGGR(parentDesc != NULL, "Unregistered parent descriptor");

        m_parents       = next;
    }
    m_parents = parent;

    if (m_parents != NULL) {
        m_baseOffset = m_parents->baseOffset;
        m_reflOffset = m_parents->reflOffset;
    }

    ReflMember * head = NULL;
    while (m_members != NULL) {
        ReflMember * next = m_members->GetNext();
        m_members->SetNext(head);
        m_members->Finalize();
        head        = m_members;
        m_members   = next;
    }

    m_members = head;

    if (head != NULL) {
        unsigned offset = head->GetOffset();
        head = head->GetNext();
        for (; head != NULL; head = head->GetNext()) {
            ASSERTMSGGR(offset < head->GetOffset() || head->IsDeprecated(), "Members are misordered");
            offset = head->GetOffset();
        }
    }

    EnumValue * enumHead = NULL;
    while (m_enumValues != NULL) {
        EnumValue * next    = m_enumValues->next;
        m_enumValues->next  = enumHead;
        enumHead            = m_enumValues;
        m_enumValues        = next;
    }

    m_enumValues = enumHead;

    for (ReflMember * memb = m_members; memb != NULL; memb = memb->GetNext()) {
        for (ReflMember * comp = memb->GetNext(); comp != NULL; comp = comp->GetNext()) {
            ASSERTMSGGR(StrCmp(memb->Name(), comp->Name(), 256) != 0, "Duplicate members");
            ASSERTMSGGR(memb->NameHash() != comp->NameHash(), "Hash Collision");
        }

        for (Parent * parent = m_parents; parent != NULL; parent = parent->next) {
            const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(parent->parentHash);
            ASSERTMSGGR(desc->FindMember(memb->NameHash()) == NULL, "Derived class is reflecting parent member");
        }

    }
}

//====================================================
void ReflTypeDesc::FinalizeInst(void * inst) const {
    if (m_finalizeFunc != NULL) {
        ReflClass * refl = CastToReflClass(inst);
        m_finalizeFunc(refl);
    }
}

//====================================================
ReflMember * ReflTypeDesc::FindMember(ReflHash nameHash) {
    unsigned offset = 0;
    const ReflMember * member = FindMember(nameHash, &offset);

    return const_cast<ReflMember *>(member);
}

//====================================================
const ReflMember * ReflTypeDesc::FindMember(ReflHash nameHash) const {
    unsigned offset = 0;
    const ReflMember * member = FindMember(nameHash, &offset);

    return member;
}

//====================================================
const ReflMember * ReflTypeDesc::FindMember(const chargr * name, unsigned * offset) const {
    ReflHash nameHash(name);

    const ReflMember * member = FindMember(nameHash, offset);

    return member;
}

//====================================================
const ReflMember * ReflTypeDesc::FindMember(ReflHash nameHash, unsigned * offset) const {
    ASSERTGR(offset != NULL);
    const ReflMember * member = NULL;
    if (m_parents != NULL) {
        for (Parent * parent = m_parents; parent != NULL && member == NULL; parent = parent->next) {
            const ReflTypeDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
            ASSERTMSGGR(parentDesc != NULL, "Missing parent descriptor");
            member = parentDesc->FindMember(nameHash, offset);
            if (member != NULL) 
                *offset += parent->baseOffset;
        }
    }

    if (member == NULL) 
        member = FindLocalMember(nameHash);

    if (member == NULL) {
        for (ReflAlias * alias = m_memberAliases; alias != NULL; alias = alias->next) {
            if (nameHash == alias->oldHash) {
                member = FindLocalMember(alias->newHash);
                break;
            }
        }
    }

    return member;
}

//====================================================
const ReflMember * ReflTypeDesc::FindLocalMember(ReflHash nameHash) const {
    const ReflMember * member = m_members;
    while (member != NULL) {
        if (member->Matches(nameHash)) {
            break;
        }
        member = member->GetNext();
    }
    return member;
}

//====================================================
ReflTypeDesc::Parent * ReflTypeDesc::FindParent(ReflHash parentHash) const {
    Parent * parent = m_parents;
    for (; parent != NULL; parent = parent->next) {
        if (parent->parentHash == parentHash) {
            break;
        }
    }

    return parent;
}

//====================================================
bool ReflTypeDesc::FindParentOffset(ReflHash parentHash, unsigned * offset, unsigned * reflOffset) const {

    bool found = false;
    Parent * retParent = FindParent(parentHash);

    if (retParent != NULL) {
        *offset     = retParent->baseOffset;
        *reflOffset = retParent->reflOffset;
        found = true;
    }
    else {
        Parent * parent = m_parents;
        for (; parent != NULL; parent = parent->next) {
            const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(parent->parentHash);
            unsigned recursiveOffset        = 0;
            unsigned recursiveReflOffset    = 0;
            if (desc->FindParentOffset(parentHash, &recursiveOffset, &recursiveReflOffset)) {
                *offset     += recursiveOffset + parent->baseOffset;
                *reflOffset += recursiveReflOffset + parent->reflOffset;
                found = true;
                break;
            }
        }
    }

    return found;
}

//====================================================
const ReflTypeDesc::EnumValue * ReflTypeDesc::GetEnumValue(int value) const {
    const EnumValue * val = m_enumValues;
    while (val != NULL) {
        if (val->value == value) 
            break;

        val = val->next;
    }

    return val;
}

//====================================================
const ReflTypeDesc::EnumValue * ReflTypeDesc::GetEnumValue(const chargr * str, unsigned len) const {
    const EnumValue * val = m_enumValues;
    while (val != NULL) {
        if (StrICmp(str, val->name, len) == 0) 
            break;

        val = val->next;
    }

    return val;
}

//====================================================
const ReflMember & ReflTypeDesc::GetMember(unsigned index) const {
    TESTME("Implement me");
    return m_members[0];
}

//====================================================
void ReflTypeDesc::InitInst(void * inst) const {
    if (m_parents != NULL) {
        Parent * parent = m_parents;
        for (; parent != NULL; parent = parent->next) {
            byte * ptr = reinterpret_cast<byte *>(inst);
            ReflClass * base = reinterpret_cast<ReflClass *>(ptr + parent->baseOffset + parent->reflOffset);
            base->SetTypeHash(m_typeHash);
        }
    }
    else {
        ReflClass * base = reinterpret_cast<ReflClass *>(inst);
        base->SetTypeHash(m_typeHash);
    }
}

//====================================================
bool ReflTypeDesc::IsEnumType() const {
    return m_enumValues != NULL;
}

//====================================================
unsigned ReflTypeDesc::NumMembers() const {
    TESTME("Implement me");
    unsigned memberCount = 0;

    const ReflTypeDesc::Parent * parent = m_parents;
    while(parent != NULL) {
        const ReflTypeDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
        memberCount += parentDesc->NumMembers();
        parent = parent->next;
    }

    const ReflMember * member = m_members;
    while(member != NULL) {
        memberCount++;
        member = member->GetNext();
    }

    return memberCount;
}

//====================================================
void ReflTypeDesc::RegisterEnumValue(ReflTypeDesc::EnumValue * value) {
    ASSERTGR(value->next == NULL);
    value->next     = m_enumValues;
    m_enumValues    = value;
}

//====================================================
void ReflTypeDesc::RegisterFinalizationFunc(ReflFinalizationFunc func) {
    ASSERTGR(m_finalizeFunc == NULL);
    m_finalizeFunc = func;
}

//====================================================
void ReflTypeDesc::RegisterManualVersioningFunc(ReflVersioningFunc func, unsigned currentVersion) {
    ASSERTGR(m_versioningFunc == NULL);
    m_versioningFunc = func;

    m_version = currentVersion;
}

//====================================================
void ReflTypeDesc::RegisterMemberAlias(ReflAlias * alias) {
    alias->next = m_memberAliases;
    m_memberAliases = alias;
}

//====================================================
void ReflTypeDesc::RegisterMember(ReflMember * member) {
    member->SetNext(m_members);
    m_members = member;
}

//====================================================
bool ReflTypeDesc::RegisterTempBinding(ReflHash memberHash, ReflHash typeHash, void * data) {
    bool bound = false;
    ReflMember * member = FindMember(memberHash);
    if (member != NULL && member->TypeHash() == typeHash) {
        member->RegisterTempBinding(data);
        bound = true;
    }

    return bound;
}

//====================================================
bool ReflTypeDesc::SerializeMembers(
    IStructuredTextStream * stream, 
    const ReflClass       * inst,
    const void            * base, 
    unsigned                offset
) const {
    if (m_parents != NULL) {
        for (Parent * parent = m_parents; parent != NULL; parent = parent->next) {
            const ReflTypeDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
            ASSERTMSGGR(parentDesc != NULL, "Missing parent descriptor");
            stream->WriteNode(L"BaseClass");
            stream->WriteNodeAttribute(L"Type", parentDesc->m_typeName);
            chargr versionStr[32];
            StrPrintf(versionStr, 32, L"0x%x", parentDesc->GetVersion());
            stream->WriteNodeAttribute(L"Version", versionStr);
            parentDesc->SerializeMembers(stream, inst, base, offset + parent->baseOffset);
            stream->EndNode();
        }
    }

    const ReflMember * member = m_members;
    while(member != NULL) {
        member->Serialize(stream, inst, base, offset);

        member = member->GetNext();
    }

    return true;
}

//====================================================
bool ReflTypeDesc::Serialize(
    IStructuredTextStream * stream, 
    const ReflClass       * inst, 
    unsigned                offset
) const {
    stream->WriteNode(L"Class");
    stream->WriteNodeAttribute(L"Type", this->m_typeName);
    chargr versionStr[32];
    StrPrintf(versionStr, 32, L"0x%x", m_version);
    stream->WriteNodeAttribute(L"Version", versionStr);

    SerializeMembers(stream, inst, CastToBase(inst), offset);

    stream->EndNode();

    return true;
}

//====================================================
void ReflTypeDesc::SetNext(ReflTypeDesc * next) {
    // This should only be set once during global initialization
    ASSERTGR(m_next == NULL);
    m_next = next;
}

//====================================================
ReflClass::ReflClass() :
    m_type(TOWSTR(ReflClass))
{
}

//////////////////////////////////////////////////////
//
// External Functions
//

//====================================================
void * ReflCanCastTo(ReflClass * inst, ReflHash actualType, ReflHash givenType, ReflHash targetType) {
    void * ret = NULL;
    const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(actualType);
    if (desc != NULL) 
        ret = desc->CastTo(inst, givenType, targetType);

    return ret;
}

//====================================================
const void * ReflCanCastTo(const ReflClass * inst, ReflHash actualType, ReflHash givenType, ReflHash targetType) {
    const void * ret = NULL;
    const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(actualType);
    if (desc != NULL) 
        ret = desc->CastTo(inst, givenType, targetType);

    return ret;
}

//====================================================
void ReflInitialize() {
    for (ReflTypeDesc * desc = s_descHead; desc != NULL; desc = desc->GetNext()) {
        desc->Finalize();
    }

    for (ReflTypeDesc * desc = s_descHead; desc != NULL; desc = desc->GetNext()) {
        for (ReflTypeDesc * compare = desc->GetNext(); compare != NULL; compare = compare->GetNext()) {
            ASSERTMSGGR(StrCmp(desc->GetTypeName(), compare->GetTypeName(), 256) != 0, "Duplicate class names");
            ASSERTMSGGR(desc->GetHash() != compare->GetHash(), "Hash conflict");
        }

        for (ReflAlias * alias = s_classAliasHead; alias != NULL; alias = alias->next) 
            ASSERTMSGGR(alias->oldHash != desc->GetHash(), "Alias conflicts with existing class");
    }

    for (ReflAlias * alias = s_classAliasHead; alias != NULL; alias = alias->next) {
        const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(alias->newHash);
        ASSERTMSGGR(desc != NULL, "Missing class for alias");
    }
}

//====================================================
void ReflInitType(void * inst, ReflHash type) {
    const ReflTypeDesc * desc = ReflLibrary::GetClassDesc(type);
    if (desc != NULL) 
        desc->InitInst(inst);
}

//====================================================
ReflClass * ReflLibrary::Deserialize(IStructuredTextStream * stream, MemFlags memFlags) {
    ReflClass * ret = NULL;

    do {
        chargr nodeName[64];
        stream->ReadNodeName(nodeName, 64);
        if (StrICmp(nodeName, L"Class", 5) != 0) 
            continue;

        chargr typeName[256];
        if (stream->ReadNodeAttribute(L"Type", typeName, 256) == STREAM_ERROR_OK) {
            const ReflTypeDesc * desc = GetClassDesc(ReflHash(typeName));

            if (desc != NULL) {
                void * base = desc->Create(1, memFlags);

                desc->Deserialize(stream, base, 0);
                void * refl = desc->CastTo(base, desc->GetHash(), ReflClass::GetReflType());
                ret = reinterpret_cast<ReflClass *>(refl);

            }
        }
        else {
            LOG(LOG_PRIORITY_ERROR, "Missing Type node in file: %s", stream->GetName());
            ASSERTMSGGR(false, "Need to log this error message");
        }
    } while (stream->ReadNextNode() != STREAM_ERROR_NODEDOESNTEXIST);

    stream->ReadParentNode();
    return ret;
}

//====================================================
bool ReflLibrary::Deserialize(IStructuredTextStream * stream, ReflClass * inst) {
    const ReflTypeDesc * desc = GetClassDesc(inst);

    return desc->Deserialize(stream, inst);
}

//====================================================
const ReflTypeDesc * ReflLibrary::GetClassDesc(ReflHash nameHash) {
    const ReflTypeDesc * classDesc = s_descHead;
    while(classDesc != NULL) {
        if (classDesc->NameMatches(nameHash))
            break;
        classDesc = classDesc->GetNext();
    }

    if (classDesc == NULL) {
        const ReflAlias * alias = s_classAliasHead;
        for (; alias != NULL; alias = alias->next) {
            if (alias->oldHash == nameHash) {
                classDesc = GetClassDesc(alias->newHash);
                break;
            }
        }
    }
    return classDesc;
}

//====================================================
const ReflTypeDesc * ReflLibrary::GetClassDesc(const ReflClass * inst) {
    return GetClassDesc(inst->GetType());
}

//====================================================
void ReflLibrary::RegisterClassDesc(ReflTypeDesc * classDesc) {
    classDesc->SetNext(s_descHead);
    s_descHead = classDesc;
}

//====================================================
void ReflLibrary::RegisterDeprecatedClassDesc(ReflAlias * classDescAlias) {
    classDescAlias->next = s_classAliasHead;
    s_classAliasHead = classDescAlias;
}

//====================================================
bool ReflLibrary::Serialize(IStructuredTextStream * stream, const ReflClass * inst) {
    const ReflTypeDesc * desc = GetClassDesc(inst);

    return desc->Serialize(stream, inst);
}

