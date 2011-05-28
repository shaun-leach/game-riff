/*
   GameRiff - Framework for creating various video game services
   Hash function unit tests
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

#define DATA_A      ("a")
#define LEN_A       (strlen(DATA_A))
#define DATA_B      ("b")
#define LEN_B       (strlen(DATA_B))
#define DATA_FILE1  ("art/planets/textures/gasGiant1.dds")
#define LEN_FILE1   (strlen(DATA_FILE1))
#define DATA_FILE2  ("art/planets/textures/gasGiant2.dds")
#define LEN_FILE2   (strlen(DATA_FILE2))

#define DATA_1      ("abcd")
#define LEN_1       (strlen(DATA_1))
#define DATA_2      ("abcde")
#define LEN_2       (strlen(DATA_2))

TEST(HashDataTest, TestUniqueness64) {
    EXPECT_EQ(HashData64(DATA_A, LEN_A), HashData64(DATA_A, LEN_A));
    EXPECT_EQ(HashData64(DATA_FILE1, LEN_FILE1), HashData64(DATA_FILE1, LEN_FILE1));

    EXPECT_NE(HashData64(DATA_A, LEN_A), HashData64(DATA_B, LEN_B));
    EXPECT_NE(HashData64(DATA_1, LEN_1), HashData64(DATA_2, LEN_2));
    EXPECT_NE(HashData64(DATA_FILE1, LEN_FILE1), HashData64(DATA_FILE2, LEN_FILE2));
}

TEST(HashDataTest, TestUniqueness32) {
    EXPECT_EQ(HashData32(DATA_A, LEN_A), HashData32(DATA_A, LEN_A));
    EXPECT_EQ(HashData32(DATA_FILE1, LEN_FILE1), HashData32(DATA_FILE1, LEN_FILE1));

    EXPECT_NE(HashData32(DATA_A, LEN_A), HashData32(DATA_B, LEN_B));
    EXPECT_NE(HashData32(DATA_1, LEN_1), HashData32(DATA_2, LEN_2));
    EXPECT_NE(HashData32(DATA_FILE1, LEN_FILE1), HashData32(DATA_FILE2, LEN_FILE2));
}


