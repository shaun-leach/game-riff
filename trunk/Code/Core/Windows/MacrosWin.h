/*
   GameRiff - Framework for creating various video game services
   Windows specific macros
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

//////////////////////////////////////////////////////
//
// Calculate the offset of a member from the base pointer
//

#define OFFSETOF(parent, member)    ((unsigned) &(((parent *)(0x0))->member))

//////////////////////////////////////////////////////
//
// Calculate the offset of base classes from the base pointer
//

#define CLASSOFFSETOF(base, derived) ((unsigned) (static_cast<base *>((derived *)(0x4))) - 4)

//////////////////////////////////////////////////////
//
// Calculate the size of a member
//

#define SIZEOF(parent, member)    (sizeof(((((parent *)(0x0))->member))))

//////////////////////////////////////////////////////
//
// Convert string to wide char string
//

#define TOWCHAR(str)    (L ## str)

//////////////////////////////////////////////////////
//
// Convert a symbol to a string
//

#define TOWSTR(str)    TOWCHAR(#str)

//////////////////////////////////////////////////////
//
// Convert a symbol to a string for non-release builds
//

#ifdef RELEASE
    #define TOWSTR_NORELEASE(str)    NULL
#else
    #define TOWSTR_NORELEASE(str)    TOWCHAR(#str)
#endif

//////////////////////////////////////////////////////
//
// Used as a reminder that code is untested
//

void WinDebugBreak();
#define TESTME(str)    WinDebugBreak()
