/*
   GameRiff - Framework for creating various video game services
   Interface for service to reflect c++ classes
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

class ReflClass;
class ReflClassDesc;
class DataStream;
class IStructuredTextStream;

enum EReflType {
    REFL_TYPE_BOOL,
    REFL_TYPE_INT8,
    REFL_TYPE_UINT8,
    REFL_TYPE_INT16,
    REFL_TYPE_UINT16,
    REFL_TYPE_INT32,
    REFL_TYPE_UINT32,
    REFL_TYPE_INT64,
    REFL_TYPE_UINT64,
    REFL_TYPE_INT128,
    REFL_TYPE_UINT128,
    REFL_TYPE_FLOAT16,
    REFL_TYPE_FLOAT32,
    REFL_TYPE_STRING,
    REFL_TYPE_CLASS,
    REFL_TYPE_ENUM,
    REFL_TYPE_FIXED_ARRAY,
    REFL_TYPE_VAR_ARRAY,
    REFL_TYPE_POINTER,
    REFL_TYPE_COLOR,
    REFL_TYPE_ANGLE,
    REFL_TYPE_PERCENTAGE,
    REFL_TYPE_EULER_ANGLES,
    REFL_TYPE_VEC3,
    REFL_TYPE_VEC4,
    REFL_TYPE_QUATERNION,
    REFL_TYPE_ENDTYPE,
};

typedef ReflClass * (*CreateFunc)(unsigned count, MemFlags memFlags);
typedef void (*FinalizationFunc)(ReflClass * inst);
typedef void (*ConversionFunc)(ReflClass * inst, Hash64 name, Hash64 oldType, void * data);

struct ReflAlias {
    ReflAlias *             next;
    Hash64                  oldHash;
    Hash64                  newHash;
};

class ReflMember {
public:
    ReflMember(
        ReflClassDesc * container,
        const chargr  * typeName,
        const chargr  * name, 
        unsigned        size,
        unsigned        offset
    );

    EReflType Type() const {
        return m_type;
    }

    const chargr * Name() const {
        return m_name;
    }

    bool Matches(Hash64 hash) const;

    Hash64 NameHash() const {
        return m_nameHash;
    }

    const ReflMember * GetNext() const {
        return m_next;
    }
    ReflMember * GetNext() {
        return m_next;
    }
    void SetNext(ReflMember * next);

    void Finalize();

    bool ConvertToString(const byte * data, chargr * str, unsigned len) const;
    bool ConvertFromString(const byte * data, chargr * str, unsigned len) const;

    bool Serialize(IStructuredTextStream * stream, const ReflClass * inst, unsigned offset) const;
    bool Deserialize(IStructuredTextStream * stream, Hash64 nameHash, ReflClass * inst, unsigned offset) const;

    bool Serialize(DataStream * stream, const ReflClass * inst, unsigned offset) const;
    bool Deserialize(DataStream * stream, Hash64 nameHash, ReflClass * inst, unsigned offset) const;

    struct EnumValue {
        EnumValue     * next;
        unsigned        value;
        const chargr  * name;
        Hash64          nameHash;
    };

    void RegisterEnumValue(EnumValue * value);
    const EnumValue * GetEnumValue(int value) const;
    const EnumValue * GetEnumValue(const chargr * str, unsigned len) const;

    void RegisterConversionFunc(ConversionFunc func);

private:

    bool DeserializeClassMember(IStructuredTextStream * stream, ReflClass * inst, unsigned offset) const;
    bool ConvertDataMember(
        IStructuredTextStream * stream, 
        Hash64                  nameHash,
        ReflClass             * inst, 
        EReflType               oldType
    ) const;
    bool ConvertClassMember(
        IStructuredTextStream * stream, 
        Hash64                  nameHash, 
        ReflClass             * inst, 
        EReflType               oldType
    ) const;

    EReflType DetermineType(Hash64 typeHash) const;

private:
    Hash64          m_nameHash;
    Hash64          m_typeHash;

    const chargr  * m_name;
    EReflType       m_type;

    unsigned        m_size;
    unsigned        m_offset;

    EnumValue     * m_enumValues;
    ReflMember    * m_next;

    ConversionFunc  m_convFunc;
};

class ReflClassDesc {
public:
    ReflClassDesc(
        const chargr  * name, 
        unsigned        size,
        CreateFunc      creationFunc
    );

    void Finalize();

    unsigned            NumMembers() const;
    const ReflMember  & GetMember(unsigned index) const;
    const ReflMember  * FindMember(const chargr * name, unsigned * offset) const;
    const ReflMember  * FindMember(Hash64 name, unsigned * offset) const;

    void RegisterMember(ReflMember * member);

    void RegisterFinalizationFunc(FinalizationFunc finalFunc);

    bool NameMatches(const Hash64 rhs) const {
        return m_nameHash == rhs;
    }

    ReflClass * Create(unsigned count, MemFlags memFlags) const {
        return m_creationFunc(count, memFlags);
    }

    bool Serialize(IStructuredTextStream * stream, const ReflClass * inst, unsigned offset = 0) const;
    bool Deserialize(IStructuredTextStream * stream, ReflClass * inst, unsigned offset = 0) const;
//  class ReflItr {
//  public:
//      ReflMember * operator * ();
//      void operator++();
//
//  private:
//   //   const ReflClassDesc *
//  };

    struct Parent {
        Parent * next;
        unsigned offset;
        Hash64   parentHash;
    };
    void AddParent(Parent * parent);

    const ReflClassDesc * GetNext() const {
        return m_next;
    }
    void SetNext(ReflClassDesc * next);

    Hash64 GetHash() const {
        return m_nameHash;
    }

    void RegisterMemberAlias(ReflAlias * alias);

private:

    bool SerializeMembers(IStructuredTextStream * stream, const ReflClass * inst, unsigned offset) const;
    void FinalizeInst(ReflClass * inst) const;
    const ReflMember  * FindLocalMember(Hash64 name) const;
    Parent * FindParent(Hash64 parentHash) const;

private:
    Hash64            m_nameHash;
    const chargr    * m_name;

    unsigned          m_size;

    CreateFunc        m_creationFunc;
    FinalizationFunc  m_finalizeFunc;

    ReflClassDesc   * m_next;

    Parent          * m_parents;

    ReflMember      * m_members;

    ReflAlias       * m_memberAliases;
};

class ReflClass {
public:
    ReflClass();

    virtual bool Serialize(IStructuredTextStream * stream) = 0;
    virtual bool Deserialize(IStructuredTextStream * stream) = 0;

private:
};

class ReflLibrary {
public:
    static const ReflClassDesc * GetClassDesc(Hash64 nameHash);

    static void RegisterClassDesc(ReflClassDesc * classDesc);
    static void RegisterClassDescAlias(ReflAlias * classDescAlias);

    static ReflClass * Deserialize(IStructuredTextStream * stream, MemFlags memFlags);
};

#define REFL_DEFINE_CLASS(name)                                             \
    private:                                                                \
        static Hash64 s_className;                                          \
    public:                                                                 \
        static ReflClass * Create(unsigned count, MemFlags memFlags);       \
        bool Serialize(IStructuredTextStream * stream);                     \
        bool Deserialize(IStructuredTextStream * stream);                   \
        static const ReflClassDesc & GetReflectionInfo();                   \
        static ReflClassDesc * CreateReflClassDesc()

#define REFL_IMPL_CLASS_INTERNAL(base, name)                                \
    ReflClass * name::Create(unsigned count, MemFlags memFlags) {           \
        return static_cast<base *>(new(memFlags) name[count]);              \
    }                                                                       \
    bool name::Serialize(IStructuredTextStream * stream) {                  \
        const ReflClassDesc * desc = ReflLibrary::GetClassDesc(s_className);\
        return desc->Serialize(stream, static_cast<base *>(this));          \
    }                                                                       \
    bool name::Deserialize(IStructuredTextStream * stream) {                \
        const ReflClassDesc * desc = ReflLibrary::GetClassDesc(s_className);\
        return desc->Deserialize(stream, static_cast<base *>(this));        \
    }                                                                       \
    Hash64 name::s_className = Hash64(TOWSTR(name))

#define REFL_IMPL_CLASS_BEGIN(base, name)                                   \
    REFL_IMPL_CLASS_INTERNAL(base, name);                                   \
    ReflClassDesc * name::CreateReflClassDesc() {                           \
        static ReflClassDesc s_reflInfo(                                    \
            TOWSTR(name),                                                   \
            sizeof(name),                                                   \
            name::Create                                                    \
    )
                                
#define REFL_IMPL_CLASS_BEGIN_NAMESPACE(base, ns, name)                     \
    REFL_IMPL_CLASS_INTERNAL(base, name);                                   \
    ReflClassDesc * name::CreateReflClassDesc() {                           \
        static ReflClassDesc s_reflInfo(                                    \
            TOWSTR(ns::name),                                               \
            sizeof(name),                                                   \
            name::Create                                                    \
        )
                                
#define REFL_IMPL_CLASS_END(name)                                           \
        return &s_reflInfo;                                                 \
    }                                                                       \
    class ReflAutoRegister##name {                                          \
    public:                                                                 \
        ReflAutoRegister##name() {                                          \
            ReflLibrary::RegisterClassDesc(name::CreateReflClassDesc());    \
        }                                                                   \
    };                                                                      \
    static ReflAutoRegister##name s_autoRegister##name

#define REFL_ADD_CLASS_ALIAS(name, alias)                                   \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                Hash64(TOWSTR(alias)),                                      \
                Hash64(TOWSTR(name))                                        \
            };                                                              \
            ReflLibrary::RegisterClassDescAlias(&s_alias##alias)

#define REFL_ADD_CLASS_ALIAS_NAMESPACE(name, ns, alias)                     \
            static ReflClassDescAlias s_alias##alias = {                    \
                NULL,                                                       \
                Hash64(TOWSTR(ns::alias)),                                  \
                Hash64(TOWSTR(name))                                        \
            };                                                              \
            ReflLibrary::RegisterClassDescAlias(&s_alias##alias)

#define REFL_ADD_PARENT(derived, parent)                                    \
            static ReflClassDesc::Parent s_parent##parent = {               \
                NULL,                                                       \
                CLASSOFFSETOF(parent, derived),                             \
                Hash64(TOWSTR(parent))                                      \
            };                                                              \
            s_reflInfo.AddParent(&s_parent##parent)

#define REFL_ADD_PARENT_NAMESPACE(derived, ns, parent)                      \
            static ReflClassDesc::Parent s_parent##parent = {               \
                NULL,                                                       \
                CLASSOFFSETOF(parent, derived),                             \
                Hash64(TOWSTR(ns::parent))                                  \
            };                                                              \
            s_reflInfo.AddParent(&s_parent##parent)

#define REFL_FINALIZATION_FUNC(func)                                        \
            s_reflInfo.RegisterFinalizationFunc(func)

#define REFL_MEMBER(parent, name, type)                                     \
            static ReflMember s_member##name(                               \
                &s_reflInfo,                                                \
                TOWSTR(type),                                               \
                TOWSTR(name),                                               \
                SIZEOF(parent, name),                                       \
                OFFSETOF(parent, name)                                      \
            )

#define REFL_ADD_MEMBER_CONVERSION(name, conv)                              \
            s_member##name.RegisterConversionFunc(conv)

#define REFL_ADD_MEMBER_ALIAS(name, alias)                                  \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                Hash64(TOWSTR(alias)),                                      \
                Hash64(TOWSTR(name))                                        \
            };                                                              \
            s_reflInfo.RegisterMemberAlias(&s_alias##alias)

#define REFL_ADD_MEMBER_ALIAS_W_CONVERSION(name, alias, conv)               \
            REFL_ADD_MEMBER_ALIAS(name, alias);                             \
            REFL_ADD_MEMBER_CONVERSION(name, conv)

#define REFL_ENUM_VALUE(enumName, value)                                    \
            static ReflMember::EnumValue s_enumValue##value = {             \
                NULL, value, TOWSTR(value), Hash64(TOWSTR(value))           \
            };                                                              \
            s_member##enumName.RegisterEnumValue(&s_enumValue##value)

#define REFL_ENUM_ALIAS(enumName, value, oldValue)                          \
            static ReflMember::EnumValue s_enumValue##oldValue = {          \
                NULL, value, TOWSTR(oldValue), Hash64(TOWSTR(oldValue))     \
            };                                                              \
            s_member##enumName.RegisterEnumValue(&s_enumValue##oldValue)

