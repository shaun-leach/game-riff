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

TEST(ReflectionTest, TestTypes) {
    EXPECT_EQ(REFL_TYPE_INT8,         0);
    EXPECT_EQ(REFL_TYPE_INT16,        1);
    EXPECT_EQ(REFL_TYPE_INT32,        2);
    EXPECT_EQ(REFL_TYPE_FLOAT32,        3);
    EXPECT_EQ(REFL_TYPE_STRING,       4);
    EXPECT_EQ(REFL_TYPE_CLASS,        5);
    EXPECT_EQ(REFL_TYPE_ENUM,         6);
    EXPECT_EQ(REFL_TYPE_FIXED_ARRAY,  7);
    EXPECT_EQ(REFL_TYPE_VAR_ARRAY,    8);
    EXPECT_EQ(REFL_TYPE_POINTER,      9);
}

class Dummy : public ReflClass {
public:
    REFL_DEFINE_CLASS(Dummy);
private:
    int test;
};

REFL_IMPL_CLASS_BEGIN(Dummy);
    REFL_MEMBER(Dummy, test, REFL_TYPE_INT32);
REFL_IMPL_CLASS_END(Dummy);

