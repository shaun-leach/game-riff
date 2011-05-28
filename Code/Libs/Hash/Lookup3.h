/*
   GameRiff - Framework for creating various video game services
   Internal interface to hashing functions
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

namespace NSLookup3 {

uint32 hashword(
    const uint32 * k,                   /* the key, an array of uint32 values */
    size_t         length,               /* the length of the key, in uint32_ts */
    uint32         initval
);

void hashword2 (
    const uint32 * k,                   /* the key, an array of uint32 values */
    size_t         length,               /* the length of the key, in uint32_ts */
    uint32       * pc,                      /* IN: seed OUT: primary hash value */
    uint32       * pb
);

uint32 Lookup3Hash32( 
    const void * key, 
    size_t       length, 
    uint32       initval
);

uint64 Lookup3Hash64(
    const void    * key,       /* the key to hash */
    size_t          length,    /* length of the key */
    uint32          pc,        /* IN: primary initval, OUT: primary hash */
    uint32          pb
);

uint32 Lookup3HashString32( 
    const char * key, 
    uint32       initval
);

uint64 Lookup3HashString64(
    const char * key,       /* the key to hash */
    uint32       pc,        /* IN: primary initval, OUT: primary hash */
    uint32       pb
);

uint32 Lookup3HashString32( 
    const wchar_t * key, 
    uint32          initval
);

uint64 Lookup3HashString64(
    const wchar_t * key,       /* the key to hash */
    uint32          pc,        /* IN: primary initval, OUT: primary hash */
    uint32          pb
);

} // namespace NSLookup3
