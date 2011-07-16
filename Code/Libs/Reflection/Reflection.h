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
class ReflTypeDesc;
class DataStream;
class IStructuredTextStream;
DECLARE_SMARTPTR(IStructuredTextStream);

typedef unsigned    ReflIndex;
typedef Hash32      ReflHash;

typedef void * (*ReflCreateFunc)(unsigned count, MemFlags memFlags);
typedef void (*ReflFinalizationFunc)(ReflClass * inst);
typedef void (*ReflConversionFunc)(ReflClass * inst, ReflHash name, ReflHash oldType, void * data);
typedef void (*ReflVersioningFunc)(IStructuredTextStreamPtr stream, ReflTypeDesc * desc, unsigned version, ReflClass * inst);

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
        ReflTypeDesc * container,
        ReflHash        typeHash,
        const chargr  * name, 
        unsigned        size,
        unsigned        offset
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

    void AdjustOffset(unsigned offset);
    unsigned GetOffset() const {
        return m_offset;
    }

    void Finalize();

    bool ConvertToString(const byte * data, chargr * str, unsigned len) const;
    bool ConvertFromString(const byte * data, chargr * str, unsigned len) const;

    bool Serialize(IStructuredTextStreamPtr stream, const ReflClass * inst, const void * base, unsigned offset) const;
    void Deserialize(IStructuredTextStreamPtr stream, ReflHash nameHash, ReflClass * inst, void * base, unsigned offset) const;

    bool Serialize(DataStream * stream, const ReflClass * inst, unsigned offset) const;
    void Deserialize(DataStream * stream, ReflHash nameHash, ReflClass * inst, void * base, unsigned offset) const;

    void RegisterConversionFunc(ReflConversionFunc func);

    void RegisterTempBinding(void * data) {
        m_tempBinding = data;
    }
    void ClearTempBinding() {
        m_tempBinding = NULL;
    }

    void MarkDeprecated() {
        m_deprecated = true;
    }
    bool IsDeprecated() const {
        return m_deprecated;
    }

    unsigned GetSize() const {
        return m_size;
    }
private:

    bool DeserializeClassMember(IStructuredTextStreamPtr stream, void * inst, unsigned offset) const;
    bool ConvertDataMember(
        IStructuredTextStreamPtr    stream, 
        ReflHash                    nameHash,
        ReflClass                 * inst, 
        ReflIndex                   oldType
    ) const;
    bool ConvertClassMember(
        IStructuredTextStreamPtr    stream, 
        ReflHash                    nameHash, 
        ReflClass                 * inst, 
        ReflIndex                   oldType
    ) const;

    ReflIndex DetermineTypeIndex(ReflHash typeHash) const;

    ReflIndex TypeIndex() const {
        return m_index;
    }

private:
    ReflHash            m_nameHash;
    ReflHash            m_typeHash;

    const chargr      * m_name;
    ReflIndex           m_index;

    unsigned            m_offset;
    unsigned            m_size;

    ReflMember        * m_next;

    ReflConversionFunc  m_convFunc;

    bool                m_deprecated; // Need bit flags class
    void              * m_tempBinding;
};

class ReflTypeDesc {
public:
    ReflTypeDesc(
        const chargr  * name, 
        unsigned        size,
        unsigned        baseOffset,
        unsigned        reflOffset,
        ReflCreateFunc  creationFunc
    );

    void Finalize();

    ReflHash GetHash() const {
        return m_typeHash;
    }

    const chargr * GetTypeName() const {
        return m_typeName;
    }

    unsigned GetVersion() const {
        return m_version;
    }

    unsigned            NumMembers() const;
    const ReflMember  & GetMember(unsigned index) const;
    const ReflMember  * FindMember(ReflHash name) const;

    void RegisterMember(ReflMember * member);

    void RegisterFinalizationFunc(ReflFinalizationFunc finalFunc);
    void RegisterManualVersioningFunc(ReflVersioningFunc loadFunc, unsigned currentVersion);

