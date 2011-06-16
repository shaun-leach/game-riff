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
// Test simple casting
//

class SimpleOtherBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleOtherBaseClass);
    SimpleOtherBaseClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleOtherBaseClass);
    REFL_MEMBER(SimpleOtherBaseClass, baseUint32Test);
    REFL_MEMBER(SimpleOtherBaseClass, baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleOtherBaseClass);

class SimpleCastBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleCastBaseClass);
    SimpleCastBaseClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleCastBaseClass);
    REFL_MEMBER(SimpleCastBaseClass, baseUint32Test);
    REFL_MEMBER(SimpleCastBaseClass, baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleCastBaseClass);

class SimpleCastDerivedClass : public SimpleCastBaseClass {
public:
    REFL_DEFINE_CLASS(SimpleCastDerivedClass);
    SimpleCastDerivedClass() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, SimpleCastDerivedClass);
    REFL_ADD_PARENT(SimpleCastDerivedClass, SimpleCastBaseClass);
    REFL_MEMBER(SimpleCastDerivedClass, derivedBoolTest);
    REFL_MEMBER(SimpleCastDerivedClass, derivedInt16Test);
    REFL_MEMBER(SimpleCastDerivedClass, derivedInt16Test2);
REFL_IMPL_CLASS_END(SimpleCastDerivedClass);

