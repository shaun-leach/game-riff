/*
   GameRiff - Framework for creating various video game services
   Windows implementation of basic assert function
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

#define BUFFER_SIZE (512)
static chargr s_stringBuffer[BUFFER_SIZE];

//////////////////////////////////////////////////////
//
// External functions
//

//====================================================
void WinDebugBreak() {
    DebugBreak();
}

//====================================================
void AssertHandler(
    const chargr * expression,
    const chargr * message,
    const chargr * file,
    unsigned       line
) {
    StrPrintf(s_stringBuffer, BUFFER_SIZE, L"%s(%d): ASSERT! %s:%s\n", file, line, message, expression);
    wprintf(s_stringBuffer);
    OutputDebugStringW(s_stringBuffer);
    DebugBreak();
}

//====================================================
void AssertHandler(
    const chargr * expression,
    const chargr * message,
    const char   * file,
    unsigned       line
) {
    StrPrintf(s_stringBuffer, BUFFER_SIZE, L"%S(%d): ASSERT! %s:%s\n", file, line, message, expression);
    wprintf(s_stringBuffer);
    OutputDebugStringW(s_stringBuffer);
    DebugBreak();
}