    bool NameMatches(const ReflHash rhs) const {
        return m_typeHash == rhs;
    }

    void * Create(unsigned count, MemFlags memFlags) const {
        return m_creationFunc(count, memFlags);
    }

    void InitInst(void * inst) const;

    bool Serialize(IStructuredTextStreamPtr stream, const ReflClass * inst, unsigned offset = 0) const;
    bool Deserialize(IStructuredTextStreamPtr stream, ReflClass * inst) const;
    bool Deserialize(IStructuredTextStreamPtr stream, void * inst, unsigned offset) const;
    bool DeserializeMembers(IStructuredTextStreamPtr stream, void * inst) const;
    bool DeserializeMembers(IStructuredTextStreamPtr stream, void * inst, unsigned offset) const;

    bool RegisterTempBinding(ReflHash memberHash, ReflHash typeHash, void * data);
    void ClearTempBinding(ReflHash memberHash, ReflHash typeHash);
    void ClearAllTempBindings();

    struct Parent {
        Parent    * next;
        unsigned    baseOffset;
        unsigned    reflOffset;
        ReflHash    parentHash;
    };
    void AddParent(Parent * parent);

    ReflTypeDesc * GetNext() {
        return m_next;
    }
    const ReflTypeDesc * GetNext() const {
        return m_next;
    }
    void SetNext(ReflTypeDesc * next);

    void RegisterMemberAlias(ReflAlias * alias);

    struct EnumValue {
        EnumValue     * next;
        int64           value;
        const chargr  * name;
        ReflHash        nameHash;
    };

    void RegisterEnumValue(EnumValue * value);
    const EnumValue * GetEnumValue(int64 value) const;
    const EnumValue * GetEnumValue(const chargr * str, unsigned len) const;
    bool IsEnumType() const;

    void * CastTo(ReflClass * inst, ReflHash givenType, ReflHash targetType) const;
    const void * CastTo(const ReflClass * inst, ReflHash givenType, ReflHash targetType) const;
    void * CastTo(void * inst, ReflHash givenType, ReflHash targetType) const;
    const void * CastTo(const void * inst, ReflHash givenType, ReflHash targetType) const;
private:

    bool CalculateCastOffset(ReflHash givenType, ReflHash targetType, int * offset) const;

    void * CastToBase(ReflClass * inst) const;
    const void * CastToBase(const ReflClass * inst) const;
    ReflClass * CastToReflClass(void * inst) const;

    void FinalizeInst(void * inst) const;

    const ReflMember  * FindMember(const chargr * name, unsigned * offset) const;
    const ReflMember  * FindMember(ReflHash name, unsigned * offset) const;
    ReflMember        * FindMember(ReflHash name);
    const ReflMember  * FindLocalMember(ReflHash name) const;

    Parent * FindParent(ReflHash parentHash) const;
    Parent * FindParentRecursive(ReflHash parentHash) const;
    bool FindParentOffset(ReflHash parentHash, unsigned * offset, unsigned * reflOffset) const;

    bool SerializeMembers(
        IStructuredTextStreamPtr    stream, 
        const ReflClass           * inst,
        const void                * base, 
        unsigned                    offset
    ) const;

private:
    ReflHash                m_typeHash;
    const chargr          * m_typeName;

    unsigned                m_version;
    unsigned                m_size;
    unsigned                m_baseOffset;
    unsigned                m_reflOffset;

    ReflCreateFunc          m_creationFunc;
    ReflFinalizationFunc    m_finalizeFunc;
    ReflVersioningFunc      m_versioningFunc;

    ReflTypeDesc          * m_next;

    Parent                * m_parents;

    ReflMember            * m_members;

    ReflAlias             * m_memberAliases;

    EnumValue             * m_enumValues;
};

class ReflClass {
public:
    ReflClass();

    ReflHash GetType() const {
        return m_type;
    }

