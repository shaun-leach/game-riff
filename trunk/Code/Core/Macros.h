/*
   GameRiff - Framework for creating various video game services
   Platform independent macros
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

//Define _XENON first seen as xbox defines WIN32 as well.
#ifdef _XENON

#elif defined(PS3)
    // Nothing for now
#elif defined(WIN32)
    #include "Windows/MacrosWin.h"
#endif

//////////////////////////////////////////////////////
//
// Concatanate 2 strings, need a double macro so the preprocessor
//  properly concatenates things
//

#define CONCAT_SYMBOLS_(sym1, sym2) sym1 ## sym2
#define CONCAT_SYMBOLS(sym1, sym2) CONCAT_SYMBOLS_(sym1,  sym2)

//////////////////////////////////////////////////////
//
// Make a unique symbol
//

#define UNIQUEFY_SYMBOL(sym) CONCAT_SYMBOLS(sym, __LINE__)

//////////////////////////////////////////////////////
//
// Get the number of elements in a fixed size array
//

#define NUM_ARRAY_ELEMENTS(arr) (sizeof(arr) / sizeof(arr[0]))

