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

typedef unsigned    ReflIndex;
typedef Hash32      ReflHash;

typedef ReflClass * (*ReflCreateFunc)(unsigned count, MemFlags memFlags);
typedef void (*ReflFinalizationFunc)(ReflClass * inst);
typedef void (*ReflConversionFunc)(ReflClass * inst, ReflHash name, ReflHash oldType, void * data);
typedef void (*ReflVersioningFunc)(IStructuredTextStream * stream, ReflClassDesc * desc, unsigned version, ReflClass * inst);

const ReflHash ReflTypeBool(L"bool");
const ReflHash ReflTypeInt32(L"int32");

struct ReflAlias {
    ReflAlias *             next;
    ReflHash                oldHash;
    ReflHash                newHash;
};

class ReflMember {
public:
    ReflMember(
        ReflClassDesc * container,
        ReflHash        typeHash,
        const chargr  * name, 
        unsigned        size,
        unsigned        offset
    );

    ReflMember(
        ReflClassDesc * container,
        ReflHash        typeHash,
        const chargr  * name, 
        unsigned        size,
        bool            deprecated
    );

    const chargr * Name() const {
        return m_name;
    }

    bool Matches(ReflHash hash) const;

    ReflHash NameHash() const {
        return m_nameHash;
    }

    ReflHash TypeHash() const {
        return m_typeHash;
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
    bool Deserialize(IStructuredTextStream * stream, ReflHash nameHash, ReflClass * inst, unsigned offset) const;

    bool Serialize(DataStream * stream, const ReflClass * inst, unsigned offset) const;
    bool Deserialize(DataStream * stream, ReflHash nameHash, ReflClass * inst, unsigned offset) const;

    struct EnumValue {
        EnumValue     * next;
        unsigned        value;
        const chargr  * name;
        ReflHash        nameHash;
    };

    void RegisterEnumValue(EnumValue * value);
    const EnumValue * GetEnumValue(int value) const;
    const EnumValue * GetEnumValue(const chargr * str, unsigned len) const;

    void RegisterConversionFunc(ReflConversionFunc func);

    void RegisterTempBinding(void * data) {
        m_tempBinding = data;
    }
    void ClearTempBinding() {
        m_tempBinding = NULL;
    }

private:

    bool DeserializeClassMember(IStructuredTextStream * stream, ReflClass * inst, unsigned offset) const;
    bool ConvertDataMember(
        IStructuredTextStream * stream, 
        ReflHash                nameHash,
        ReflClass             * inst, 
        ReflIndex               oldType
    ) const;
    bool ConvertClassMember(
        IStructuredTextStream * stream, 
        ReflHash                nameHash, 
        ReflClass             * inst, 
        ReflIndex               oldType
    ) const;

    ReflIndex DetermineTypeIndex(ReflHash typeHash) const;

    ReflIndex TypeIndex() const {
        return m_index;
    }

private:
    ReflHash        m_nameHash;
    ReflHash        m_typeHash;

    const chargr  * m_name;
    ReflIndex       m_index;

    unsigned        m_size;
    unsigned        m_offset;

    EnumValue     * m_enumValues;
    ReflMember    * m_next;

    ReflConversionFunc  m_convFunc;

    bool            m_deprecated; // Need bit flags class
    void          * m_tempBinding;
};

class ReflClassDesc {
public:
    ReflClassDesc(
        const chargr  * name, 
        unsigned        size,
        ReflCreateFunc  creationFunc
    );

    void Finalize();

    unsigned            NumMembers() const;
    const ReflMember  & GetMember(unsigned index) const;
    const ReflMember  * FindMember(ReflHash name) const;

    void RegisterMember(ReflMember * member);

    void RegisterFinalizationFunc(ReflFinalizationFunc finalFunc);
    void RegisterManualVersioningFunc(ReflVersioningFunc loadFunc, unsigned currentVersion);

    bool NameMatches(const ReflHash rhs) const {
        return m_nameHash == rhs;
    }

    ReflClass * Create(unsigned count, MemFlags memFlags) const {
        return m_creationFunc(count, memFlags);
    }

    bool Serialize(IStructuredTextStream * stream, const ReflClass * inst, unsigned offset = 0) const;
    bool Deserialize(IStructuredTextStream * stream, ReflClass * inst) const;
    bool Deserialize(IStructuredTextStream * stream, ReflClass * inst, unsigned offset) const;
    bool DeserializeMembers(IStructuredTextStream * stream, ReflClass * inst) const;
    bool DeserializeMembers(IStructuredTextStream * stream, ReflClass * inst, unsigned offset) const;
//  class ReflItr {
//  public:
//      ReflMember * operator * ();
//      void operator++();
//
//  private:
//   //   const ReflClassDesc *
//  };

    bool RegisterTempBinding(ReflHash memberHash, ReflHash typeHash, void * data);
    void ClearTempBinding(ReflHash memberHash, ReflHash typeHash);
    void ClearAllTempBindings();