//====================================================
TEST(ReflectionTest, TestSimpleCasting) {
    SimpleCastDerivedClass testCasting;
    testCasting.baseUint32Test      = s_uint32Value;
    testCasting.baseFloat32Test     = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testSimpleCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testSimpleCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleCastDerivedClass * loadTypes = ReflCast<SimpleCastDerivedClass>(inst);
    ASSERT_TRUE(loadTypes != NULL);
    SimpleCastDerivedClass * self = ReflCast<SimpleCastDerivedClass>(loadTypes);
    EXPECT_EQ(loadTypes, self);

    EXPECT_EQ(s_uint32Value,    loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,   loadTypes->baseFloat32Test);
    EXPECT_EQ(s_boolValue,      loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     loadTypes->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     loadTypes->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    EXPECT_EQ(true, base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test class hierarchy
//

class MoreDerivedCastClass : public SimpleCastDerivedClass {
public:
    REFL_DEFINE_CLASS(MoreDerivedCastClass);
    MoreDerivedCastClass() :
        moreDerivedBoolTest(false),
        moreDerivedFloat32Test(0.0f),
        moreDerivedBoolTest2(false)
    {
        InitReflType();
    }

//private:
    bool        moreDerivedBoolTest;
    float32     moreDerivedFloat32Test;
    bool        moreDerivedBoolTest2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastDerivedClass, MoreDerivedCastClass);
    REFL_ADD_PARENT(MoreDerivedCastClass, SimpleCastDerivedClass);
    REFL_MEMBER(MoreDerivedCastClass, moreDerivedBoolTest);
    REFL_MEMBER(MoreDerivedCastClass, moreDerivedFloat32Test);
    REFL_MEMBER(MoreDerivedCastClass, moreDerivedBoolTest2);
REFL_IMPL_CLASS_END(MoreDerivedCastClass);

//====================================================
TEST(ReflectionTest, TestClassHierarchyCasting) {
    MoreDerivedCastClass testCasting;
    testCasting.baseUint32Test          = s_uint32Value;
    testCasting.baseFloat32Test         = s_float32Value;
    testCasting.derivedBoolTest         = s_boolValue;
    testCasting.derivedInt16Test        = s_int16Value;
    testCasting.derivedInt16Test2       = s_int16Value;
    testCasting.moreDerivedBoolTest     = s_boolValue;
    testCasting.moreDerivedFloat32Test  = s_float32Value;
    testCasting.moreDerivedBoolTest2    = s_boolValue;

    IStructuredTextStream * testStream = StreamCreateXML(L"testClassHierarchyCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testClassHierarchyCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    MoreDerivedCastClass * loadTypes = ReflCast<MoreDerivedCastClass>(inst);
    ASSERT_TRUE(loadTypes != NULL);
    MoreDerivedCastClass * self = ReflCast<MoreDerivedCastClass>(loadTypes);
    EXPECT_EQ(loadTypes, self);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test2);
    EXPECT_EQ(s_boolValue,        loadTypes->moreDerivedBoolTest);
    EXPECT_EQ(s_float32Value,     loadTypes->moreDerivedFloat32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->moreDerivedBoolTest2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    ASSERT_TRUE(base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    SimpleCastDerivedClass * derived = ReflCast<SimpleCastDerivedClass>(inst);
    ASSERT_TRUE(derived != NULL);
    EXPECT_EQ(s_uint32Value,    derived->baseUint32Test);
    EXPECT_EQ(s_float32Value,   derived->baseFloat32Test);
    EXPECT_EQ(s_int16Value,     derived->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     derived->derivedInt16Test2);
    EXPECT_EQ(s_boolValue,      derived->derivedBoolTest);

    SimpleCastDerivedClass * otherDerived = ReflCast<SimpleCastDerivedClass>(base);
    ASSERT_TRUE(otherDerived != NULL);
    EXPECT_EQ(derived->baseUint32Test,      otherDerived->baseUint32Test);
    EXPECT_EQ(derived->baseFloat32Test,     otherDerived->baseFloat32Test);
    EXPECT_EQ(derived->derivedInt16Test,    otherDerived->derivedInt16Test);
    EXPECT_EQ(derived->derivedInt16Test2,   otherDerived->derivedInt16Test2);
    EXPECT_EQ(derived->derivedBoolTest,     otherDerived->derivedBoolTest);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test multiple inheritance casting
//

class SimpleCastBaseClass2 : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleCastBaseClass2);
    SimpleCastBaseClass2() :
        base2Uint32Test(0),
        base2Float32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      base2Uint32Test;
    float32     base2Float32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleCastBaseClass2);
    REFL_MEMBER(SimpleCastBaseClass2, base2Uint32Test);
    REFL_MEMBER(SimpleCastBaseClass2, base2Float32Test);
REFL_IMPL_CLASS_END(SimpleCastBaseClass2);

class MultipleInheritanceCastClass : public SimpleCastBaseClass, public SimpleCastBaseClass2 {
public:
    REFL_DEFINE_CLASS(MultipleInheritanceCastClass);
    MultipleInheritanceCastClass() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, MultipleInheritanceCastClass);
    REFL_ADD_PARENT(MultipleInheritanceCastClass, SimpleCastBaseClass);
    REFL_ADD_PARENT(MultipleInheritanceCastClass, SimpleCastBaseClass2);
    REFL_MEMBER(MultipleInheritanceCastClass, derivedBoolTest);
    REFL_MEMBER(MultipleInheritanceCastClass, derivedInt16Test);
    REFL_MEMBER(MultipleInheritanceCastClass, derivedInt16Test2);
REFL_IMPL_CLASS_END(MultipleInheritanceCastClass);

//====================================================
TEST(ReflectionTest, TestMultipleInheritanceCasting) {
    MultipleInheritanceCastClass testCasting;
    testCasting.baseUint32Test      = s_uint32Value;
    testCasting.baseFloat32Test     = s_float32Value;
    testCasting.base2Uint32Test     = 310000;
    testCasting.base2Float32Test    = 31.31f;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_uint16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, static_cast<SimpleCastBaseClass *>(&testCasting));
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    MultipleInheritanceCastClass * loadTypes = ReflCast<MultipleInheritanceCastClass>(inst);
    ASSERT_TRUE(loadTypes != NULL);
    //MultipleInheritanceCastClass * self = ReflCast<MultipleInheritanceCastClass>(loadTypes);
    //EXPECT_EQ(loadTypes, self);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(310000,             loadTypes->base2Uint32Test);
    EXPECT_EQ(31.31f,             loadTypes->base2Float32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test);
    EXPECT_EQ(s_uint16Value,      loadTypes->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    ASSERT_TRUE(base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    SimpleCastBaseClass2 * base2 = ReflCast<SimpleCastBaseClass2>(inst);
    EXPECT_EQ(true, static_cast<ReflClass *>(base2) != static_cast<ReflClass *>(base));
    ASSERT_TRUE(base2 != NULL);
    EXPECT_EQ(310000,    base2->base2Uint32Test);
    EXPECT_EQ(31.31f,    base2->base2Float32Test);

    SimpleCastBaseClass2 * otherBase2 = ReflCast<SimpleCastBaseClass2>(base);
    ASSERT_TRUE(otherBase2 != NULL);
    EXPECT_EQ(true, base2 == otherBase2);
    EXPECT_EQ(base2->base2Uint32Test,   otherBase2->base2Uint32Test);
    EXPECT_EQ(base2->base2Float32Test,  otherBase2->base2Float32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test casting multiple inheritance with derived base classes 
//

class SimpleCastBaseClass3 : public SimpleCastBaseClass2 {
public:
    REFL_DEFINE_CLASS(SimpleCastBaseClass3);
    SimpleCastBaseClass3() :
        base3Uint32Test(0),
        base3Float32Test(0.0f)
    {
        InitReflType();
    }

//private:
    uint32      base3Uint32Test;
    float32     base3Float32Test;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass2, SimpleCastBaseClass3);
    REFL_ADD_PARENT(SimpleCastBaseClass3, SimpleCastBaseClass2);
    REFL_MEMBER(SimpleCastBaseClass3, base3Uint32Test);
    REFL_MEMBER(SimpleCastBaseClass3, base3Float32Test);
REFL_IMPL_CLASS_END(SimpleCastBaseClass3);

class ComplexInheritanceCastClass : public SimpleCastBaseClass, public SimpleCastBaseClass3 {
public:
    REFL_DEFINE_CLASS(ComplexInheritanceCastClass);
    ComplexInheritanceCastClass() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, ComplexInheritanceCastClass);
    REFL_ADD_PARENT(ComplexInheritanceCastClass, SimpleCastBaseClass);
    REFL_ADD_PARENT(ComplexInheritanceCastClass, SimpleCastBaseClass3);
    REFL_MEMBER(ComplexInheritanceCastClass, derivedBoolTest);
    REFL_MEMBER(ComplexInheritanceCastClass, derivedInt16Test);
    REFL_MEMBER(ComplexInheritanceCastClass, derivedInt16Test2);
REFL_IMPL_CLASS_END(ComplexInheritanceCastClass);

//====================================================
TEST(ReflectionTest, TestComplexInheritanceCasting) {
    ComplexInheritanceCastClass testCasting;
    testCasting.baseUint32Test      = s_uint32Value;
    testCasting.baseFloat32Test     = s_float32Value;
    testCasting.base2Uint32Test     = 310000;
    testCasting.base2Float32Test    = 31.31f;
    testCasting.base3Uint32Test     = 300000;
    testCasting.base3Float32Test    = 30.30f;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_uint16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testComplexMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, static_cast<SimpleCastBaseClass *>(&testCasting));
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testComplexMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    ComplexInheritanceCastClass * loadTypes = ReflCast<ComplexInheritanceCastClass>(inst);
    ASSERT_TRUE(loadTypes != NULL);
    //ComplexInheritanceCastClass * self = ReflCast<ComplexInheritanceCastClass>(loadTypes);
    //EXPECT_EQ(loadTypes, self);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(310000,             loadTypes->base2Uint32Test);
    EXPECT_EQ(31.31f,             loadTypes->base2Float32Test);
    EXPECT_EQ(300000,             loadTypes->base3Uint32Test);
    EXPECT_EQ(30.30f,             loadTypes->base3Float32Test);
    EXPECT_EQ(s_boolValue,        loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       loadTypes->derivedInt16Test);
    EXPECT_EQ(s_uint16Value,      loadTypes->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    ASSERT_TRUE(base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    SimpleCastBaseClass2 * base2 = ReflCast<SimpleCastBaseClass2>(inst);
    EXPECT_EQ(true, static_cast<ReflClass *>(base2) != static_cast<ReflClass *>(base));
    ASSERT_TRUE(base2 != NULL);
    EXPECT_EQ(310000,    base2->base2Uint32Test);
    EXPECT_EQ(31.31f,    base2->base2Float32Test);

    SimpleCastBaseClass2 * otherBase2 = ReflCast<SimpleCastBaseClass2>(base);
    ASSERT_TRUE(otherBase2 != NULL);
    EXPECT_EQ(true, base2 == otherBase2);
    EXPECT_EQ(base2->base2Uint32Test,   otherBase2->base2Uint32Test);
    EXPECT_EQ(base2->base2Float32Test,  otherBase2->base2Float32Test);

    SimpleCastBaseClass3 * base3 = ReflCast<SimpleCastBaseClass3>(inst);
    EXPECT_EQ(true, static_cast<ReflClass *>(base3) != static_cast<ReflClass *>(base));
    ASSERT_TRUE(base3 != NULL);
    EXPECT_EQ(310000,    base3->base2Uint32Test);
    EXPECT_EQ(31.31f,    base3->base2Float32Test);
    EXPECT_EQ(300000,    base3->base3Uint32Test);
    EXPECT_EQ(30.30f,    base3->base3Float32Test);

    SimpleCastBaseClass3 * otherBase3 = ReflCast<SimpleCastBaseClass3>(base2);
    ASSERT_TRUE(otherBase3 != NULL);
    EXPECT_EQ(true, base3 == otherBase3);
    EXPECT_EQ(base3->base3Uint32Test,   otherBase3->base3Uint32Test);
    EXPECT_EQ(base3->base3Float32Test,  otherBase3->base3Float32Test);

    SimpleCastBaseClass2 * otherBase22 = ReflCast<SimpleCastBaseClass2>(base3);
    ASSERT_TRUE(otherBase22 != NULL);
    EXPECT_EQ(true, base2 == otherBase22);
    EXPECT_EQ(base2->base2Uint32Test,   otherBase22->base2Uint32Test);
    EXPECT_EQ(base2->base2Float32Test,  otherBase22->base2Float32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//====================================================
TEST(ReflectionTest, TestCastingFromMultipleBases) {

    IStructuredTextStream * testStream = StreamOpenXML(L"testComplexMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    ComplexInheritanceCastClass * actual = ReflCast<ComplexInheritanceCastClass>(inst);
    ASSERT_TRUE(actual != NULL);
    //ComplexInheritanceCastClass * self = ReflCast<ComplexInheritanceCastClass>(actual);
    //EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,      actual->baseUint32Test);
    EXPECT_EQ(s_float32Value,     actual->baseFloat32Test);
    EXPECT_EQ(310000,             actual->base2Uint32Test);
    EXPECT_EQ(31.31f,             actual->base2Float32Test);
    EXPECT_EQ(300000,             actual->base3Uint32Test);
    EXPECT_EQ(30.30f,             actual->base3Float32Test);
    EXPECT_EQ(s_boolValue,        actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       actual->derivedInt16Test);
    EXPECT_EQ(s_uint16Value,      actual->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass  * base1    = ReflCast<SimpleCastBaseClass>(inst);
    ASSERT_TRUE(base1 != NULL);
    EXPECT_EQ(s_uint32Value,      base1->baseUint32Test);
    EXPECT_EQ(s_float32Value,     base1->baseFloat32Test);
    SimpleCastBaseClass2 * base2    = ReflCast<SimpleCastBaseClass2>(inst);
    ASSERT_TRUE(base2 != NULL);
    EXPECT_EQ(310000,             base2->base2Uint32Test);
    EXPECT_EQ(31.31f,             base2->base2Float32Test);
    SimpleCastBaseClass3 * base3    = ReflCast<SimpleCastBaseClass3>(inst);
    ASSERT_TRUE(base3 != NULL);
    EXPECT_EQ(310000,             base3->base2Uint32Test);
    EXPECT_EQ(31.31f,             base3->base2Float32Test);
    EXPECT_EQ(300000,             base3->base3Uint32Test);
    EXPECT_EQ(30.30f,             base3->base3Float32Test);

    //ReflClass            * instD    = ReflCast<ReflClass>(loadTypes);
    ReflClass            * inst1    = ReflCast<ReflClass>(base1);
    ReflClass            * inst2    = ReflCast<ReflClass>(base2);
    ReflClass            * inst3    = ReflCast<ReflClass>(base3);
    EXPECT_EQ(inst, inst1);
    EXPECT_EQ(inst, inst2);
    EXPECT_EQ(inst, inst2);

    ComplexInheritanceCastClass * derived1 = ReflCast<ComplexInheritanceCastClass>(base1);
    ComplexInheritanceCastClass * derived2 = ReflCast<ComplexInheritanceCastClass>(base2);
    ComplexInheritanceCastClass * derived3 = ReflCast<ComplexInheritanceCastClass>(base3);
    EXPECT_EQ(actual, derived1);
    EXPECT_EQ(actual, derived2);
    EXPECT_EQ(actual, derived3);

    SimpleCastBaseClass  * base12 = ReflCast<SimpleCastBaseClass>(base2);
    SimpleCastBaseClass  * base13 = ReflCast<SimpleCastBaseClass>(base3);
    SimpleCastBaseClass2 * base21 = ReflCast<SimpleCastBaseClass2>(base1);
    SimpleCastBaseClass2 * base23 = ReflCast<SimpleCastBaseClass2>(base3);
    SimpleCastBaseClass3 * base31 = ReflCast<SimpleCastBaseClass3>(base1);
    SimpleCastBaseClass3 * base32 = ReflCast<SimpleCastBaseClass3>(base2);
    EXPECT_EQ(base1, base12);
    EXPECT_EQ(base1, base13);
    EXPECT_EQ(base2, base21);
    EXPECT_EQ(base2, base23);
    EXPECT_EQ(base3, base31);
    EXPECT_EQ(base3, base32);

    delete actual;
    actual = NULL;
}

//////////////////////////////////////////////////////
//
// Test simple casting w/ virtuals
//

class NonreflectedClass {
public:
    NonreflectedClass() :
        nfUint32Test(0),
        nfFloat32Test(0.0f)
    {
    }

    virtual ~NonreflectedClass() {
    }
//private:
    uint32      nfUint32Test;
    float32     nfFloat32Test;
};

class SimpleCastWithVirtualsNonreflectedBase : public NonreflectedClass, public SimpleCastBaseClass {
public:
    REFL_DEFINE_CLASS(SimpleCastWithVirtualsNonreflectedBase);
    SimpleCastWithVirtualsNonreflectedBase() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(SimpleCastBaseClass, SimpleCastWithVirtualsNonreflectedBase);
    REFL_ADD_PARENT(SimpleCastWithVirtualsNonreflectedBase, SimpleCastBaseClass);
    REFL_MEMBER(SimpleCastWithVirtualsNonreflectedBase, derivedBoolTest);
    REFL_MEMBER(SimpleCastWithVirtualsNonreflectedBase, derivedInt16Test);
    REFL_MEMBER(SimpleCastWithVirtualsNonreflectedBase, derivedInt16Test2);
REFL_IMPL_CLASS_END(SimpleCastWithVirtualsNonreflectedBase);

//====================================================
TEST(ReflectionTest, TestCastingWVirtualsNonreflectedBase) {
    SimpleCastWithVirtualsNonreflectedBase testCasting;
    testCasting.baseUint32Test      = s_uint32Value;
    testCasting.baseFloat32Test     = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testCastingWVirtualsNonreflectedBase.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testCastingWVirtualsNonreflectedBase.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleCastWithVirtualsNonreflectedBase * loadTypes = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    SimpleCastWithVirtualsNonreflectedBase * self = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(loadTypes);
    EXPECT_EQ(loadTypes, self);

    EXPECT_EQ(s_uint32Value,    loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,   loadTypes->baseFloat32Test);
    EXPECT_EQ(s_boolValue,      loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     loadTypes->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     loadTypes->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    EXPECT_EQ(true, base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test casting w/ virtuals in the base class
//

class VirtualBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(VirtualBaseClass);
    VirtualBaseClass() :
        basevUint32Test(0),
        basevFloat32Test(0.0f)
    {
        InitReflType();
    }

    virtual ~VirtualBaseClass() {
    }
//private:
    uint32      basevUint32Test;
    float32     basevFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, VirtualBaseClass);
    REFL_MEMBER(VirtualBaseClass, basevUint32Test);
    REFL_MEMBER(VirtualBaseClass, basevFloat32Test);
REFL_IMPL_CLASS_END(VirtualBaseClass);

class CastWithVirtualsInBase : public VirtualBaseClass {
public:
    REFL_DEFINE_CLASS(CastWithVirtualsInBase);
    CastWithVirtualsInBase() :
        derivedBoolTest(false),
        derivedInt16Test(0),
        derivedInt16Test2(0)
    {
        InitReflType();
    }

//private:
    bool        derivedBoolTest;
    int16       derivedInt16Test;
    int16       derivedInt16Test2;
};

REFL_IMPL_CLASS_BEGIN(VirtualBaseClass, CastWithVirtualsInBase);
    REFL_ADD_PARENT(CastWithVirtualsInBase, VirtualBaseClass);
    REFL_MEMBER(CastWithVirtualsInBase, derivedBoolTest);
    REFL_MEMBER(CastWithVirtualsInBase, derivedInt16Test);
    REFL_MEMBER(CastWithVirtualsInBase, derivedInt16Test2);
REFL_IMPL_CLASS_END(CastWithVirtualsInBase);

//====================================================
TEST(ReflectionTest, TestCastingWVirtualsInBase) {
    CastWithVirtualsInBase testCasting;
    testCasting.basevUint32Test     = s_uint32Value;
    testCasting.basevFloat32Test    = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testCastingWVirtualBase.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testCastingWVirtualBase.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    CastWithVirtualsInBase * loadTypes = ReflCast<CastWithVirtualsInBase>(inst);
    ASSERT_TRUE(loadTypes != NULL);

    CastWithVirtualsInBase * self = ReflCast<CastWithVirtualsInBase>(loadTypes);
    EXPECT_EQ(loadTypes, self);

    EXPECT_EQ(s_uint32Value,    loadTypes->basevUint32Test);
    EXPECT_EQ(s_float32Value,   loadTypes->basevFloat32Test);
    EXPECT_EQ(s_boolValue,      loadTypes->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     loadTypes->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     loadTypes->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    VirtualBaseClass * base = ReflCast<VirtualBaseClass>(inst);
    EXPECT_EQ(true, base != NULL);
    EXPECT_EQ(s_uint32Value,    base->basevUint32Test);
    EXPECT_EQ(s_float32Value,   base->basevFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}

