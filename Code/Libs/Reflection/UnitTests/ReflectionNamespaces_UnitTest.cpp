/*
   GameRiff - Framework for creating various video game services
   Reflection unit tests
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

#include <gtest/gtest.h>

#include "Pch.h"

//////////////////////////////////////////////////////
//
// Internal constants
//
static const bool      s_boolValue       =  true;
static const int8      s_int8Value       = -8;
static const uint8     s_uint8Value      =  8;
static const int16     s_int16Value      = -1600;
static const uint16    s_uint16Value     =  1600;
static const int32     s_int32Value      = -320000;
static const uint32    s_uint32Value     =  320000;
static const int64     s_int64Value      = -640000000LL;
static const uint64    s_uint64Value     =  640000000ULL;
static const float32   s_float32Value    =  32.32f;
static const float     s_angleValue      =  MathDegreesToRadians(30.0f);
static const float     s_percentValue    = 0.20f;

//////////////////////////////////////////////////////
//
// Test basic name space support
//

namespace TestSpace {

class SimpleNamespaceClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleNamespaceClass);
    SimpleNamespaceClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN_NAMESPACE(ReflClass, TestSpace, SimpleNamespaceClass);
    REFL_MEMBER(SimpleNamespaceClass, baseUint32Test, uint32);
    REFL_MEMBER(SimpleNamespaceClass, baseFloat32Test, float32);
REFL_IMPL_CLASS_END(SimpleNamespaceClass);

} // namespace TestSpace

//====================================================
TEST(ReflectionTest, TestSimpleNamespaces) {
    TestSpace::SimpleNamespaceClass testNamespaces;
    testNamespaces.baseUint32Test    = s_uint32Value;
    testNamespaces.baseFloat32Test   = s_float32Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testBasicNamespaces.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = testNamespaces.Serialize(testStream);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testBasicNamespaces.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    TestSpace::SimpleNamespaceClass * loadTypes = dynamic_cast<TestSpace::SimpleNamespaceClass *>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test parent name space support
//

namespace TestSpaceParent {

class SimpleNamespaceParentClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleNamespaceParentClass);
    SimpleNamespaceParentClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN_NAMESPACE(ReflClass, TestSpaceParent, SimpleNamespaceParentClass);
    REFL_MEMBER(SimpleNamespaceParentClass, baseUint32Test, uint32);
    REFL_MEMBER(SimpleNamespaceParentClass, baseFloat32Test, float32);
REFL_IMPL_CLASS_END(SimpleNamespaceParentClass);

} // namespace TestSpaceParent

class SimpleParentNamespaceClass : public TestSpaceParent::SimpleNamespaceParentClass {
public:
    REFL_DEFINE_CLASS(SimpleParentNamespaceClass);
    SimpleParentNamespaceClass() :
        int16Test(0),
        int8Test(0)
    {
    }

//private:
    int16   int16Test;
    int8    int8Test;
};

REFL_IMPL_CLASS_BEGIN(TestSpaceParent::SimpleNamespaceParentClass, SimpleParentNamespaceClass);
    REFL_ADD_PARENT_NAMESPACE(SimpleParentNamespaceClass, TestSpaceParent, SimpleNamespaceParentClass);
    REFL_MEMBER(SimpleParentNamespaceClass, int16Test, int16);
    REFL_MEMBER(SimpleParentNamespaceClass, int8Test, int8);
REFL_IMPL_CLASS_END(SimpleParentNamespaceClass);

//====================================================
TEST(ReflectionTest, TestParentNamespaces) {
    SimpleParentNamespaceClass testNamespaces;
    testNamespaces.baseUint32Test   = s_uint32Value;
    testNamespaces.baseFloat32Test  = s_float32Value;
    testNamespaces.int16Test        = s_int16Value;
    testNamespaces.int8Test         = s_int8Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testParentNamespaces.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = testNamespaces.Serialize(testStream);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testParentNamespaces.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleParentNamespaceClass * loadTypes = dynamic_cast<SimpleParentNamespaceClass *>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(s_int16Value,       loadTypes->int16Test);
    EXPECT_EQ(s_int8Value,        loadTypes->int8Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test aliased name space support
//

class SimpleAliasedNamespaceClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleAliasedNamespaceClass);
    SimpleAliasedNamespaceClass() :
        baseUint8Test0(0),
        baseUint8Test1(0),
        baseUint8Test2(0),
        baseUint16Test0(0)
    {
    }

//private:
    uint8      baseUint8Test0;
    uint8      baseUint8Test1;
    uint8      baseUint8Test2;
    uint16     baseUint16Test0;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleAliasedNamespaceClass);
    REFL_ADD_CLASS_ALIAS_NAMESPACE(SimpleAliasedNamespaceClass, OldNamespace, SimpleAliasedNamespaceClass);
    REFL_MEMBER(SimpleAliasedNamespaceClass, baseUint8Test0, uint8);
    REFL_MEMBER(SimpleAliasedNamespaceClass, baseUint8Test1, uint8);
    REFL_MEMBER(SimpleAliasedNamespaceClass, baseUint8Test2, uint8);
    REFL_MEMBER(SimpleAliasedNamespaceClass, baseUint16Test0, uint16);
REFL_IMPL_CLASS_END(SimpleAliasedNamespaceClass);

//====================================================
TEST(ReflectionTest, TestAliasedNamespaces) {

    IStructuredTextStream * testStream = StreamOpenXML(L"testAliasedNamespaces.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleAliasedNamespaceClass * loadTypes = dynamic_cast<SimpleAliasedNamespaceClass *>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(s_uint8Value,      loadTypes->baseUint8Test0);
    EXPECT_EQ(s_uint8Value,      loadTypes->baseUint8Test1);
    EXPECT_EQ(s_uint8Value,      loadTypes->baseUint8Test2);
    EXPECT_EQ(s_uint16Value,     loadTypes->baseUint16Test0);

    delete loadTypes;
    loadTypes = NULL;
}