    struct Parent {
        Parent    * next;
        unsigned    offset;
        ReflHash    parentHash;
    };
    void AddParent(Parent * parent);

    const ReflClassDesc * GetNext() const {
        return m_next;
    }
    void SetNext(ReflClassDesc * next);

    ReflHash GetHash() const {
        return m_nameHash;
    }

    unsigned GetVersion() const {
        return m_version;
    }

    void RegisterMemberAlias(ReflAlias * alias);

private:

    void FinalizeInst(ReflClass * inst) const;
    const ReflMember  * FindMember(const chargr * name, unsigned * offset) const;
    const ReflMember  * FindMember(ReflHash name, unsigned * offset) const;
    ReflMember  * FindMember(ReflHash name);

    const ReflMember  * FindLocalMember(ReflHash name) const;
    Parent * FindParent(ReflHash parentHash) const;

    bool SerializeMembers(IStructuredTextStream * stream, const ReflClass * inst, unsigned offset) const;

private:
    ReflHash                m_nameHash;
    const chargr          * m_name;

    unsigned                m_version;
    unsigned                m_size;

    ReflCreateFunc          m_creationFunc;
    ReflFinalizationFunc    m_finalizeFunc;
    ReflVersioningFunc      m_versioningFunc;

    ReflClassDesc         * m_next;

    Parent                * m_parents;

    ReflMember            * m_members;

    ReflAlias             * m_memberAliases;
};

class ReflClass {
public:
    ReflClass();

    virtual bool Serialize(IStructuredTextStream * stream) = 0;
    virtual bool Deserialize(IStructuredTextStream * stream) = 0;

private:
};

template<typename t_Type>
    ReflHash ReflGetTypeHash(const t_Type & reflType);

class ReflLibrary {
public:
    static const ReflClassDesc * GetClassDesc(ReflHash nameHash);

    static void RegisterClassDesc(ReflClassDesc * classDesc);
    static void RegisterClassDescAlias(ReflAlias * classDescAlias);

    static ReflClass * Deserialize(IStructuredTextStream * stream, MemFlags memFlags);
};

#define REFL_DEFINE_USER_TYPE(type)                                         \
    template<typename t_Type>                                               \
        ReflHash ReflGetTypeHash(const t_Type & reflType);                  \
    template<>                                                              \
    inline ReflHash ReflGetTypeHash<type>(const type & reflType) {          \
        return ReflHash(TOWSTR(type));                                      \
    }

#define REFL_DEFINE_CLASS(name)                                             \
    private:                                                                \
        static ReflHash s_className;                                        \
    public:                                                                 \
        static ReflClass * Create(unsigned count, MemFlags memFlags);       \
        bool Serialize(IStructuredTextStream * stream);                     \
        bool Deserialize(IStructuredTextStream * stream);                   \
        static const ReflClassDesc & GetReflectionInfo();                   \
        static ReflClassDesc * CreateReflClassDesc()

#define REFL_IMPL_CLASS_INTERNAL(base, name, nameStr)                       \
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
    ReflHash name::s_className = ReflHash(nameStr)

#define REFL_IMPL_CLASS_BEGIN(base, name)                                   \
    REFL_DEFINE_USER_TYPE(name);                                            \
    REFL_IMPL_CLASS_INTERNAL(base, name, TOWSTR(name));                     \
    ReflClassDesc * name::CreateReflClassDesc() {                           \
        static ReflClassDesc s_reflInfo(                                    \
            TOWSTR(name),                                                   \
            sizeof(name),                                                   \
            name::Create                                                    \
    )
                                
#define REFL_IMPL_CLASS_BEGIN_NAMESPACE(base, ns, name)                     \
    }                                                                       \
    REFL_DEFINE_USER_TYPE(ns::name);                                        \
    namespace ns {                                                          \
    REFL_IMPL_CLASS_INTERNAL(base, name, TOWSTR(ns::name));                 \
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

#define REFL_DO_MANUAL_VERSIONING(func, version)                            \
        s_reflInfo.RegisterManualVersioningFunc(                            \
            func,                                                           \
            version                                                         \
        )
#define REFL_ADD_CLASS_ALIAS(name, alias)                                   \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                ReflHash(TOWSTR(alias)),                                    \
                ReflHash(TOWSTR(name))                                      \
            };                                                              \
            ReflLibrary::RegisterClassDescAlias(&s_alias##alias)

#define REFL_ADD_CLASS_ALIAS_NAMESPACE(name, ns, alias)                     \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                ReflHash(TOWSTR(ns::alias)),                                \
                ReflHash(TOWSTR(name))                                      \
            };                                                              \
            ReflLibrary::RegisterClassDescAlias(&s_alias##alias)

#define REFL_ADD_PARENT(derived, parent)                                    \
            static ReflClassDesc::Parent s_parent##parent = {               \
                NULL,                                                       \
                CLASSOFFSETOF(parent, derived),                             \
                ReflHash(TOWSTR(parent))                                    \
            };                                                              \
            s_reflInfo.AddParent(&s_parent##parent)

#define REFL_ADD_PARENT_NAMESPACE(derived, ns, parent)                      \
            static ReflClassDesc::Parent s_parent##parent = {               \
                NULL,                                                       \
                CLASSOFFSETOF(parent, derived),                             \
                ReflHash(TOWSTR(ns::parent))                                \
            };                                                              \
            s_reflInfo.AddParent(&s_parent##parent)

#define REFL_FINALIZATION_FUNC(func)                                        \
            s_reflInfo.RegisterFinalizationFunc(func)

#define REFL_MEMBER_INTERNAL(parent, name, typeHash)                        \
            static ReflMember s_member##name(                               \
                &s_reflInfo,                                                \
                typeHash,                                                   \
                TOWSTR(name),                                               \
                SIZEOF(parent, name),                                       \
                OFFSETOF(parent, name)                                      \
            )

#define REFL_MEMBER(parent, name)                                           \
            REFL_MEMBER_INTERNAL(                                           \
                parent,                                                     \
                name,                                                       \
                ReflGetTypeHash((((parent *)(0x0))->name))                  \
            )                           

#define REFL_MEMBER_DEPRECATED(parent, name, type)                          \
            static ReflMember s_member##name(                               \
                &s_reflInfo,                                                \
                ReflGetTypeHash(*((type *) (0x0))),                         \
                TOWSTR(name),                                               \
                sizeof(type),                                               \
                true                                                        \
            )