    void SetTypeHash(ReflHash typeHash) {
        m_type = typeHash;
    }

    static ReflHash GetReflType() {
        return s_classType;
    }
protected:

private:
    static ReflHash s_classType;

    ReflHash m_type;
};

template<typename t_Type>
    ReflHash ReflGetTypeHash(const t_Type & reflType);

class ReflLibrary {
public:
    static const ReflTypeDesc * GetClassDesc(ReflHash nameHash);
    static const ReflTypeDesc * GetClassDesc(const ReflClass * inst);

    static void RegisterClassDesc(ReflTypeDesc * classDesc);
    static void RegisterDeprecatedClassDesc(ReflAlias * classDescAlias);

    static ReflClass * Deserialize(IStructuredTextStreamPtr stream, MemFlags memFlags);
    static bool Serialize(IStructuredTextStreamPtr stream, const ReflClass * inst);
    static bool Deserialize(IStructuredTextStreamPtr stream, ReflClass * inst);
};

void ReflInitialize();

void ReflInitType(void * inst, ReflHash type);

void * ReflCanCastTo(ReflClass * inst, ReflHash actualType, ReflHash givenType, ReflHash targetType);
const void * ReflCanCastTo(const ReflClass * inst, ReflHash actualType, ReflHash givenType, ReflHash targetType);

template<typename t_cast, typename t_given>
t_cast * ReflCast(t_given * inst) {
    if (inst == NULL) 
        return NULL;

    ReflHash givenType  = t_given::GetReflType();
    ReflHash targetType = t_cast::GetReflType();

    t_cast * ret = reinterpret_cast<t_cast *>(ReflCanCastTo(inst, inst->GetType(), givenType, targetType));
    return ret;
}

template<typename t_cast, typename t_given>
const t_cast * ReflCast(const t_given * inst) {
    if (inst == NULL) 
        return NULL;

    ReflHash givenType  = t_given::GetReflType();
    ReflHash targetType = t_cast::GetReflType();

    const t_cast * ret = reinterpret_cast<const t_cast *>(ReflCanCastTo(inst, inst->GetType(), givenType, targetType));
    return ret;
}

#define REFL_DEFINE_USER_TYPE(type)                                         \
    template<typename t_Type>                                               \
        ReflHash ReflGetTypeHash(const t_Type & reflType);                  \
    template<>                                                              \
    static inline ReflHash ReflGetTypeHash<type>(const type & reflType) {   \
        return ReflHash(TOWSTR(type));                                      \
    }

#define REFL_DEFINE_CLASS(name)                                             \
    private:                                                                \
        static ReflHash s_className;                                        \
    public:                                                                 \
        static void * Create(unsigned count, MemFlags memFlags);            \
        static const ReflTypeDesc * GetReflectionInfo();                    \
        template<typename t_reflType>                                       \
        static ReflTypeDesc * ReflCreateClassDesc();                        \
        inline static ReflHash GetReflType() {                              \
            return s_className;                                             \
        }                                                                   \
        void InitReflType()

#define REFL_IMPL_CLASS_INTERNAL(base, name, nameStr)                       \
    void * name::Create(unsigned count, MemFlags memFlags) {                \
        name * inst = new(memFlags) name;                                   \
        return inst;                                                        \
    }                                                                       \
    const ReflTypeDesc * name::GetReflectionInfo() {                        \
        return ReflLibrary::GetClassDesc(s_className);                      \
    }                                                                       \
    void name::InitReflType() {                                             \
        ReflInitType(this, s_className);                                    \
    }                                                                       \
    ReflHash name::s_className = ReflHash(nameStr)

