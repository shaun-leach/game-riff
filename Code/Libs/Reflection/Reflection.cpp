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

static ReflClassDesc  * s_descHead  = NULL;
static ReflAlias      * s_classAliasHead = NULL;

//////////////////////////////////////////////////////
//
// Internal Types
//

typedef void (*ToStringFunc)(const ReflMember * desc, const byte * data, const chargr * format, chargr * str, unsigned len);
typedef void (*FromStringFunc)(const ReflMember * desc, byte * data, const chargr * format, const chargr * str, unsigned len);

struct TypeDesc {
    TypeDesc()
    {
        // Empty constructor seen as the table will have already 
        //   been initialized in the function below by the time
        //   the global constructor is called.
        typeHash = ReflHash(typeName);
    }

    TypeDesc(
        const chargr    * name,
        unsigned          size,
        const chargr    * inFormat,
        ToStringFunc      toStr,
        FromStringFunc    fromStr
    ) :
        typeName(name),
        typeSize(size),
        typeHash(name),
        format(inFormat),
        toString(toStr),
        fromString(fromStr)
    {
    }
    const chargr  * typeName;
    ReflHash        typeHash;
    unsigned        typeSize;
    const chargr  * format;
    ToStringFunc    toString;
    FromStringFunc  fromString;
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
    const ReflMember::EnumValue * enumValue = memberDesc->GetEnumValue(*data);
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
    const ReflMember::EnumValue * value = memberDesc->GetEnumValue(string, len);
    if (value != NULL) {
        *((int *) data) = value->value;
    }
    else {
        ASSERTMSGGR(false, "Unhandled enum value");
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
    s_typeDesc[REFL_INDEX_BOOL        ] = TypeDesc(L"bool",            sizeof(bool),         NULL,      &ConvertToString<REFL_INDEX_BOOL,           bool>, &ConvertFromString<REFL_INDEX_BOOL,           bool>);
    s_typeDesc[REFL_INDEX_INT8        ] = TypeDesc(L"int8",            sizeof(int8),         NULL,      &ConvertToString<REFL_INDEX_INT8,           int8>, &ConvertFromString<REFL_INDEX_INT8,           int8>);
    s_typeDesc[REFL_INDEX_UINT8       ] = TypeDesc(L"uint8",           sizeof(uint8),        NULL,      &ConvertToString<REFL_INDEX_UINT8,         uint8>, &ConvertFromString<REFL_INDEX_UINT8,         uint8>);
    s_typeDesc[REFL_INDEX_INT16       ] = TypeDesc(L"int16",           sizeof(int16),        L"%hd",    &ConvertToString<REFL_INDEX_INT16,         int16>, &ConvertFromString<REFL_INDEX_INT16,         int16>);
    s_typeDesc[REFL_INDEX_UINT16      ] = TypeDesc(L"uint16",          sizeof(uint16),       L"%hu",    &ConvertToString<REFL_INDEX_UINT16,       uint16>, &ConvertFromString<REFL_INDEX_UINT16,       uint16>);
    s_typeDesc[REFL_INDEX_INT32       ] = TypeDesc(L"int32",           sizeof(int32),        L"%d",     &ConvertToString<REFL_INDEX_INT32,         int32>, &ConvertFromString<REFL_INDEX_INT32,         int32>);
    s_typeDesc[REFL_INDEX_UINT32      ] = TypeDesc(L"uint32",          sizeof(uint32),       L"%u",     &ConvertToString<REFL_INDEX_UINT32,       uint32>, &ConvertFromString<REFL_INDEX_UINT32,       uint32>);
    s_typeDesc[REFL_INDEX_INT64       ] = TypeDesc(L"int64",           sizeof(int64),        L"%lld",   &ConvertToString<REFL_INDEX_INT64,         int64>, &ConvertFromString<REFL_INDEX_INT64,         int64>);
    s_typeDesc[REFL_INDEX_UINT64      ] = TypeDesc(L"uint64",          sizeof(uint64),       L"%llu",   &ConvertToString<REFL_INDEX_UINT64,       uint64>, &ConvertFromString<REFL_INDEX_UINT64,       uint64>);
    s_typeDesc[REFL_INDEX_INT128      ] = TypeDesc(L"int128",          sizeof(int128),       NULL,      &ConvertToString<REFL_INDEX_INT128,       int128>, &ConvertFromString<REFL_INDEX_INT128,       int128>);
    s_typeDesc[REFL_INDEX_UINT128     ] = TypeDesc(L"uint128",         sizeof(uint128),      NULL,      &ConvertToString<REFL_INDEX_UINT128,     uint128>, &ConvertFromString<REFL_INDEX_UINT128,     uint128>);
    s_typeDesc[REFL_INDEX_FLOAT16     ] = TypeDesc(L"float16",         sizeof(float16),      L"%f",     &ConvertToString<REFL_INDEX_FLOAT16,       float>, &ConvertFromString<REFL_INDEX_FLOAT16,       float>);
    s_typeDesc[REFL_INDEX_FLOAT32     ] = TypeDesc(L"float32",         sizeof(float32),      L"%f",     &ConvertToString<REFL_INDEX_FLOAT32,       float>, &ConvertFromString<REFL_INDEX_FLOAT32,       float>);
    s_typeDesc[REFL_INDEX_STRING      ] = TypeDesc(L"chargr *",        sizeof(chargr *),     L"%s",     &ConvertToString<REFL_INDEX_STRING, const chargr>, &ConvertFromString<REFL_INDEX_STRING, const chargr>);
    s_typeDesc[REFL_INDEX_CLASS       ] = TypeDesc(L"class",           0,                    NULL,      &ConvertToString<REFL_INDEX_CLASS,          char>, &ConvertFromString<REFL_INDEX_CLASS,          char>);
    s_typeDesc[REFL_INDEX_ENUM        ] = TypeDesc(L"enum",            sizeof(unsigned),     NULL,      &ConvertToString<REFL_INDEX_ENUM,           char>, &ConvertFromString<REFL_INDEX_ENUM,           char>);
    s_typeDesc[REFL_INDEX_FIXED_ARRAY ] = TypeDesc(L"fixedarray",      0,                    NULL,      &ConvertToString<REFL_INDEX_FIXED_ARRAY,    char>, &ConvertFromString<REFL_INDEX_FIXED_ARRAY,    char>);
    s_typeDesc[REFL_INDEX_VAR_ARRAY   ] = TypeDesc(L"vararray",        0,                    NULL,      &ConvertToString<REFL_INDEX_VAR_ARRAY,      char>, &ConvertFromString<REFL_INDEX_VAR_ARRAY,      char>);
    s_typeDesc[REFL_INDEX_POINTER     ] = TypeDesc(L"pointer",         sizeof(void *),       NULL,      &ConvertToString<REFL_INDEX_POINTER,        char>, &ConvertFromString<REFL_INDEX_POINTER,        char>);
    s_typeDesc[REFL_INDEX_COLOR       ] = TypeDesc(L"color",           sizeof(color),        NULL,      &ConvertToString<REFL_INDEX_COLOR,          char>, &ConvertFromString<REFL_INDEX_COLOR,          char>);
    s_typeDesc[REFL_INDEX_ANGLE       ] = TypeDesc(L"angle",           sizeof(angle),        NULL,      &ConvertToString<REFL_INDEX_ANGLE,         float>, &ConvertFromString<REFL_INDEX_ANGLE,         float>);
    s_typeDesc[REFL_INDEX_PERCENTAGE  ] = TypeDesc(L"percentage",      sizeof(percentage),   NULL,      &ConvertToString<REFL_INDEX_PERCENTAGE,    float>, &ConvertFromString<REFL_INDEX_PERCENTAGE,    float>);
    s_typeDesc[REFL_INDEX_EULER_ANGLES] = TypeDesc(L"eulers",          sizeof(eulers),       NULL,      &ConvertToString<REFL_INDEX_EULER_ANGLES,  float>, &ConvertFromString<REFL_INDEX_EULER_ANGLES,  float>);
    s_typeDesc[REFL_INDEX_VEC3        ] = TypeDesc(L"vec3",            sizeof(vec3),         NULL,      &ConvertToString<REFL_INDEX_VEC3,          float>, &ConvertFromString<REFL_INDEX_VEC3,          float>);
    s_typeDesc[REFL_INDEX_VEC4        ] = TypeDesc(L"vec4",            sizeof(vec4),         NULL,      &ConvertToString<REFL_INDEX_VEC4,          float>, &ConvertFromString<REFL_INDEX_VEC4,          float>);
    s_typeDesc[REFL_INDEX_QUATERNION  ] = TypeDesc(L"quaternion",      sizeof(quaternion),   NULL,      &ConvertToString<REFL_INDEX_QUATERNION,    float>, &ConvertFromString<REFL_INDEX_QUATERNION,    float>);

};

//////////////////////////////////////////////////////
//
// Member Functions
//

//====================================================
ReflMember::ReflMember(
    ReflClassDesc * container,
    const chargr  * tname, 
    const chargr  * name, 
    unsigned        size,
    unsigned        offset
) :
    m_nameHash(name),
    m_name(name),
    m_index(REFL_INDEX_ENDTYPE),
    m_typeHash(tname),
    m_size(size),
    m_offset(offset),
    m_enumValues(NULL),
    m_next(NULL),
    m_convFunc(NULL)
{
    static bool s_typeTableInitialized = false;
    if (!s_typeTableInitialized) {
        InitializeTypeTable();
        s_typeTableInitialized = true;
    }

    for(unsigned i = 0; i < REFL_INDEX_ENDTYPE; i++) {
        if (m_typeHash == s_typeDesc[i].typeHash) {
            m_index = i;
            break;
        }
    }

    if (m_index == REFL_INDEX_ENDTYPE) {
        // If it's not registered, assume it's a user type
        m_index = REFL_INDEX_CLASS;
    }

    container->RegisterMember(this);
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

            ReflIndex oldType = DetermineType(typeHash);

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
    unsigned                offset
) const {
    chargr type[256];
    EStreamError result = stream->ReadNodeAttribute(L"Type", type, 256);
    ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");
    ReflHash typeHash(type);

    bool retResult = true;
    if (typeHash == s_typeDesc[Type()].typeHash) {
        if (m_index != REFL_INDEX_CLASS) {
            chargr value[256];
            stream->ReadNodeValue(value, 256);
            byte * member = reinterpret_cast<byte *>(inst);
            member += m_offset + offset;
            s_typeDesc[Type()].fromString(this, member, s_typeDesc[Type()].format, value, 256);
        }
        else {
            retResult = DeserializeClassMember(stream, inst, offset);
        }
    }
    else if (m_convFunc != NULL) {
        ReflIndex oldType = DetermineType(typeHash);

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
bool ReflMember::DeserializeClassMember(IStructuredTextStream * stream, ReflClass * inst, unsigned offset) const {
    const ReflClassDesc * subClass = ReflLibrary::GetClassDesc(m_typeHash);
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
                subClass->Deserialize(stream, inst, offset + m_offset);
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
ReflIndex ReflMember::DetermineType(ReflHash typeHash) const {
    ReflIndex type = REFL_INDEX_ENDTYPE;
    for (unsigned i = 0; i < REFL_INDEX_ENDTYPE; i++) {
        if (s_typeDesc[i].typeHash == typeHash) {
            type = static_cast<ReflIndex>(i);
            break;
        }
    }
    ASSERTMSGGR(type != REFL_INDEX_ENDTYPE, "Log: Unsupported type?");

    return type;
}

//====================================================
void ReflMember::Finalize() {
    EnumValue * head = NULL;
    while (m_enumValues != NULL) {
        EnumValue * next = m_enumValues->next;
        m_enumValues->next  = head;
        head                = m_enumValues;
        m_enumValues = next;
    }

    m_enumValues = head;
}

//====================================================
const ReflMember::EnumValue * ReflMember::GetEnumValue(int value) const {
    const EnumValue * val = m_enumValues;
    while (val != NULL) {
        if (val->value == value) 
            break;

        val = val->next;
    }

    return val;
}

//====================================================
const ReflMember::EnumValue * ReflMember::GetEnumValue(const chargr * str, unsigned len) const {
    const EnumValue * val = m_enumValues;
    while (val != NULL) {
        if (StrICmp(str, val->name, len) == 0) 
            break;

        val = val->next;
    }

    return val;
}

//====================================================
bool ReflMember::Matches(ReflHash hash) const {
    return m_nameHash == hash;
}

//====================================================
void ReflMember::RegisterConversionFunc(ConversionFunc func) {
    ASSERTMSGGR(m_convFunc == NULL, "Conversion fucntion already registered");
    m_convFunc = func;
}

//====================================================
void ReflMember::RegisterEnumValue(ReflMember::EnumValue * value) {
    value->next     = m_enumValues;
    m_enumValues    = value;
}

//====================================================
void ReflMember::SetNext(ReflMember * next) {
    m_next = next;
}

//====================================================
bool ReflMember::Serialize(IStructuredTextStream * stream, const ReflClass * inst, unsigned offset) const {
    stream->WriteNode(L"DataMember");
    stream->WriteNodeAttribute(L"Name", Name());
    stream->WriteNodeAttribute(L"Type", s_typeDesc[Type()].typeName);
    if (m_index == REFL_INDEX_CLASS) {
        const ReflClassDesc * subClass = ReflLibrary::GetClassDesc(m_typeHash);
        if (subClass != NULL) 
            subClass->Serialize(stream, inst, offset + m_offset);
        else {
            // Need to log error
            ASSERTMSGGR(false, "Unregistered type, log error");
        }
    }
    else {
        chargr value[1024];
        const byte * member = reinterpret_cast<const byte *>(inst);
        member += m_offset + offset;
        s_typeDesc[Type()].toString(this, member, s_typeDesc[Type()].format, value, 1024);

        stream->WriteNodeValue(value);
    }
    stream->EndNode();
    
    return true;
}

//====================================================
ReflClassDesc::ReflClassDesc(
    const chargr  * name, 
    unsigned        size,
    CreateFunc      creationFunc
) :
    m_name(name),
    m_nameHash(name),
    m_size(size),   
    m_creationFunc(creationFunc),
    m_finalizeFunc(NULL),
    m_members(NULL),
    m_next(NULL),
    m_parents(NULL)
{
}

//====================================================
void ReflClassDesc::AddParent(Parent * parent) {
    parent->next = m_parents;
    m_parents = parent;
}

//====================================================
bool ReflClassDesc::Deserialize(IStructuredTextStream * stream, ReflClass * inst, unsigned offset) const {
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
                    inst, 
                    offset + memberOffset
                );
            }
        }
        else if (StrICmp(nodeName, L"BaseClass", 9) == 0) {
            chargr baseClassName[256];
            EStreamError result = stream->ReadNodeAttribute(L"Type", baseClassName, 256);
            ASSERTMSGGR(result == STREAM_ERROR_OK, "Need to log this error message");

            const ReflClassDesc * parentDesc = ReflLibrary::GetClassDesc(ReflHash(baseClassName));
            if (parentDesc != NULL) {
                Parent * parent = FindParent(parentDesc->GetHash());
                if (parent != NULL) 
                    parentDesc->Deserialize(stream, inst, offset + parent->offset);
            }
        }
    } while (stream->ReadNextNode() != STREAM_ERROR_NODEDOESNTEXIST);

