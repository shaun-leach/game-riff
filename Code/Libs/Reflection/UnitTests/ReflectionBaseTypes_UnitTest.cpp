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
// Test simple types
//

enum ETestEnum {
    TEST_ENUM_VALUE1,
    TEST_ENUM_VALUE2,
    TEST_ENUM_VALUE3
};

REFL_ENUM_IMPL_BEGIN(ETestEnum);
    REFL_ENUM_VALUE(TEST_ENUM_VALUE1, Large);
    REFL_ENUM_VALUE(TEST_ENUM_VALUE2, Medium);
    REFL_ENUM_VALUE(TEST_ENUM_VALUE3, Small);
REFL_ENUM_IMPL_END(ETestEnum);

class TestBaseTypes : public ReflClass {
public:
    REFL_DEFINE_CLASS(TestBaseTypes);
    TestBaseTypes() :
        boolTest(false),
        int8Test(0),  
        uint8Test(0), 
        int16Test(0), 
        uint16Test(0),
        int32Test(0), 
        uint32Test(0),
        int64Test(0), 
        uint64Test(0),
        float32Test(0),
        enumTest(TEST_ENUM_VALUE1),
        angleTest(0.0f),
        percentTest(0.0f)
    {
        InitReflType();
    }
    
//private: Make these public to simplify the test code        
    // Boolean types
    bool    boolTest;

    // Integer types
    int8    int8Test;
    uint8   uint8Test;
    int16   int16Test;
    uint16  uint16Test;
    int32   int32Test;
    uint32  uint32Test;
    int64   int64Test;
    uint64  uint64Test;

    // Float types
    //float16   float16Test;
    float32     float32Test;

    ETestEnum   enumTest;

    angle       angleTest;
    percentage  percentTest;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, TestBaseTypes);
    REFL_MEMBER(boolTest);
    REFL_MEMBER(int8Test);
    REFL_MEMBER(uint8Test);
    REFL_MEMBER(int16Test);
    REFL_MEMBER(uint16Test);
    REFL_MEMBER(int32Test);
    REFL_MEMBER(uint32Test);
    REFL_MEMBER(int64Test);
    REFL_MEMBER(uint64Test);
    REFL_MEMBER(float32Test);
    REFL_MEMBER_ENUM(enumTest);
    //REFL_MEMBER(TestBaseTypes, angleTest);
    //REFL_MEMBER(TestBaseTypes, percentTest);
REFL_IMPL_CLASS_END(TestBaseTypes);

static const ETestEnum   s_enumValue    =  TEST_ENUM_VALUE2;

//====================================================
TEST(ReflectionTest, TestBaseTypes) {
    TestBaseTypes testTypes;
    testTypes.boolTest      = s_boolValue;
    testTypes.int8Test      = s_int8Value;
    testTypes.uint8Test     = s_uint8Value;
    testTypes.int16Test     = s_int16Value;
    testTypes.uint16Test    = s_uint16Value;
    testTypes.int32Test     = s_int32Value;
    testTypes.uint32Test    = s_uint32Value;
    testTypes.int64Test     = s_int64Value;
    testTypes.uint64Test    = s_uint64Value;
    testTypes.float32Test   = s_float32Value;
    testTypes.enumTest      = s_enumValue;
    //testTypes.angleTest     = s_angleValue;
    //testTypes.percentTest   = s_percentValue;

    IStructuredTextStream * testStream = StreamCreateXML(L"testBaseTypes.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testTypes);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testBaseTypes.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    TestBaseTypes * loadTypes = ReflCast<TestBaseTypes>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    EXPECT_EQ(s_boolValue,      loadTypes->boolTest);
    EXPECT_EQ(s_int8Value,      loadTypes->int8Test);
    EXPECT_EQ(s_uint8Value,     loadTypes->uint8Test);
    EXPECT_EQ(s_int16Value,     loadTypes->int16Test);
    EXPECT_EQ(s_uint16Value,    loadTypes->uint16Test);
    EXPECT_EQ(s_int32Value,     loadTypes->int32Test);
    EXPECT_EQ(s_uint32Value,    loadTypes->uint32Test);
    EXPECT_EQ(s_int64Value,     loadTypes->int64Test);
    EXPECT_EQ(s_uint64Value,    loadTypes->uint64Test);
    EXPECT_EQ(s_float32Value,   loadTypes->float32Test);
    EXPECT_EQ(s_enumValue,      loadTypes->enumTest);
    //EXPECT_EQ(s_angleValue,     loadTypes->angleTest);
    //EXPECT_EQ(s_percentValue,   loadTypes->percentTest);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test member types
//

class MemberClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(MemberClass);
    MemberClass() :
        memberUint32Test(0),
        memberFloat32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      memberUint32Test;
    float32     memberFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, MemberClass);
    REFL_MEMBER(memberUint32Test);
    REFL_MEMBER(memberFloat32Test);
REFL_IMPL_CLASS_END(MemberClass);

class TestMemberClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(TestMemberClass);
    TestMemberClass() :
        containerUint16Test(0),
        containerBoolTest(false)
    {
        InitReflType();
    }

// private:
    uint16      containerUint16Test;
    bool        containerBoolTest;
    MemberClass classMemberTest;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, TestMemberClass);
    REFL_MEMBER(containerUint16Test);
    REFL_MEMBER(containerBoolTest);
    REFL_MEMBER(classMemberTest);
REFL_IMPL_CLASS_END(TestMemberClass);

//====================================================
TEST(ReflectionTest, TestMemberClass) {
    TestMemberClass testClassMember;
    testClassMember.containerUint16Test                 = s_uint16Value;
    testClassMember.containerBoolTest                   = s_boolValue;
    testClassMember.classMemberTest.memberUint32Test    = s_uint32Value;
    testClassMember.classMemberTest.memberFloat32Test   = s_float32Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testMemberClass.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testClassMember);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testMemberClass.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    TestMemberClass * loadTypes = ReflCast<TestMemberClass>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    EXPECT_EQ(s_uint16Value,    loadTypes->containerUint16Test);
    EXPECT_EQ(s_boolValue,      loadTypes->containerBoolTest);
    EXPECT_EQ(s_uint32Value,    loadTypes->classMemberTest.memberUint32Test);
    EXPECT_EQ(s_float32Value,   loadTypes->classMemberTest.memberFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}


