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

/*
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
    REFL_INDEX_CLASS,
    REFL_INDEX_ENUM,
    REFL_INDEX_ANGLE,
    REFL_INDEX_PERCENTAGE,
*/
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
        float32Test(0)
    {
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

    enum, angles, percentage
};

REFL_IMPL_CLASS_BEGIN(ReflClass, TestBaseTypes);
    REFL_MEMBER(TestBaseTypes, boolTest,     bool);
    REFL_MEMBER(TestBaseTypes, int8Test,     int8);
    REFL_MEMBER(TestBaseTypes, uint8Test,    uint8);
    REFL_MEMBER(TestBaseTypes, int16Test,    int16);
    REFL_MEMBER(TestBaseTypes, uint16Test,   uint16);
    REFL_MEMBER(TestBaseTypes, int32Test,    int32);
    REFL_MEMBER(TestBaseTypes, uint32Test,   uint32);
    REFL_MEMBER(TestBaseTypes, int64Test,    int64);
    REFL_MEMBER(TestBaseTypes, uint64Test,   uint64);
    REFL_MEMBER(TestBaseTypes, float32Test,  float32);
REFL_IMPL_CLASS_END(TestBaseTypes);

static const bool      s_boolValue       =  true;
static const int8      s_int8Value       = -8;
static const uint8     s_uint8Value      =  8;
static const int16     s_int16Value      = -1600;
static const uint16    s_uint16Value     =  1600;
static const int32     s_int32Value      = -320000;
static const uint32    s_uint32Value     =  320000;
static const int64     s_int64Value      = -6400000;
static const uint64    s_uint64Value     =  6400000;
static const float32   s_float32Value    =  32.32f;

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

    IStructuredTextStream * testStream = StreamCreateXML(L"testBaseTypes.xml");
    ASSERT_TRUE(testStream != NULL);
    bool result = testTypes.Serialize(testStream);
    EXPECT_EQ(true, result);
    testStream->Save();
    delete testStream;
    testStream = NULL;

    testStream = StreamOpenXML(L"testBaseTypes.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    TestBaseTypes * loadTypes = dynamic_cast<TestBaseTypes *>(inst);
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
}


