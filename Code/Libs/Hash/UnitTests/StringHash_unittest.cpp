/*
   GameRiff - Framework for creating various video game services
   String hashing unit tests
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

TEST(StringHashTest, TestUniqueness64) {
    EXPECT_EQ(HashString64("a"), HashString64("a"));
    EXPECT_EQ(HashString64(""), HashString64(""));
    EXPECT_EQ(HashString64("art/planets/textures/gasGiant1.dds"), HashString64("art/planets/textures/gasGiant1.dds"));

    EXPECT_NE(HashString64("a"), HashString64("b"));
    EXPECT_NE(HashString64("abcd"), HashString64("abcde"));
    EXPECT_NE(HashString64("art/planets/textures/gasGiant1.dds"), HashString64("art/planets/textures/gasGiant2.dds"));
    EXPECT_NE(HashString64("abcd"), HashString64("abce"));
}

TEST(StringHashTest, TestCaseInsensitivity64) {
    EXPECT_EQ(HashString64("abcdefg"), HashString64("ABCDEFG"));
    EXPECT_EQ(HashString64("ArT/PlAnEtS/TeXtUrEs/GaSgIant1.DdS"), HashString64("art/planets/textures/gasGiant1.dds"));
    EXPECT_NE(HashString64("abcdefg"), HashString64("ABCDEFGH"));
}

TEST(StringHashTest, TestUniqueness32) {
    EXPECT_EQ(HashString32("a"), HashString32("a"));
    EXPECT_EQ(HashString32(""), HashString32(""));
    EXPECT_EQ(HashString32("art/planets/textures/gasGiant1.dds"), HashString32("art/planets/textures/gasGiant1.dds"));

    EXPECT_NE(HashString32("a"), HashString32("b"));
    EXPECT_NE(HashString32("abcd"), HashString32("abcde"));
    EXPECT_NE(HashString32("art/planets/textures/gasGiant1.dds"), HashString32("art/planets/textures/gasGiant2.dds"));
    EXPECT_NE(HashString32("abcd"), HashString32("abce"));
}

TEST(StringHashTest, TestCaseInsensitivity32) {
    EXPECT_EQ(HashString32("abcdefg"), HashString32("ABCDEFG"));
    EXPECT_EQ(HashString32("ArT/PlAnEtS/TeXtUrEs/GaSgIant1.DdS"), HashString32("art/planets/textures/gasGiant1.dds"));
    EXPECT_NE(HashString32("abcdefg"), HashString32("ABCDEFGH"));
}

TEST(StringHashTest, TestWideChar) {
    EXPECT_EQ(HashString64(L"a"), HashString64(L"a"));
    EXPECT_EQ(HashString64(L"art/planets/textures/gasGiant1.dds"), HashString64(L"art/planets/textures/gasGiant1.dds"));

    EXPECT_NE(HashString64("art/planets/textures/gasGiant1.dds"), HashString64(L"art/planets/textures/gasGiant1.dds"));
}
