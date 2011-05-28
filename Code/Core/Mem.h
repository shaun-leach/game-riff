/*
   GameRiff - Framework for creating various video game services
   Memory interface
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


// Packed into 4 bits
enum EMemArena {
    MEM_ARENA_DEFAULT       =  0,
    MEM_ARENA_GLOBAL        =  1, // Allocations that are persistent for the life time of the app
    MEM_ARENA_TRANSIENT     =  2, // Temporary allocations > 1 frame
    MEM_ARENA_TEMP          =  3, // Temporary allocations < 1 frame
    MEM_ARENA_NEXT          =  4,
    MEM_ARENA_MAX           = 15, // Can't have more than 15 so we can pack it into 4 bits
};

// Packed into 3 bits
enum EMemAlignment {
    MEM_ALIGN_DEFAULT,
    MEM_ALIGN_4,
    MEM_ALIGN_16,
    MEM_ALIGN_128,
    MEM_ALIGN_1024,
    MEM_ALIGN_4096,
};

// Packed into 7 bits
enum EMemCategory {
    MEM_CAT_UNCATEGORIZED,
    MEM_CAT_FILEIO,
    MEM_CAT_STRING,
    MEM_CAT_TEST,
    MEM_CAT_XML,
};

class MemFlags {
public:
    MemFlags(EMemArena arena, EMemCategory cat) :
        m_arena(arena),
        m_align(MEM_ALIGN_DEFAULT),
        m_category(cat),
        m_OOMSafe(0),
        m_padding(0) 
    {
    }
    MemFlags(EMemArena arena, EMemCategory cat, EMemAlignment align) :
        m_arena(arena),
        m_align(align),
        m_category(cat),
        m_OOMSafe(0),
        m_padding(0) 
    {
    }

    EMemArena GetArena() const {
        return (EMemArena) m_arena;
    }

    EMemAlignment GetAlignment() const {
        return (EMemAlignment) m_align;
    }

    EMemCategory GetCategory() const {
        return (EMemCategory) m_category;
    }

private:
    unsigned m_arena    :  4;
    unsigned m_align    :  3;
    unsigned m_category :  7;
    unsigned m_OOMSafe  :  1; // If set the caller handles OOM directly, otherwise the allocater should assert
    unsigned m_padding  : 17;
};

void  * MemAlloc(unsigned size, MemFlags flags);
void    MemFree(void * mem);

//////////////////////////////////////////////////////
//
// new operators
//
inline void * operator new(size_t size) {
    return MemAlloc(size, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_UNCATEGORIZED));
}

inline void * operator new[](size_t size) {
    return MemAlloc(size, MemFlags(MEM_ARENA_DEFAULT, MEM_CAT_UNCATEGORIZED));
}

inline void * operator new(size_t size, MemFlags flags) {
    return MemAlloc(size, flags);
}

inline void * operator new[](size_t size, MemFlags flags) {
    return MemAlloc(size, flags);
}

//////////////////////////////////////////////////////
//
// delete operators
//
inline void operator delete(void * mem) {
    MemFree(mem);
}

inline void operator delete[](void * mem) {
    MemFree(mem);
}

inline void operator delete(void * mem, MemFlags flags) {
    return MemFree(mem);
}

inline void operator delete[](void * mem, MemFlags flags) {
    return MemFree(mem);
}

//////////////////////////////////////////////////////
//
// placement new operators (don't remember why I need these)
//
/*inline void * operator new(size_t size, void * mem) {
    return mem;
}
inline void * operator new[](size_t size, void * mem) {
    return mem;
}*/

