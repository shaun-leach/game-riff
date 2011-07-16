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
    REFL_MEMBER(baseUint32Test);
    REFL_MEMBER(baseFloat32Test);
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
    REFL_MEMBER(baseUint32Test);
    REFL_MEMBER(baseFloat32Test);
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
    REFL_ADD_PARENT(SimpleCastBaseClass);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
REFL_IMPL_CLASS_END(SimpleCastDerivedClass);

//====================================================
TEST(ReflectionTest, TestSimpleCasting) {
    SimpleCastDerivedClass testCasting;
    testCasting.baseUint32Test      = s_uint32Value;
    testCasting.baseFloat32Test     = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testSimpleCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testSimpleCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleCastDerivedClass * actual = ReflCast<SimpleCastDerivedClass>(inst);
    ASSERT_TRUE(actual != NULL);
    SimpleCastDerivedClass * self = ReflCast<SimpleCastDerivedClass>(actual);
    EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,    actual->baseUint32Test);
    EXPECT_EQ(s_float32Value,   actual->baseFloat32Test);
    EXPECT_EQ(s_boolValue,      actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    EXPECT_EQ(true, base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    SimpleCastBaseClass * base2 = ReflCast<SimpleCastBaseClass>(actual);
    EXPECT_EQ(base, base2);

    delete actual;
    actual = NULL;
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
    REFL_ADD_PARENT(SimpleCastDerivedClass);
    REFL_MEMBER(moreDerivedBoolTest);
    REFL_MEMBER(moreDerivedFloat32Test);
    REFL_MEMBER(moreDerivedBoolTest2);
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

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testClassHierarchyCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testClassHierarchyCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    MoreDerivedCastClass * actual = ReflCast<MoreDerivedCastClass>(inst);
    ASSERT_TRUE(actual != NULL);
    MoreDerivedCastClass * self = ReflCast<MoreDerivedCastClass>(actual);
    EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,      actual->baseUint32Test);
    EXPECT_EQ(s_float32Value,     actual->baseFloat32Test);
    EXPECT_EQ(s_boolValue,        actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       actual->derivedInt16Test);
    EXPECT_EQ(s_int16Value,       actual->derivedInt16Test2);
    EXPECT_EQ(s_boolValue,        actual->moreDerivedBoolTest);
    EXPECT_EQ(s_float32Value,     actual->moreDerivedFloat32Test);
    EXPECT_EQ(s_boolValue,        actual->moreDerivedBoolTest2);

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

    SimpleCastBaseClass * base1 = ReflCast<SimpleCastBaseClass>(actual);
    EXPECT_EQ(base, base1);
    SimpleCastDerivedClass * base2 = ReflCast<SimpleCastDerivedClass>(actual);
    EXPECT_EQ(derived, base2);

    delete actual;
    actual = NULL;
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
    REFL_MEMBER(base2Uint32Test);
    REFL_MEMBER(base2Float32Test);
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
    REFL_ADD_PARENT(SimpleCastBaseClass);
    REFL_ADD_PARENT(SimpleCastBaseClass2);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
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

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, static_cast<SimpleCastBaseClass *>(&testCasting));
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    MultipleInheritanceCastClass * actual = ReflCast<MultipleInheritanceCastClass>(inst);
    ASSERT_TRUE(actual != NULL);
    //MultipleInheritanceCastClass * self = ReflCast<MultipleInheritanceCastClass>(actual);
    //EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,      actual->baseUint32Test);
    EXPECT_EQ(s_float32Value,     actual->baseFloat32Test);
    EXPECT_EQ(310000,             actual->base2Uint32Test);
    EXPECT_EQ(31.31f,             actual->base2Float32Test);
    EXPECT_EQ(s_boolValue,        actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,       actual->derivedInt16Test);
    EXPECT_EQ(s_uint16Value,      actual->derivedInt16Test2);

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

    //SimpleCastBaseClass * base1 = ReflCast<SimpleCastBaseClass>(actual);
    //EXPECT_EQ(base, base1);
    //SimpleCastBaseClass2 * base2FromActual = ReflCast<SimpleCastBaseClass2>(actual);
    //EXPECT_EQ(base2, base2FromActual);

    delete actual;
    actual = NULL;
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
    REFL_ADD_PARENT(SimpleCastBaseClass2);
    REFL_MEMBER(base3Uint32Test);
    REFL_MEMBER(base3Float32Test);
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
    REFL_ADD_PARENT(SimpleCastBaseClass);
    REFL_ADD_PARENT(SimpleCastBaseClass3);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
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

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testComplexMultipleInheritanceCasting.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, static_cast<SimpleCastBaseClass *>(&testCasting));
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testComplexMultipleInheritanceCasting.xml");
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

    delete actual;
    actual = NULL;
}

