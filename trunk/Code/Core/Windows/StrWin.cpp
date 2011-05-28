/*
   GameRiff - Framework for creating various video game services
   Windows implementaion of various string functions
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
int StrICmp(const chargr  * strA, const chargr * strB, unsigned len) {
    return _wcsnicmp(strA, strB, len);
}

//====================================================
int StrICmp(const charsys * strA, const charsys * strB, unsigned len) {
    return _strnicmp(strA, strB, len);
}

//====================================================
void StrConvertToUtf8(const chargr * src, charsys * dest, unsigned len) {
    size_t bytesConverted = 0;
    wcstombs_s(&bytesConverted, dest, len, src, len);
}

//====================================================
void StrUtf8ConvertToCharGr(const charsys * src, chargr * dest, unsigned len) {
    size_t bytesConverted = 0;
    mbstowcs_s(&bytesConverted, dest, len, src, len);
}

//====================================================
int StrPrintf(chargr * dest, unsigned len, const chargr * format, ...) {
    va_list args;
    va_start(args, format);
    return vswprintf_s(dest, len, format, args);
}

//====================================================
int StrPrintf(charsys * dest, unsigned len, const charsys * format, ...) {
    va_list args;
    va_start(args, format);
    return vsprintf_s(dest, len, format, args);
}

//====================================================
int StrReadValue(const chargr * src, unsigned len, const chargr * format, ...) {
    va_list args;
    va_start(args, format);
    void * arg = va_arg(args, void *);
    return _snwscanf_s(src, len, format, arg);
}

//====================================================
int StrScanf(const charsys * src, unsigned len, const charsys * format, ...) {
    va_list args;
    va_start(args, format);
    TESTME("Need to properly handle the var args");
    return _snscanf_s(src, len, format, args);
}