#define REFL_MEMBER_ENUM(parent, name)                                      \
            REFL_MEMBER_INTERNAL(                                           \
                parent,                                                     \
                name,                                                       \
                ReflHash(TOWSTR(enum))                                      \
            )                           

#define REFL_ADD_MEMBER_CONVERSION(name, conv)                              \
            s_member##name.RegisterConversionFunc(conv)

#define REFL_ADD_MEMBER_ALIAS(name, alias)                                  \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                ReflHash(TOWSTR(alias)),                                    \
                ReflHash(TOWSTR(name))                                      \
            };                                                              \
            s_reflInfo.RegisterMemberAlias(&s_alias##alias)

#define REFL_ADD_MEMBER_ALIAS_W_CONVERSION(name, alias, conv)               \
            REFL_ADD_MEMBER_ALIAS(name, alias);                             \
            REFL_ADD_MEMBER_CONVERSION(name, conv)

#define REFL_ENUM_VALUE(enumName, value)                                    \
            static ReflMember::EnumValue s_enumValue##value = {             \
                NULL,                                                       \
                value,                                                      \
                TOWSTR(value),                                              \
                ReflHash(TOWSTR(value))                                     \
            };                                                              \
            s_member##enumName.RegisterEnumValue(&s_enumValue##value)

#define REFL_ENUM_ALIAS(enumName, value, oldValue)                          \
            static ReflMember::EnumValue s_enumValue##oldValue = {          \
                NULL,                                                       \
                value,                                                      \
                TOWSTR(oldValue),                                           \
                ReflHash(TOWSTR(oldValue))                                  \
            };                                                              \
            s_member##enumName.RegisterEnumValue(&s_enumValue##oldValue)

REFL_DEFINE_USER_TYPE(bool);
REFL_DEFINE_USER_TYPE(int8);
REFL_DEFINE_USER_TYPE(uint8);
REFL_DEFINE_USER_TYPE(int16);
REFL_DEFINE_USER_TYPE(uint16);
REFL_DEFINE_USER_TYPE(int32);
REFL_DEFINE_USER_TYPE(uint32);
REFL_DEFINE_USER_TYPE(int64);
REFL_DEFINE_USER_TYPE(uint64);
//REFL_DEFINE_USER_TYPE(int128);
//REFL_DEFINE_USER_TYPE(uint128);
//REFL_DEFINE_USER_TYPE(float16);
REFL_DEFINE_USER_TYPE(float32);
//REFL_DEFINE_USER_TYPE();//REFL_INDEX_STRING,
//REFL_DEFINE_USER_TYPE();//REFL_INDEX_ENUM,
//REFL_DEFINE_USER_TYPE();//REFL_INDEX_FIXED_ARRAY,
//REFL_DEFINE_USER_TYPE();//REFL_INDEX_VAR_ARRAY,
//REFL_DEFINE_USER_TYPE();//REFL_INDEX_POINTER,
//REFL_DEFINE_USER_TYPE(color);
//REFL_DEFINE_USER_TYPE(angle);
//REFL_DEFINE_USER_TYPE(percentage);
//REFL_DEFINE_USER_TYPE(eulers);
//REFL_DEFINE_USER_TYPE(vec3);
//REFL_DEFINE_USER_TYPE(vec4);
//REFL_DEFINE_USER_TYPE(quaternion);


