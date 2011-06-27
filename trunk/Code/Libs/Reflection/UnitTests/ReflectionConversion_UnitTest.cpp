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
// Test simple member conversion
//

class SimpleConversionClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleConversionClass);
    SimpleConversionClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
        InitReflType();
    }

    static void ConvertMember(
        ReflClass   * inst, 
        ReflHash      name, 
        ReflHash      oldType, 
        void        * data
     ) {
        SimpleConversionClass * conv = ReflCast<SimpleConversionClass>(inst);
        if (conv != NULL) {
           if (name == ReflHash(L"baseUint32Test") && oldType == ReflHash(L"float32")) {
               float32 * fdata = reinterpret_cast<float32 *>(data);

               conv->baseUint32Test = static_cast<uint32>(ceilf(*fdata));
           }
        }
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleConversionClass);
    REFL_MEMBER(baseUint32Test);
        REFL_ADD_MEMBER_CONVERSION(baseUint32Test, ConvertMember);
    REFL_MEMBER(baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleConversionClass);

//====================================================
TEST(ReflectionTest, TestSimpleConversion) {
    IStructuredTextStream * testStream = StreamOpenXML(L"testSimpleConversion.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleConversionClass * loadTypes = ReflCast<SimpleConversionClass>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(47,                 loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test simple member conversion with aliasing
//

class SimpleAliasingConversionClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleAliasingConversionClass);
    SimpleAliasingConversionClass() :
        baseInt32Test(0),
        baseFloat32Test(0.0f)
    {
        InitReflType();
    }

    static void ConvertMember(
        ReflClass   * inst, 
        ReflHash      name, 
        ReflHash      oldType, 
        void        * data
     ) {
        SimpleAliasingConversionClass * conv = ReflCast<SimpleAliasingConversionClass>(inst);
        if (conv != NULL) {
           if (name == ReflHash(L"floatTest") && oldType == ReflHash(L"float32")) {
               float32 * fdata = reinterpret_cast<float32 *>(data);

               conv->baseInt32Test = static_cast<int32>(ceilf(*fdata));
           }
        }
    }

//private:
    uint32      baseInt32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleAliasingConversionClass);
    REFL_MEMBER(baseInt32Test);
        REFL_ADD_MEMBER_ALIAS_W_CONVERSION(baseInt32Test, floatTest, ConvertMember);
    REFL_MEMBER(baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleAliasingConversionClass);

//====================================================
TEST(ReflectionTest, TestSimpleConversionWithAliasing) {
    IStructuredTextStream * testStream = StreamOpenXML(L"testSimpleConversionWithAliasing.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleAliasingConversionClass * loadTypes = ReflCast<SimpleAliasingConversionClass>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(-50,                loadTypes->baseInt32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}

//////////////////////////////////////////////////////
//
// Test class member conversion
//

class SimpleClassConversionClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleClassConversionClass);
    SimpleClassConversionClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f)
    {
        InitReflType();
    }

    static void ConvertMember(
        ReflClass   * inst, 
        ReflHash      name, 
        ReflHash      oldType, 
        void        * data
     ) {
        SimpleClassConversionClass * conv = ReflCast<SimpleClassConversionClass>(inst);
        if (conv != NULL) {
           if (name == ReflHash(L"memberUint32Test") && oldType == ReflHash(L"uint32")) {
               uint32 * udata = reinterpret_cast<uint32 *>(data);

               conv->baseUint32Test = *udata;
           }
        }
    }

//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleClassConversionClass);
    REFL_MEMBER(baseUint32Test);
        REFL_ADD_MEMBER_ALIAS_W_CONVERSION(baseUint32Test, classMemberTest, ConvertMember);
    REFL_MEMBER(baseFloat32Test);
REFL_IMPL_CLASS_END(SimpleClassConversionClass);

//====================================================
TEST(ReflectionTest, TestClassConversion) {
    IStructuredTextStream * testStream = StreamOpenXML(L"testClassConversion.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleClassConversionClass * loadTypes = ReflCast<SimpleClassConversionClass>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(s_uint32Value,      loadTypes->baseUint32Test);
    EXPECT_EQ(s_float32Value,     loadTypes->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}