#define REFL_IMPL_CLASS_BEGIN(base, name)                                   \
    REFL_DEFINE_USER_TYPE(name);                                            \
    REFL_IMPL_CLASS_INTERNAL(base, name, TOWSTR(name));                     \
    template<typename t_reflType>                                           \
    ReflTypeDesc * name::ReflCreateClassDesc() {                            \
        static ReflTypeDesc s_reflInfo(                                     \
            TOWSTR(name),                                                   \
            sizeof(t_reflType),                                             \
            CLASSOFFSETOF(base, t_reflType),                                \
            CLASSOFFSETOF(ReflClass, base),                                 \
            t_reflType::Create                                              \
        )
                                
#define REFL_IMPL_CLASS_BEGIN_NAMESPACE(base, ns, name)                     \
    }                                                                       \
    REFL_DEFINE_USER_TYPE(ns::name);                                        \
    namespace ns {                                                          \
    REFL_IMPL_CLASS_INTERNAL(base, name, TOWSTR(ns::name));                 \
    template<typename t_reflType>                                           \
    ReflTypeDesc * name::ReflCreateClassDesc() {                            \
        static ReflTypeDesc s_reflInfo(                                     \
            TOWSTR(ns::name),                                               \
            sizeof(t_reflType),                                             \
            CLASSOFFSETOF(base, t_reflType),                                \
            CLASSOFFSETOF(ReflClass, base),                                 \
            t_reflType::Create                                              \
        )
                                
#define REFL_IMPL_CLASS_END(name)                                           \
        return &s_reflInfo;                                                 \
    }                                                                       \
    class ReflAutoRegister##name {                                          \
    public:                                                                 \
        ReflAutoRegister##name() {                                          \
            ReflTypeDesc * desc = name::ReflCreateClassDesc<name>();        \
            ReflLibrary::RegisterClassDesc(desc);                           \
        }                                                                   \
    };                                                                      \
    static ReflAutoRegister##name s_autoRegister##name

#define REFL_DO_MANUAL_VERSIONING(func, version)                            \
        s_reflInfo.RegisterManualVersioningFunc(                            \
            func,                                                           \
            version                                                         \
        )
#define REFL_ADD_DEPRECATED_CLASS(name, alias)                              \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                ReflHash(TOWSTR(alias)),                                    \
                ReflHash(TOWSTR(name))                                      \
            };                                                              \
            ReflLibrary::RegisterDeprecatedClassDesc(&s_alias##alias)

#define REFL_ADD_DEPRECATED_CLASS_NAMESPACE(name, ns, alias)                \
            static ReflAlias s_alias##alias = {                             \
                NULL,                                                       \
                ReflHash(TOWSTR(ns::alias)),                                \
                ReflHash(TOWSTR(name))                                      \
            };                                                              \
            ReflLibrary::RegisterDeprecatedClassDesc(&s_alias##alias)

#define REFL_ADD_PARENT(parent)                                             \
            static ReflTypeDesc::Parent s_parent##parent = {                \
                NULL,                                                       \
                CLASSOFFSETOF(parent, t_reflType),                          \
                CLASSOFFSETOF(ReflClass, parent),                           \
                ::ReflGetTypeHash(*((parent *) 0x0))                        \
            };                                                              \
            s_reflInfo.AddParent(&s_parent##parent)

#define REFL_ADD_PARENT_NAMESPACE(ns, parent)                               \
            static ReflTypeDesc::Parent s_parent##parent = {                \
                NULL,                                                       \
                CLASSOFFSETOF(parent, t_reflType),                          \
                CLASSOFFSETOF(ReflClass, parent),                           \
                ReflHash(TOWSTR(ns::parent))                                \
            };                                                              \
            s_reflInfo.AddParent(&s_parent##parent)

#define REFL_FINALIZATION_FUNC(func)                                        \
            s_reflInfo.RegisterFinalizationFunc(func)

#define REFL_MEMBER_INTERNAL(name, typeHash)                                \
            static ReflMember s_member##name(                               \
                &s_reflInfo,                                                \
                typeHash,                                                   \
                TOWSTR(name),                                               \
                SIZEOF(t_reflType, name),                                   \
                OFFSETOF(t_reflType, name)                                  \
            )

