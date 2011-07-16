/*
   GameRiff - Framework for creating various video game services
   Ref counted types, SmartPtrs, WeakPtrs. Oh My!
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
// Basic reference counted type
//

class RefCounted {
public:
    RefCounted() :
        m_refCount(0)
    {
    }

    unsigned AddRef() {
        unsigned refCount = m_refCount;
        m_refCount++;
        return refCount;
    }
    unsigned ReleaseRef() {
        unsigned refCount = m_refCount;
        ASSERTGR(m_refCount > 0);
        m_refCount--;
        return refCount;
    }

private:
    unsigned m_refCount;
};

//////////////////////////////////////////////////////
//
// SmartPtr template
//

template <typename t_type>
class SmartPtr {
public:
    SmartPtr(t_type * ptr) :
        m_ptr(ptr)
    {
        if (m_ptr != NULL) 
            m_ptr->AddRef();
    }
    SmartPtr(const SmartPtr<t_type> & rhs) :
        m_ptr(rhs.m_ptr)
    {
        if (m_ptr != NULL) 
            m_ptr->AddRef();
    }

    ~SmartPtr() {
        ReleaseRef();
    }

    SmartPtr & operator=(const SmartPtr & rhs) {
        if (m_ptr != NULL) {
            ReleaseRef();
            m_ptr = NULL;
        }
        m_ptr = rhs.m_ptr;
        if (m_ptr != NULL) 
            m_ptr->AddRef();

        return *this;
    }

    t_type * operator->() {
        ASSERTGR(m_ptr != NULL);
        return m_ptr;
    }

    bool operator==(unsigned nullPtr) const {
        return m_ptr == reinterpret_cast<t_type *>(nullPtr);
    }

    bool operator!=(unsigned nullPtr) const {
        return m_ptr != reinterpret_cast<t_type *>(nullPtr);
    }

    bool operator!=(const SmartPtr<t_type> & rhs) const {
        return m_ptr != rhs.m_ptr;
    }
private:
    void ReleaseRef() {
        if (m_ptr != NULL) {
            unsigned refCount = m_ptr->ReleaseRef();
            if (refCount == 1) {
                delete m_ptr;
                m_ptr = NULL;
            }
        }
    }
    t_type * m_ptr;
};

#define DECLARE_SMARTPTR(type)                                              \
    typedef SmartPtr<type>          type##Ptr;                              \
    typedef SmartPtr<const type>    type##ConstPtr
