/*
   GameRiff - Framework for creating various video game services
   Memory Subsystem Header File
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

void AssertHandler(
    const chargr * expression,
    const chargr * message,
    const char   * file,
    unsigned       line
);

void AssertHandler(
    const chargr * expression,
    const chargr * message,
    const chargr * file,
    unsigned       line
);

#define ASSERTGR(exp)                                                          \
    do {                                                                       \
        if (!(exp))                                                            \
            AssertHandler(TOWCHAR(#exp), TOWCHAR(""), __FILE__, __LINE__);     \
    } while(false)
#define ASSERTMSGGR(exp, msg)                                                  \
    do {                                                                       \
        if (!(exp))                                                            \
            AssertHandler(TOWCHAR(#exp), TOWCHAR(msg), __FILE__, __LINE__);    \
    } while(false)

#ifdef _DEBUG
    #define ASSERTGR_DGB(exp)           ASSERTGR(exp)
    #define ASSERTMSGGR_DGB(exp, msg)   ASSERTMSGGR(exp, msg)
#else
    #define ASSERTGR_DGB(exp)
    #define ASSERTMSGGR_DGB(exp, msg)
#endif

#define COMPILERASSERTGR(exp) typedef int UNIQUEFY_SYMBOL(COMPILERASSERTIMP_) [(exp) ? 1 : 0]