#define REFL_MEMBER(name)                                                   \
            REFL_MEMBER_INTERNAL(                                           \
                name,                                                       \
                ::ReflGetTypeHash((((t_reflType *)(0x0))->name))            \
            )                           

#define REFL_MEMBER_DEPRECATED(name, type)                                  \
            static ReflMember s_member##name(                               \
                &s_reflInfo,                                                \
                ::ReflGetTypeHash(*((type *) (0x0))),                       \
                TOWSTR(name),                                               \
                sizeof(type),                                               \
                0                                                           \
            );                                                              \
            s_member##name.MarkDeprecated()

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

#define REFL_ENUM_CLASS_IMPL_BEGIN(scope, type)                             \
    REFL_DEFINE_USER_TYPE(scope::type);                                     \
    template<typename t_reflType>                                           \
    ReflTypeDesc * ReflCreateEnumDesc##type() {                             \
        static ReflTypeDesc s_typeDesc(                                     \
            TOWSTR(scope::type),                                            \
            sizeof(t_reflType),                                             \
            0,                                                              \
            0,                                                              \
            NULL                                                            \
        )

#define REFL_ENUM_CLASS_VALUE(scope, value, displayName)                    \
        static ReflTypeDesc::EnumValue s_enumValue##value = {               \
            NULL,                                                           \
            scope::value,                                                   \
            TOWSTR(displayName),                                            \
            ReflHash(TOWSTR(scope::value))                                  \
        };                                                                  \
        s_typeDesc.RegisterEnumValue(&s_enumValue##value)

#define REFL_ENUM_CLASS_IMPL_END(scope, name)                               \
        return &s_typeDesc;                                                 \
    }                                                                       \
    class ReflAutoRegister##name {                                          \
    public:                                                                 \
        ReflAutoRegister##name() {                                          \
            ReflTypeDesc * desc = ReflCreateEnumDesc##name<scope::name>();  \
            ReflLibrary::RegisterClassDesc(desc);                           \
        }                                                                   \
    };                                                                      \
    static ReflAutoRegister##name s_autoRegister##name

#define REFL_ENUM_IMPL_BEGIN(name)                                          \
    REFL_DEFINE_USER_TYPE(name);                                            \
    template<typename t_reflType>                                           \
    ReflTypeDesc * ReflCreateEnumDesc##name() {                             \
        static ReflTypeDesc s_typeDesc(                                     \
            TOWSTR(name),                                                   \
            sizeof(t_reflType),                                             \
            0,                                                              \
            0,                                                              \
            NULL                                                            \
        )

#define REFL_ENUM_VALUE(value, displayName)                                 \
        static ReflTypeDesc::EnumValue s_enumValue##value = {               \
            NULL,                                                           \
            value,                                                          \
            TOWSTR(displayName),                                            \
            ReflHash(TOWSTR(value))                                         \
        };                                                                  \
        s_typeDesc.RegisterEnumValue(&s_enumValue##value)

#define REFL_ENUM_ALIAS(value, oldValue, oldDisplay)                        \
        static ReflTypeDesc::EnumValue s_enumValue##oldValue = {            \
            NULL,                                                           \
            value,                                                          \
            TOWSTR(oldDisplay),                                             \
            ReflHash(TOWSTR(oldValue))                                      \
        };                                                                  \
        s_typeDesc.RegisterEnumValue(&s_enumValue##oldValue)

#define REFL_ENUM_IMPL_END(name)                                            \
        return &s_typeDesc;                                                 \
    }                                                                       \
    class ReflAutoRegister##name {                                          \
    public:                                                                 \
        ReflAutoRegister##name() {                                          \
            ReflTypeDesc * desc = ReflCreateEnumDesc##name<name>();         \
            ReflLibrary::RegisterClassDesc(desc);                           \
        }                                                                   \
    };                                                                      \
    static ReflAutoRegister##name s_autoRegister##name

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


