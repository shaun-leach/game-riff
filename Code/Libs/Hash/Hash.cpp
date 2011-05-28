/*
   GameRiff - Framework for creating various video game services
   Functions for hashing strings and data
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

#include "Pch.h"

//////////////////////////////////////////////////////
//
// Constants
//

#define INIT_A (0xabcdefab)
#define INIT_B (0x12345678)

//////////////////////////////////////////////////////
//
// External functions
//

//====================================================
Hash64::Hash64(const charsys * str) {
    m_hash = NSLookup3::Lookup3HashString64(str, INIT_A, INIT_B);
}

//====================================================
Hash64::Hash64(const chargr * str) {
    m_hash = NSLookup3::Lookup3HashString64(str, INIT_A, INIT_B);
}

//====================================================
Hash64::Hash64(const void * data, unsigned length) {
    m_hash = NSLookup3::Lookup3Hash64(data, length, INIT_A, INIT_B);
}

//====================================================
Hash32::Hash32(const charsys * str) {
    m_hash = NSLookup3::Lookup3HashString32(str, INIT_A);
}

//====================================================
Hash32::Hash32(const chargr * str) {
    m_hash = NSLookup3::Lookup3HashString32(str, INIT_A);
}

//====================================================
Hash32::Hash32(const void * data, unsigned length) {
    m_hash = NSLookup3::Lookup3Hash32(data, length, INIT_A);
}

//====================================================
Hash64 HashString64(
    const charsys * str
) {
    return Hash64(str);
}

//====================================================
Hash64 HashString64(
    const chargr * str
) {
    return Hash64(str);
}

//====================================================
Hash32 HashString32(
    const charsys * str
) {
    return Hash32(str);
}

//====================================================
Hash32 HashString32(
    const chargr * str
) {
    return Hash32(str);
}

//====================================================
Hash64 HashData64(
    const void    * data,
    unsigned        length
) {
    return Hash64(data, length);
}

//====================================================
Hash32 HashData32(
    const void    * data,
    unsigned        length
) {
    return Hash32(data, length);
}

