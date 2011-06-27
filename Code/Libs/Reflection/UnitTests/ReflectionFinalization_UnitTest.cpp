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
// Test finalization
//

class SimpleFinalizationClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleFinalizationClass);
    SimpleFinalizationClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f),
        classFinalized(false)
    {
        InitReflType();
    }

    static void Finalize(ReflClass * inst) {
        SimpleFinalizationClass * final = ReflCast<SimpleFinalizationClass>(inst);
        if (final != NULL) {
            final->classFinalized = true;
        }
    }
//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
    bool        classFinalized;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleFinalizationClass);
    REFL_FINALIZATION_FUNC(SimpleFinalizationClass::Finalize);
    REFL_MEMBER(baseUint32Test);
    REFL_MEMBER(baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleFinalizationClass);

//====================================================
TEST(ReflectionTest, TestFinalization) {
    SimpleFinalizationClass testFinalization;
    testFinalization.baseUint32Test   = s_uint32Value;
    testFinalization.baseFloat32Test  = s_float32Value;

    IStructuredTextStream * testStream = StreamCreateXML(L"testFinalization.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = ReflLibrary::Serialize(testStream, &testFinalization);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testFinalization.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleFinalizationClass * loadTypes = ReflCast<SimpleFinalizationClass>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);
    EXPECT_EQ(true,               loadTypes->classFinalized);

    delete loadTypes;
    loadTypes = NULL;
}

