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
// Test simple inheritance
//

class SimpleBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleBaseClass);
    SimpleBaseClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleBaseClass);
    REFL_MEMBER(SimpleBaseClass, baseUint32Test);
    REFL_MEMBER(SimpleBaseClass, baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleBaseClass);

class SimpleDerivedClass : public SimpleBaseClass {
public:
    REFL_DEFINE_CLASS(SimpleDerivedClass);
    SimpleDerivedClass() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleBaseClass, SimpleDerivedClass);
    REFL_ADD_PARENT(SimpleDerivedClass, SimpleBaseClass);
    REFL_MEMBER(SimpleDerivedClass, derivedBoolTest);
    REFL_MEMBER(SimpleDerivedClass, derivedInt16Test);
    REFL_MEMBER(SimpleDerivedClass, derivedInt16Test2);
REFL_IMPL_CLASS_END(SimpleDerivedClass);

//====================================================
TEST(ReflectionTest, TestSimpleInheritance) {
    SimpleDerivedClass testInheritance;
    testInheritance.baseUint32Test      = s_uint32Value;
    testInheritance.baseFloat32Test     = s_float32Value;
    testInheritance.derivedBoolTest     = s_boolValue;
    testInheritance.derivedInt16Test    = s_int16Value;
    testInheritance.derivedInt16Test2   = s_int16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testSimpleInheritance.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = testInheritance.Serialize(testStream);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testSimpleInheritance.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleDerivedClass * loadTypes = dynamic_cast<SimpleDerivedClass *>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test2);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test class hierarchy
//

class MoreDerivedClass : public SimpleDerivedClass {
public:
    REFL_DEFINE_CLASS(MoreDerivedClass);
    MoreDerivedClass() :
        moreDerivedBoolTest(false),
        moreDerivedFloat32Test(0.0f),
        moreDerivedBoolTest2(false)
    {
    }

//private:
    bool        moreDerivedBoolTest;
    float32     moreDerivedFloat32Test;
    bool        moreDerivedBoolTest2;
};

REFL_IMPL_CLASS_BEGIN(SimpleDerivedClass, MoreDerivedClass);
    REFL_ADD_PARENT(MoreDerivedClass, SimpleDerivedClass);
    REFL_MEMBER(MoreDerivedClass, moreDerivedBoolTest);
    REFL_MEMBER(MoreDerivedClass, moreDerivedFloat32Test);
    REFL_MEMBER(MoreDerivedClass, moreDerivedBoolTest2);
REFL_IMPL_CLASS_END(MoreDerivedClass);

//====================================================
TEST(ReflectionTest, TestClassHierarchy) {
    MoreDerivedClass testInheritance;
    testInheritance.baseUint32Test          = s_uint32Value;
    testInheritance.baseFloat32Test         = s_float32Value;
    testInheritance.derivedBoolTest         = s_boolValue;
    testInheritance.derivedInt16Test        = s_int16Value;
    testInheritance.derivedInt16Test2       = s_int16Value;
    testInheritance.moreDerivedBoolTest     = s_boolValue;
    testInheritance.moreDerivedFloat32Test  = s_float32Value;
    testInheritance.moreDerivedBoolTest2    = s_boolValue;

    IStructuredTextStream * testStream = StreamCreateXML(L"testClassHierarchy.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = testInheritance.Serialize(testStream);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testClassHierarchy.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    MoreDerivedClass * loadTypes = dynamic_cast<MoreDerivedClass *>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test2);
    EXPECT_EQ(s_boolValue,        loadTypes->moreDerivedBoolTest);
    EXPECT_EQ(s_float32Value,     loadTypes->moreDerivedFloat32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->moreDerivedBoolTest2);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test multiple inheritance
//

class SimpleBaseClass2 : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleBaseClass2);
    SimpleBaseClass2() :
        base2Uint32Test(0),
        base2Float32Test(0.0f)
    {
    }

//private:
    uint32      base2Uint32Test;
    float32     base2Float32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleBaseClass2);
    REFL_MEMBER(SimpleBaseClass2, base2Uint32Test);
    REFL_MEMBER(SimpleBaseClass2, base2Float32Test);
REFL_IMPL_CLASS_END(SimpleBaseClass2);

class MultipleInheritanceClass : public SimpleBaseClass, public SimpleBaseClass2 {
public:
    REFL_DEFINE_CLASS(MultipleInheritanceClass);
    MultipleInheritanceClass() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleBaseClass, MultipleInheritanceClass);
    REFL_ADD_PARENT(MultipleInheritanceClass, SimpleBaseClass);
    REFL_ADD_PARENT(MultipleInheritanceClass, SimpleBaseClass2);
    REFL_MEMBER(MultipleInheritanceClass, derivedBoolTest);
    REFL_MEMBER(MultipleInheritanceClass, derivedInt16Test);
    REFL_MEMBER(MultipleInheritanceClass, derivedInt16Test2);
REFL_IMPL_CLASS_END(MultipleInheritanceClass);

//====================================================
TEST(ReflectionTest, TestMultipleInheritance) {
    MultipleInheritanceClass testInheritance;
    testInheritance.baseUint32Test      = s_uint32Value;
    testInheritance.baseFloat32Test     = s_float32Value;
    testInheritance.base2Uint32Test     = 310000;
    testInheritance.base2Float32Test    = 31.31f;
    testInheritance.derivedBoolTest     = s_boolValue;
    testInheritance.derivedInt16Test    = s_int16Value;
    testInheritance.derivedInt16Test2   = s_uint16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testMultipleInheritance.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = testInheritance.Serialize(testStream);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testMultipleInheritance.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    MultipleInheritanceClass * loadTypes = dynamic_cast<MultipleInheritanceClass *>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(310000,             loadTypes->base2Uint32Test);
    EXPECT_EQ(31.31f,             loadTypes->base2Float32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test);
    EXPECT_EQ(s_uint16Value,      loadTypes->derivedInt16Test2);

    delete loadTypes;
    loadTypes = NULL;
}