    stream->ReadParentNode();

    FinalizeInst(inst);

    return true;
}

//====================================================
void ReflClassDesc::Finalize() {
    Parent * parent = NULL;
    while (m_parents != NULL) {
        Parent * next   = m_parents->next;
        m_parents->next = parent;
        parent          = m_parents;
        m_parents       = next;
    }
    m_parents = parent;

    ReflMember * head = NULL;
    while (m_members != NULL) {
        ReflMember * next = m_members->GetNext();
        m_members->SetNext(head);
        m_members->Finalize();
        head        = m_members;
        m_members   = next;
    }

    m_members = head;
}

//====================================================
void ReflClassDesc::FinalizeInst(ReflClass * inst) const {
    if (m_finalizeFunc != NULL) {
        m_finalizeFunc(inst);
    }
}

//====================================================
const ReflMember * ReflClassDesc::FindMember(const chargr * name, unsigned * offset) const {
    ReflHash nameHash(name);

    const ReflMember * member = FindMember(nameHash, offset);

    return member;
}

//====================================================
const ReflMember * ReflClassDesc::FindMember(ReflHash nameHash, unsigned * offset) const {
    ASSERTGR(offset != NULL);
    const ReflMember * member = NULL;
    if (m_parents != NULL) {
        for (Parent * parent = m_parents; parent != NULL && member == NULL; parent = parent->next) {
            const ReflClassDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
            ASSERTMSGGR(parentDesc != NULL, "Missing parent descriptor");
            member = parentDesc->FindMember(nameHash, offset);
            if (member != NULL) 
                *offset += parent->offset;
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
const ReflMember * ReflClassDesc::FindLocalMember(ReflHash nameHash) const {
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
ReflClassDesc::Parent * ReflClassDesc::FindParent(ReflHash parentHash) const {
    Parent * parent = m_parents;
    for (; parent != NULL; parent = parent->next) {
        if (parent->parentHash == parentHash) {
            break;
        }
    }

    return parent;
}

//====================================================
const ReflMember & ReflClassDesc::GetMember(unsigned index) const {
    TESTME("Implement me");
    return m_members[0];
}

//====================================================
unsigned ReflClassDesc::NumMembers() const {
    TESTME("Implement me");
    unsigned memberCount = 0;

    const ReflClassDesc::Parent * parent = m_parents;
    while(parent != NULL) {
        const ReflClassDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
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
void ReflClassDesc::RegisterMemberAlias(ReflAlias * alias) {
    alias->next = m_memberAliases;
    m_memberAliases = alias;
}

//====================================================
void ReflClassDesc::RegisterFinalizationFunc(FinalizationFunc func) {
    ASSERTGR(m_finalizeFunc == NULL);
    m_finalizeFunc = func;
}

//====================================================
void ReflClassDesc::RegisterMember(ReflMember * member) {
    member->SetNext(m_members);
    m_members = member;
}

//====================================================
bool ReflClassDesc::SerializeMembers(
    IStructuredTextStream * stream, 
    const ReflClass       * inst, 
    unsigned                offset
) const {
    const ReflClassDesc * parent = NULL;
    if (m_parents != NULL) {
        for (Parent * parent = m_parents; parent != NULL; parent = parent->next) {
            const ReflClassDesc * parentDesc = ReflLibrary::GetClassDesc(parent->parentHash);
            ASSERTMSGGR(parentDesc != NULL, "Missing parent descriptor");
            stream->WriteNode(L"BaseClass");
            stream->WriteNodeAttribute(L"Type", parentDesc->m_name);
            parentDesc->SerializeMembers(stream, inst, offset + parent->offset);
            stream->EndNode();
        }
    }

    const ReflMember * member = m_members;
    while(member != NULL) {
        member->Serialize(stream, inst, offset);

        member = member->GetNext();
    }

    return true;
}

//====================================================
bool ReflClassDesc::Serialize(
    IStructuredTextStream * stream, 
    const ReflClass       * inst, 
    unsigned                offset
) const {
    stream->WriteNode(L"Class");
    stream->WriteNodeAttribute(L"Type", this->m_name);

    SerializeMembers(stream, inst, offset);

    stream->EndNode();

    return true;
}

//====================================================
void ReflClassDesc::SetNext(ReflClassDesc * next) {
    // This should only be set once during global initialization
    ASSERTGR(m_next == NULL);
    m_next = next;
}

//====================================================
ReflClass::ReflClass() {
}

//////////////////////////////////////////////////////
//
// External Functions
//

//====================================================
const ReflClassDesc * ReflLibrary::GetClassDesc(ReflHash nameHash) {
    const ReflClassDesc * classDesc = s_descHead;
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
void ReflLibrary::RegisterClassDesc(ReflClassDesc * classDesc) {
    classDesc->SetNext(s_descHead);
    s_descHead = classDesc;

    classDesc->Finalize();
}

//====================================================
void ReflLibrary::RegisterClassDescAlias(ReflAlias * classDescAlias) {
    classDescAlias->next = s_classAliasHead;
    s_classAliasHead = classDescAlias;
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
            const ReflClassDesc * desc = GetClassDesc(ReflHash(typeName));

            if (desc != NULL) {
                ret = desc->Create(1, memFlags);

                desc->Deserialize(stream, ret, 0);
            }
        }
        else
            ASSERTMSGGR(false, "Need to log this error message");
    } while (stream->ReadNextNode() != STREAM_ERROR_NODEDOESNTEXIST);

    stream->ReadParentNode();
    return ret;
}

