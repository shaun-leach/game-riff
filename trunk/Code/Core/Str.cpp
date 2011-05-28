/*
   GameRiff - Framework for creating various video game services
   String services
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

//====================================================
unsigned StrLen(const chargr  * str) {
    return wcslen(str);
}

//====================================================
unsigned StrLen(const charsys * str) {
    return strlen(str);
}

//====================================================
int StrCmp(const chargr  * strA, const chargr * strB, unsigned len) {
    return wcsncmp(strA, strB, len);
}

//====================================================
int StrCmp(const charsys * strA, const charsys * strB, unsigned len) {
    return strncmp(strA, strB, len);
}

//====================================================
int StrCmp(const chargr  * strA, const charsys * strB, unsigned len) {
    StrStackConverter sysA(strA);
    return StrCmp(sysA, strB, len);
}

//====================================================
int StrCmp(const charsys  * strA, const chargr * strB, unsigned len) {
    StrStackConverter sysB(strB);
    return StrCmp(strA, sysB, len);
}

//====================================================
int StrICmp(const chargr  * strA, const charsys * strB, unsigned len) {
    StrStackConverter sysA(strA);
    return StrCmp(sysA, strB, len);
}

//====================================================
int StrICmp(const charsys  * strA, const chargr * strB, unsigned len) {
    StrStackConverter sysB(strB);
    return StrCmp(strA, sysB, len);
}

//====================================================
StrStackConverter::StrStackConverter(const chargr * str) {
    unsigned len = StrLen(str);
    m_str = new(MemFlags(MEM_ARENA_TEMP, MEM_CAT_STRING)) charsys[len + 1];
    StrConvertToUtf8(str, m_str, len + 1);
}

//====================================================
StrStackConverter::~StrStackConverter() {
    if (m_str != NULL) {
        delete [] m_str;
        m_str = NULL;
    }
}

//====================================================
charsys * StrCreateUtf8(const chargr * src, MemFlags flags) {
    unsigned len = 2 * wcslen(src);
    charsys * dest = new(flags) charsys[len];

    StrConvertToUtf8(src, dest, len);

    return dest;
}