//====================================================
TEST(ReflectionTest, TestCastingFromMultipleBases) {

    IStructuredTextStreamPtr testStream = StreamOpenXML(L"testComplexMultipleInheritanceCasting.xml");
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

    //ReflClass            * instD    = ReflCast<ReflClass>(actual);
    ReflClass            * inst1    = ReflCast<ReflClass>(base1);
    ReflClass            * inst2    = ReflCast<ReflClass>(base2);
    ReflClass            * inst3    = ReflCast<ReflClass>(base3);
    EXPECT_EQ(inst, inst1);
    EXPECT_EQ(inst, inst2);
    EXPECT_EQ(inst, inst3);

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

//  SimpleCastBaseClass  * base1FromActual = ReflCast<SimpleCastBaseClass>(actual);
//  SimpleCastBaseClass2 * base2FromActual = ReflCast<SimpleCastBaseClass2>(actual);
//  SimpleCastBaseClass3 * base3FromActual = ReflCast<SimpleCastBaseClass3>(actual);
//  EXPECT_EQ(base1, base1FromActual);
//  EXPECT_EQ(base2, base2FromActual);
//  EXPECT_EQ(base3, base3FromActual);

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
    REFL_ADD_PARENT(SimpleCastBaseClass);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
REFL_IMPL_CLASS_END(SimpleCastWithVirtualsNonreflectedBase);

//====================================================
TEST(ReflectionTest, TestCastingWVirtualsNonreflectedBase) {
    SimpleCastWithVirtualsNonreflectedBase testCasting;
    testCasting.baseUint32Test      = s_uint32Value;
    testCasting.baseFloat32Test     = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testCastingWVirtualsNonreflectedBase.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testCastingWVirtualsNonreflectedBase.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleCastWithVirtualsNonreflectedBase * actual = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(inst);
    ASSERT_TRUE(actual != NULL);

    SimpleCastWithVirtualsNonreflectedBase * self = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(actual);
    EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,    actual->baseUint32Test);
    EXPECT_EQ(s_float32Value,   actual->baseFloat32Test);
    EXPECT_EQ(s_boolValue,      actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base = ReflCast<SimpleCastBaseClass>(inst);
    EXPECT_EQ(true, base != NULL);
    EXPECT_EQ(s_uint32Value,    base->baseUint32Test);
    EXPECT_EQ(s_float32Value,   base->baseFloat32Test);

    SimpleCastWithVirtualsNonreflectedBase * derived1 = ReflCast<SimpleCastWithVirtualsNonreflectedBase>(base);
    EXPECT_EQ(actual, derived1);

    ReflClass * inst1 = ReflCast<ReflClass>(base);
    EXPECT_EQ(inst, inst1);

    ReflClass * inst2 = ReflCast<ReflClass>(actual);
    EXPECT_EQ(inst, inst2);

    SimpleCastBaseClass * base2 = ReflCast<SimpleCastBaseClass>(actual);
    EXPECT_EQ(true, base2 != NULL);
    EXPECT_EQ(base, base2);

    delete actual;
    actual = NULL;
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
    REFL_MEMBER(basevUint32Test);
    REFL_MEMBER(basevFloat32Test);
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
    REFL_ADD_PARENT(VirtualBaseClass);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
REFL_IMPL_CLASS_END(CastWithVirtualsInBase);

//====================================================
TEST(ReflectionTest, TestCastingWVirtualsInBase) {
    CastWithVirtualsInBase testCasting;
    testCasting.basevUint32Test     = s_uint32Value;
    testCasting.basevFloat32Test    = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testCastingWVirtualBase.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testCasting);
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testCastingWVirtualBase.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    CastWithVirtualsInBase * actual = ReflCast<CastWithVirtualsInBase>(inst);
    ASSERT_TRUE(actual != NULL);

    CastWithVirtualsInBase * self = ReflCast<CastWithVirtualsInBase>(actual);
    EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,    actual->basevUint32Test);
    EXPECT_EQ(s_float32Value,   actual->basevFloat32Test);
    EXPECT_EQ(s_boolValue,      actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    VirtualBaseClass * base = ReflCast<VirtualBaseClass>(inst);
    EXPECT_EQ(true, base != NULL);
    EXPECT_EQ(s_uint32Value,    base->basevUint32Test);
    EXPECT_EQ(s_float32Value,   base->basevFloat32Test);

    VirtualBaseClass * baseFromActual = ReflCast<VirtualBaseClass>(actual);
    EXPECT_EQ(base, baseFromActual);

    delete actual;
    actual = NULL;
}

class CastWithVirtualsIn2ndBase : public SimpleCastBaseClass, public VirtualBaseClass {
public:
    REFL_DEFINE_CLASS(CastWithVirtualsIn2ndBase);
    CastWithVirtualsIn2ndBase() :
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

REFL_IMPL_CLASS_BEGIN(VirtualBaseClass, CastWithVirtualsIn2ndBase);
    REFL_ADD_PARENT(VirtualBaseClass);
    REFL_ADD_PARENT(SimpleCastBaseClass);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
REFL_IMPL_CLASS_END(CastWithVirtualsIn2ndBase);

//====================================================
TEST(ReflectionTest, TestCastingWVirtualsIn2ndBase) {
    CastWithVirtualsIn2ndBase testCasting;
    testCasting.baseUint32Test      = 330000;
    testCasting.baseFloat32Test     = 33.33f;
    testCasting.basevUint32Test     = s_uint32Value;
    testCasting.basevFloat32Test    = s_float32Value;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testCastingWVirtual2ndBase.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, static_cast<VirtualBaseClass *>(&testCasting));
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testCastingWVirtual2ndBase.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    CastWithVirtualsIn2ndBase * actual = ReflCast<CastWithVirtualsIn2ndBase>(inst);
    ASSERT_TRUE(actual != NULL);

    //CastWithVirtualsIn2ndBase * self = ReflCast<CastWithVirtualsIn2ndBase>(actual);
    //EXPECT_EQ(actual, self);

    EXPECT_EQ(330000,           actual->baseUint32Test);
    EXPECT_EQ(33.33f,           actual->baseFloat32Test);
    EXPECT_EQ(s_uint32Value,    actual->basevUint32Test);
    EXPECT_EQ(s_float32Value,   actual->basevFloat32Test);
    EXPECT_EQ(s_boolValue,      actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    SimpleCastBaseClass * base1 = ReflCast<SimpleCastBaseClass>(inst);
    EXPECT_EQ(true, base1 != NULL);
    EXPECT_EQ(330000,           base1->baseUint32Test);
    EXPECT_EQ(33.33f,           base1->baseFloat32Test);

    //VirtualBaseClass * base1FromActual = ReflCast<VirtualBaseClass>(actual);
    //EXPECT_EQ(base1, base1FromActual);

    VirtualBaseClass * base2 = ReflCast<VirtualBaseClass>(inst);
    EXPECT_EQ(true, base2 != NULL);
    EXPECT_EQ(s_uint32Value,    base2->basevUint32Test);
    EXPECT_EQ(s_float32Value,   base2->basevFloat32Test);

    //VirtualBaseClass * base2FromActual = ReflCast<VirtualBaseClass>(actual);
    //EXPECT_EQ(base2, base2FromActual);

    SimpleCastBaseClass * base12 = ReflCast<SimpleCastBaseClass>(base2);
    VirtualBaseClass    * base21 = ReflCast<VirtualBaseClass>(base1);
    EXPECT_EQ(base1, base12);
    EXPECT_EQ(base2, base21);

    delete actual;
    actual = NULL;
}

//////////////////////////////////////////////////////
//
// Test casting w/ virtuals in the base class
//

class Virtual2ndBaseClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(Virtual2ndBaseClass);
    Virtual2ndBaseClass() :
        basev2Uint32Test(0),
        basev2Float32Test(0.0f)
    {
        InitReflType();
    }

    virtual ~Virtual2ndBaseClass() {
    }
//private:
    uint32      basev2Uint32Test;
    float32     basev2Float32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, Virtual2ndBaseClass);
    REFL_MEMBER(basev2Uint32Test);
    REFL_MEMBER(basev2Float32Test);
REFL_IMPL_CLASS_END(Virtual2ndBaseClass);

class CastWithVirtualsIn2Bases : public VirtualBaseClass, public Virtual2ndBaseClass {
public:
    REFL_DEFINE_CLASS(CastWithVirtualsIn2Bases);
    CastWithVirtualsIn2Bases() :
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

REFL_IMPL_CLASS_BEGIN(VirtualBaseClass, CastWithVirtualsIn2Bases);
    REFL_ADD_PARENT(VirtualBaseClass);
    REFL_ADD_PARENT(Virtual2ndBaseClass);
    REFL_MEMBER(derivedBoolTest);
    REFL_MEMBER(derivedInt16Test);
    REFL_MEMBER(derivedInt16Test2);
REFL_IMPL_CLASS_END(CastWithVirtualsIn2Bases);

//====================================================
TEST(ReflectionTest, TestCastingWVirtualsIn2Bases) {
    CastWithVirtualsIn2Bases testCasting;
    testCasting.basevUint32Test     = s_uint32Value;
    testCasting.basevFloat32Test    = s_float32Value;
    testCasting.basev2Uint32Test      = 330000;
    testCasting.basev2Float32Test     = 33.33f;
    testCasting.derivedBoolTest     = s_boolValue;
    testCasting.derivedInt16Test    = s_int16Value;
    testCasting.derivedInt16Test2   = s_int16Value;

    IStructuredTextStreamPtr testStream = StreamCreateXML(L"testCastingWVirtual2Bases.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, static_cast<VirtualBaseClass *>(&testCasting));
    EXPECT_EQ(true, result);
    testStream->Save();

    testStream = StreamOpenXML(L"testCastingWVirtual2Bases.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    CastWithVirtualsIn2Bases * actual = ReflCast<CastWithVirtualsIn2Bases>(inst);
    ASSERT_TRUE(actual != NULL);

    //CastWithVirtualsIn2ndBase * self = ReflCast<CastWithVirtualsIn2ndBase>(actual);
    //EXPECT_EQ(actual, self);

    EXPECT_EQ(s_uint32Value,    actual->basevUint32Test);
    EXPECT_EQ(s_float32Value,   actual->basevFloat32Test);
    EXPECT_EQ(330000,           actual->basev2Uint32Test);
    EXPECT_EQ(33.33f,           actual->basev2Float32Test);
    EXPECT_EQ(s_boolValue,      actual->derivedBoolTest);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test);
    EXPECT_EQ(s_int16Value,     actual->derivedInt16Test2);

    SimpleOtherBaseClass * castNull = ReflCast<SimpleOtherBaseClass>(inst);
    EXPECT_EQ(true, castNull == NULL);

    VirtualBaseClass * base1 = ReflCast<VirtualBaseClass>(inst);
    EXPECT_EQ(true, base1 != NULL);
    EXPECT_EQ(s_uint32Value,    base1->basevUint32Test);
    EXPECT_EQ(s_float32Value,   base1->basevFloat32Test);

    //VirtualBaseClass * base1FromActual = ReflCast<VirtualBaseClass>(actual);
    //EXPECT_EQ(base1, base1FromActual);

    Virtual2ndBaseClass * base2 = ReflCast<Virtual2ndBaseClass>(inst);
    EXPECT_EQ(true, base2 != NULL);
    EXPECT_EQ(330000,           base2->basev2Uint32Test);
    EXPECT_EQ(33.33f,           base2->basev2Float32Test);

    //VirtualBaseClass * base2FromActual = ReflCast<VirtualBaseClass>(actual);
    //EXPECT_EQ(base2, base2FromActual);

    VirtualBaseClass    * base12 = ReflCast<VirtualBaseClass>(base2);
    Virtual2ndBaseClass * base21 = ReflCast<Virtual2ndBaseClass>(base1);
    EXPECT_EQ(base1, base12);
    EXPECT_EQ(base2, base21);

    delete actual;
    actual = NULL;
}

