/*
   GameRiff - Framework for creating various video game services
   String services interface
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

#pragma once

unsigned StrLen(const chargr  * str, unsigned len);
unsigned StrLen(const charsys * str, unsigned len);

int StrCmp(const chargr  * strA, const chargr * strB, unsigned len);
int StrCmp(const charsys * strA, const charsys * strB, unsigned len);
int StrCmp(const chargr  * strA, const charsys * strB, unsigned len);
int StrCmp(const charsys  * strA, const chargr * strB, unsigned len);
int StrICmp(const chargr  * strA, const chargr * strB, unsigned len);
int StrICmp(const charsys * strA, const charsys * strB, unsigned len);
int StrICmp(const chargr  * strA, const charsys * strB, unsigned len);
int StrICmp(const charsys  * strA, const chargr * strB, unsigned len);

int StrCopy(chargr  * dest, unsigned len, const chargr  * src);
//int StrCopy(chargr  * dest, unsigned len, const charsys * src);
//int StrCopy(charsys * dest, unsigned len, const chargr  * src);
int StrCopy(charsys * dest, unsigned len, const charsys * src);

void StrConvertToUtf8(const chargr * src, charsys * dest, unsigned len);
charsys * StrCreateUtf8(const chargr * src, MemFlags flags);

void StrUtf8ConvertToCharGr(const charsys * src, chargr * dest, unsigned len);
chargr * StrCreateCharGr(const charsys * src, MemFlags flags);

int StrPrintf(charsys * dest, unsigned len, const charsys * format, ...);
int StrPrintf(chargr  * dest, unsigned len, const chargr * format, ...);
int StrPrintfV(charsys * dest, unsigned len, const charsys * format, va_list vargs);
int StrPrintfV(chargr  * dest, unsigned len, const chargr * format, va_list vargs);

int StrReadValue(const chargr * src, unsigned len, const chargr * format, ...);

class StrStackConverter {
public:
    StrStackConverter(const chargr * str, unsigned maxLen);
    ~StrStackConverter();

    operator const charsys * () {
        return m_str;
    }
private:
    charsys * m_str;
};
