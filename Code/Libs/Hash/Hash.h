/*
   GameRiff - Framework for creating various video game services
   Service for hashing data and strings
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

#define HASH64(token, value)     Hash64(TOWSTR(token), value##ULL)
#define HASH32(token, value)     Hash32(TOWSTR(token), value##ULL)
#define CHASH64(token, value)    (value##ULL)
#define CHASH32(token, value)    (value##ULL)

class Hash64 {
public:
    Hash64() :
        m_hash(0)
    {
    }
    Hash64(const chargr * str);
    Hash64(const charsys * str);
    Hash64(const chargr * str, uint64 hash);
    Hash64(const charsys * str, uint64 hash);
    Hash64(const void * data, unsigned length);

    bool operator == (const Hash64 rhs) const {
        return m_hash == rhs.m_hash;
    }
    bool operator != (const Hash64 rhs) const {
        return m_hash != rhs.m_hash;
    }
private:
    uint64 m_hash;
};

class Hash32 {
public:
    Hash32() :
        m_hash(0)
    {
    }
    Hash32(const chargr * str);
    Hash32(const charsys * str);
    Hash32(const chargr * str, uint32 hash);
    Hash32(const charsys * str, uint32 hash);
    Hash32(const void * data, unsigned length);

    bool operator == (const Hash32 rhs) const {
        return m_hash == rhs.m_hash;
    }
    bool operator != (const Hash32 rhs) const {
        return m_hash != rhs.m_hash;
    }
private:
    uint32 m_hash;
};

//
// Hash string functions convert the string to all one case to eliminate 
//  issues with mixed casing giving different hashes
//
Hash64 HashString64(
    const chargr * str
);
Hash64 HashString64(
    const charsys * str
);

Hash32 HashString32(
    const chargr * str
);

Hash32 HashString32(
    const charsys * str
);

Hash64 HashData64(
    const void    * data,
    unsigned        length
);

Hash32 HashData32(
    const void    * data,
    unsigned        length
);

