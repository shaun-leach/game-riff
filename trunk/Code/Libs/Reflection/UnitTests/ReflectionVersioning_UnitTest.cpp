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
// Test simple class aliasing
//

class SimpleVersioningClass : public ReflClass {
public:
    REFL_DEFINE_CLASS(SimpleVersioningClass);
    SimpleVersioningClass() :
        baseUint32Test(0),
        baseFloat32Test(0.0f),
        newUint32Test(0)
    {
        InitReflType();
    }

    static void VersioningFunc(
        IStructuredTextStream * stream, 
        ReflClassDesc         * desc, 
        unsigned                version, 
        ReflClass             * inst
    ) {
        SimpleVersioningClass * versioning = ReflCast<SimpleVersioningClass>(inst);
        if (versioning != NULL) {
            uint32 oldUintValue;
            float32 oldFloatValue;
            if (version == 0x1) {
                desc->RegisterTempBinding(ReflHash(L"oldUint32Test"), ReflHash(L"uint32"), &oldUintValue);
                desc->RegisterTempBinding(ReflHash(L"oldFloat32Test"), ReflHash(L"float32"), &oldFloatValue);
            }
            desc->DeserializeMembers(stream, inst);
            if (version == 0x1) {
                if (oldUintValue == s_uint32Value && oldFloatValue < s_float32Value) {
                    versioning->baseUint32Test  = 2 * s_uint32Value;
                    versioning->baseFloat32Test = 0.0f;
                }
                else if (oldUintValue == 0) {
                    versioning->baseUint32Test  = 0;
                }
            }

            desc->ClearAllTempBindings();
        }
    }
//private:
    uint32      baseUint32Test;
    float32     baseFloat32Test;
    uint32      newUint32Test;
};

REFL_IMPL_CLASS_BEGIN(ReflClass, SimpleVersioningClass);
    REFL_DO_MANUAL_VERSIONING(VersioningFunc, 2);
    REFL_MEMBER(SimpleVersioningClass, baseUint32Test);
    REFL_MEMBER(SimpleVersioningClass, baseFloat32Test);
    REFL_MEMBER_DEPRECATED(SimpleVersioningClass, oldUint32Test, uint32);
    REFL_MEMBER_DEPRECATED(SimpleVersioningClass, oldFloat32Test, float32);
REFL_IMPL_CLASS_END(SimpleVersioningClass);

//====================================================
TEST(ReflectionTest, TestSimpleVersioning) {
    IStructuredTextStream * testStream = StreamOpenXML(L"testSimpleVersioning.xml");
    ASSERT_TRUE(testStream != NULL);

    ReflClass * inst = ReflLibrary::Deserialize(testStream, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_TEST));
    ASSERT_TRUE(inst != NULL);
    SimpleVersioningClass * loadTypes = ReflCast<SimpleVersioningClass>(inst);
    EXPECT_EQ(true, loadTypes != NULL);

    EXPECT_EQ(2 * s_uint32Value,  loadTypes->baseUint32Test);
    EXPECT_EQ(0.0f,               loadTypes->baseFloat32Test);

    delete loadTypes;
    loadTypes = NULL;
}


